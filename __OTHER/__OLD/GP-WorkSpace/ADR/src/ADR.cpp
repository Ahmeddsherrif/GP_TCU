/*
 * ADR.cpp
 *
 *  Created on: Jun 28, 2023
 *      Author: ahmed
 */

#include "main.h"

#include "ADR.h"

#define KMH_TO_MS(speed) 								(speed*1000.0/3600.0)
#define MILLISECONDS_TO_SECONDS(inputMilliseconds)		(inputMilliseconds / 1000)

constexpr double pi = 3.14159265358979323846;
constexpr double earthRadius = 6371000.0; // Earth's radius in meters

double ADR::averageSpeed = 0.0;
double ADR::averageYawRate = 0.0;

//Estimated Variables
double ADR::currentEstimatedDistance = 0.0;
double ADR::currentEstimatedBearingAngle = 0.0;
GPSLocation ADR::currentEstimatedGPSLocation = { 0.0, 0.0 };

//True Variables
GPSLocation ADR::currentTrueGPSSample = { 0, 0 };
GPSLocation ADR::previousTrueGPSSample = { 0, 0 };
double ADR::currentTrueBearingAngle = 0.0;

int ADR::secondsSinceLastOutage = 0;

string ADR::getADR_GPS(queue<double> &speedQueue, mutex &speedMutex, queue<double> &gyroQueue, mutex &gyroMutex,
		queue<GPSLocation> &gpsQueue, mutex &gpsMutex, outageStatus &currentOutageStatus,
		mutex &mutexCurrentOutageStatus) {

	// get GPS Data
	unique_lock<mutex> lock_gpsMutex(gpsMutex);
	if (gpsQueue.empty() == false) {
		currentTrueGPSSample = gpsQueue.front();
		gpsQueue.pop();
	}
	lock_gpsMutex.unlock();

	// Average speed data
	unique_lock<mutex> lock_speedMutex(speedMutex);
	if (speedQueue.empty() == false) {
		averageSpeed = sumQueue(speedQueue);
	}
	lock_speedMutex.unlock();

	// get andgle diffrence data
	unique_lock<mutex> lock_gyroMutex(gyroMutex);
	if (gyroQueue.empty() == false) {
		averageYawRate = calculateQueueDifference(gyroQueue);
	}
	lock_gyroMutex.unlock();

	//Always Calculate Bearing -- FOr Demo Perposes only
	currentTrueBearingAngle = calculateBearingAngle(previousTrueGPSSample, currentTrueGPSSample);
	previousTrueGPSSample = currentTrueGPSSample;

	// switch based on outage status

	unique_lock<mutex> lockMutexCurrentOutageStatus(mutexCurrentOutageStatus);
	switch (currentOutageStatus) {
		case NO_OUTAGE: {
			secondsSinceLastOutage = 0;
			currentEstimatedBearingAngle = currentTrueBearingAngle;
			currentEstimatedGPSLocation = currentTrueGPSSample;

			break;
		}
		case OUTAGE: {
			secondsSinceLastOutage++;
			currentEstimatedDistance = KMH_TO_MS(averageSpeed) * MILLISECONDS_TO_SECONDS((double )40);
			currentEstimatedBearingAngle += averageYawRate;
			currentEstimatedGPSLocation = calculateDeadReckoningGPS(currentEstimatedGPSLocation,
					currentEstimatedDistance, currentEstimatedBearingAngle);
			break;
		}
	}
	lockMutexCurrentOutageStatus.unlock();

	//return the ADR string		LAT,LONG,Seconds_since_last_outage
	string rtnString = to_string(currentEstimatedGPSLocation.latitude) + ","
			+ to_string(currentEstimatedGPSLocation.longitude) + "," + to_string(secondsSinceLastOutage);

	return rtnString;
}

GPSLocation ADR::calculateDeadReckoningGPS(GPSLocation initialGPS, double distance, double bearing) {
	double lat1 = initialGPS.latitude * pi / 180.0; // convert to radians
	double lon1 = initialGPS.longitude * pi / 180.0;

	double heading = bearing * pi / 180.0; // convert to radians
	// Calculate new latitude using dead reckoning formula
	double lat2 = asin(
			sin(lat1) * cos(distance / earthRadius) + cos(lat1) * sin(distance / earthRadius) * cos(heading));
	// Calculate new longitude using dead reckoning formula
	double lon2 = lon1
			+ atan2(sin(heading) * sin(distance / earthRadius) * cos(lat1),
					cos(distance / earthRadius) - sin(lat1) * sin(lat2));

	// Convert back to degrees
	GPSLocation newGPS;
	newGPS.latitude = lat2 * 180.0 / pi;
	newGPS.longitude = lon2 * 180.0 / pi;
	return newGPS;
}

double ADR::calculateBearingAngle(GPSLocation fromGPS, GPSLocation toGPS) {
	double lat1 = fromGPS.latitude * pi / 180.0;
	double lon1 = fromGPS.longitude * pi / 180.0;
	double lat2 = toGPS.latitude * pi / 180.0;
	double lon2 = toGPS.longitude * pi / 180.0;

	double y = sin(lon2 - lon1) * cos(lat2);
	double x = cos(lat1) * sin(lat2) - sin(lat1) * cos(lat2) * cos(lon2 - lon1);
	double bearing = atan2(y, x);

	// Convert bearing to degrees
	return bearing * 180.0 / pi;
}

double ADR::calculateDegreeDifference(double start_angle, double end_angle) {
	double diff = fmod(end_angle - start_angle + 540, 360) - 180;
	if (diff < -180) {
		diff += 360;
	}
	return diff;
}

double ADR::calculateQueueDifference(queue<double> &angle_queue) {
	double start_angle = angle_queue.front();
	double end_angle = angle_queue.back();

	//Clear Queue
	while (!angle_queue.empty()) {
		angle_queue.pop();
	}

	return calculateDegreeDifference(start_angle, end_angle);
}

double ADR::sumQueue(queue<double> &q) {
	double sum = 0.0;

	int count = q.size();

	while (!q.empty()) {
		sum += q.front();
		q.pop();
	}

	if (count == 0) {
		return 0.0;
	}

	return sum;
}

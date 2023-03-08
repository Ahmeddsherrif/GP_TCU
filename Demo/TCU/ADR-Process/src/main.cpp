#include "main.h"

GPSLocation calculateDeadReckoningGPS(GPSLocation initialGPS, double distance, double bearing) {
	double lat1 = initialGPS.latitude * pi / 180.0; // convert to radians
	double lon1 = initialGPS.longitude * pi / 180.0;

	double heading = bearing * pi / 180.0; // convert to radians
	// Calculate new latitude using dead reckoning formula
	double lat2 = asin(sin(lat1) * cos(distance / earthRadius) + cos(lat1) * sin(distance / earthRadius) * cos(heading));
	// Calculate new longitude using dead reckoning formula
	double lon2 = lon1 + atan2(sin(heading) * sin(distance / earthRadius) * cos(lat1), cos(distance / earthRadius) - sin(lat1) * sin(lat2));

	// Convert back to degrees
	GPSLocation newGPS;
	newGPS.latitude = lat2 * 180.0 / pi;
	newGPS.longitude = lon2 * 180.0 / pi;
	return newGPS;
}

double calculateBearingAngle(GPSLocation fromGPS, GPSLocation toGPS) {
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


double calculateDegreeDifference(double start_angle, double end_angle) {
    double diff = fmod(end_angle - start_angle + 540, 360) - 180;
    if (diff < -180) {
        diff += 360;
    }
    return diff;
}


double calculateQueueDifference(queue<double>& angle_queue) {
    double start_angle = angle_queue.front();
    double end_angle = angle_queue.back();

    //Clear Queue
	while (!angle_queue.empty()) {
		angle_queue.pop();
	}

    return calculateDegreeDifference(start_angle, end_angle);
}


//string getTimestamp() {
//
//	// Get the current time point
//	auto now = chrono::system_clock::now();
//
//	// Convert the time point to a time_t object
//	auto currentTime = chrono::system_clock::to_time_t(now);
//
//	// Extract the microseconds from the time point
//	auto microseconds = chrono::duration_cast<chrono::microseconds>(now.time_since_epoch()) % 1000000;
//
//	// Convert the time_t object to a local time struct
//	tm *localTime = localtime(&currentTime);
//
//	// Format the timestamp string
//	char timestamp[100];
//	strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", localTime);
//
//	// Append the microseconds to the string
//	sprintf(timestamp, "%s.%06d", timestamp, static_cast<int>(microseconds.count()));
//
//	return string(timestamp);
//}

double sumQueue(queue<double> &q) {
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

mutex isThreadStarted_mutex;
condition_variable cv;
bool isThreadStarted = false;
bool isProgramFinished = false;

queue<double> speedometerSensorReadings_queue;
mutex speedometerSensorReadings_queueMutex;
constexpr int speedometerSampleRate = 4;	//40;
constexpr int maxSpeedSampleChange = 3;
void SpeedometerSensor_thread() {

	SpeedometerSensor speedometer("/home/ahmed/Desktop/ADR-DATA/Speedx.csv");
	int sampleNumber = 0;
	const int maxSampleNumber = speedometer.getNumberOfSamples() - 1;
	double currentSpeedSample;
	double previousSpeedSample = speedometer.getSpeed(sampleNumber);

	unique_lock<mutex> isThreadStarted_mutexLock(isThreadStarted_mutex);
	while (!isThreadStarted) {
		cv.wait(isThreadStarted_mutexLock);
	}
	isThreadStarted_mutexLock.unlock();

	// Define the start time of the loop
	auto startTime = high_resolution_clock::now();

	// Define the next time the loop should start
	auto nextLoopTime = startTime + milliseconds(speedometerSampleRate);

	while (isProgramFinished == false) {

		// Data Acquisition
		currentSpeedSample = speedometer.getSpeed(sampleNumber);

		// Data Cleaning
		if (abs(currentSpeedSample - previousSpeedSample) > maxSpeedSampleChange) {
			currentSpeedSample = previousSpeedSample;
		}

		previousSpeedSample = currentSpeedSample;

		unique_lock<mutex> speedometerSensorReadingsQueueMutexLock(speedometerSensorReadings_queueMutex);
		speedometerSensorReadings_queue.push(currentSpeedSample);
		speedometerSensorReadingsQueueMutexLock.unlock();

		sampleNumber++;

		if (sampleNumber >= maxSampleNumber) {
			isProgramFinished = true;
		}

		// Sleep until the next loop period
		this_thread::sleep_until(nextLoopTime);

		// Calculate the next loop time
		nextLoopTime += milliseconds(speedometerSampleRate);
	}
}

queue<GPSReading> gpsSensorReadings_queue;
mutex gpsSensorReadings_queueMutex;
constexpr int gpsSampleRate = 100;	//1000;
void GPSSensor_thread() {

	GPSSensor gps("/home/ahmed/Desktop/ADR-DATA/Positionx.csv");
	int sampleNumber = 0;
	const int maxSampleNumber = gps.getNumberOfSamples() - 1;
	GPSLocation currentGPSSample;
	GPSStatus currentGPSStatus = GPSStatus::AVAILABLE;
	GPSReading currentGPSReading;

	unique_lock<mutex> isThreadStarted_mutexLock(isThreadStarted_mutex);
	while (!isThreadStarted) {
		cv.wait(isThreadStarted_mutexLock);
	}
	isThreadStarted_mutexLock.unlock();

	// Define the start time of the loop
	auto startTime = high_resolution_clock::now();

	// Define the next time the loop should start
	auto nextLoopTime = startTime + milliseconds(gpsSampleRate);

	while (isProgramFinished == false) {

		// Data Manipulation -- To Simulate an Outage   10/120
		if ((sampleNumber > 10 && sampleNumber < 100 )||(sampleNumber > 200 && sampleNumber < 400 ) ||(sampleNumber > 500 && sampleNumber < 600 )) {
			currentGPSStatus = GPSStatus::UNAVALIABLE;
		} else {
			currentGPSStatus = GPSStatus::AVAILABLE;
		}

		// Data Acquisition
		currentGPSSample = gps.getGPS(sampleNumber);
		currentGPSReading.Location = currentGPSSample;
		currentGPSReading.Status = currentGPSStatus;

		unique_lock<mutex> gpsSensorReadingsQueueMutexLock(gpsSensorReadings_queueMutex);
		gpsSensorReadings_queue.push(currentGPSReading);
		gpsSensorReadingsQueueMutexLock.unlock();

		sampleNumber++;

		if (sampleNumber >= maxSampleNumber) {
			isProgramFinished = true;
		}

		// Sleep until the next loop period
		this_thread::sleep_until(nextLoopTime);

		// Calculate the next loop time
		nextLoopTime += milliseconds(gpsSampleRate);
	}
}

queue<double> gyroSensorReadings_queue;
mutex gyroSensorReadings_queueMutex;
constexpr int gyroSampleRate = 1;	//10;
constexpr double gyroStaticBias = 0;
void GyroSensor_thread() {

	GyroscopeSensor gyro("/home/ahmed/Desktop/ADR-DATA/Orientationx.csv");
	int sampleNumber = 0;
	const int maxSampleNumber = gyro.getNumberOfSamples() - 1;
	double currentGyroSample;

	unique_lock<mutex> isThreadStarted_mutexLock(isThreadStarted_mutex);
	while (!isThreadStarted) {
		cv.wait(isThreadStarted_mutexLock);
	}
	isThreadStarted_mutexLock.unlock();

	// Define the start time of the loop
	auto startTime = high_resolution_clock::now();

	// Define the next time the loop should start
	auto nextLoopTime = startTime + milliseconds(gyroSampleRate);

	while (isProgramFinished == false) {

		// Data Acquisition
		currentGyroSample = gyro.getYawRate(sampleNumber);

		unique_lock<mutex> gyroSensorReadingsQueueMutexLock(gyroSensorReadings_queueMutex);
		gyroSensorReadings_queue.push(currentGyroSample);
		gyroSensorReadingsQueueMutexLock.unlock();

		sampleNumber++;

		if (sampleNumber >= maxSampleNumber) {
			isProgramFinished = true;
		}

		// Sleep until the next loop period
		this_thread::sleep_until(nextLoopTime);

		// Calculate the next loop time
		nextLoopTime += milliseconds(gyroSampleRate);
	}
}

enum class State {
	GPS_ON, GPS_OFF
};

int main() {

	// iomanip
	cout << fixed << setprecision(6);
	cout << "TrueBearing,EstimatedBearing,latitude,Longitude,Status" << endl;

	double averageSpeed;
	double averageYawRate;

	//Estimated Variables
	double currentEstimatedDistance = 0.0;
	double currentEstimatedBearingAngle = 0.0;
	GPSLocation currentEstimatedGPSLocation = { 0.0, 0.0 };
	GPSReading currentADRReading;
	double previousEstimatedBearingAngle = 0.0;
	double deltaEstimatedBearingAngle;

	//True Variables
	GPSReading currentGPSReading;
	GPSStatus currenGPSStatus = GPSStatus::AVAILABLE;
	GPSLocation currentTrueGPSSample = { 0, 0 };
	GPSLocation previousTrueGPSSample = { 0, 0 };
	bool newGPSReadingFlag = false;
	double currentTrueBearingAngle = 0.0;
	double previousTrueBearingAngle = 0.0;
	double deltaTrueBearingAngle;

	State state = State::GPS_ON;

	thread t1(GPSSensor_thread);
	thread t2(SpeedometerSensor_thread);
	thread t3(GyroSensor_thread);

	//Wait until all threads initialize and block
	this_thread::sleep_for(seconds(1));

	unique_lock<mutex> isThreadStarted_mutexLock(isThreadStarted_mutex);
	isThreadStarted = true;
	cv.notify_all();
	isThreadStarted_mutexLock.unlock();

	while (isProgramFinished == false) {

		unique_lock<mutex> gpsSensorReadingsQueue_MutexLock(gpsSensorReadings_queueMutex);
		if (gpsSensorReadings_queue.empty() == false) {
			currentGPSReading = gpsSensorReadings_queue.front();
			currentTrueGPSSample = currentGPSReading.Location;
			currenGPSStatus = currentGPSReading.Status;
			gpsSensorReadings_queue.pop();
			newGPSReadingFlag = true;

			if (currenGPSStatus == GPSStatus::AVAILABLE) {
				state = State::GPS_ON;
			} else {
				state = State::GPS_OFF;
			}

			currentADRReading.Status = currenGPSStatus;

		}
		gpsSensorReadingsQueue_MutexLock.unlock();

		if (newGPSReadingFlag == true) {
			newGPSReadingFlag = false;

			unique_lock<mutex> speedometerSensorReadingsQueueMutexLock(speedometerSensorReadings_queueMutex);
			if (speedometerSensorReadings_queue.empty() == false) {
				averageSpeed = sumQueue(speedometerSensorReadings_queue);
			}
			speedometerSensorReadingsQueueMutexLock.unlock();

			unique_lock<mutex> gyroSensorReadingsQueueMutexLock(gyroSensorReadings_queueMutex);
			if (gyroSensorReadings_queue.empty() == false) {
				averageYawRate = calculateQueueDifference(gyroSensorReadings_queue);
			}
			gyroSensorReadingsQueueMutexLock.unlock();

			//Always Calculate Bearing -- FOr Demo Perposes only
			currentTrueBearingAngle = calculateBearingAngle(previousTrueGPSSample, currentTrueGPSSample);
			previousTrueGPSSample = currentTrueGPSSample;

			deltaTrueBearingAngle = currentTrueBearingAngle - previousTrueBearingAngle;
			previousTrueBearingAngle = currentTrueBearingAngle;

			switch (state) {
				case (State::GPS_ON): {

					currentEstimatedBearingAngle = currentTrueBearingAngle;
					currentEstimatedGPSLocation = currentTrueGPSSample;

					currentADRReading.Location = currentTrueGPSSample;

					break;
				}
				case (State::GPS_OFF): {

					currentEstimatedDistance = KMH_TO_MS(averageSpeed) * MILLISECONDS_TO_SECONDS((double )40);
					currentEstimatedBearingAngle += averageYawRate;
					currentEstimatedGPSLocation = calculateDeadReckoningGPS(currentEstimatedGPSLocation, currentEstimatedDistance,
							currentEstimatedBearingAngle);

					currentADRReading.Location = currentEstimatedGPSLocation;

					break;
				}
			}

			deltaEstimatedBearingAngle = currentEstimatedBearingAngle - previousEstimatedBearingAngle;
			previousEstimatedBearingAngle = currentEstimatedBearingAngle;

			cout << currentADRReading << endl;

		}
	}

	t1.join();
	t2.join();
	t3.join();

	return 0;
}

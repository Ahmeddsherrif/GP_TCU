/*
 * ADR.h
 *
 *  Created on: Jun 28, 2023
 *      Author: ahmed
 */

#ifndef ADR_H_
#define ADR_H_

class ADR {
public:
	static string getADR_GPS(queue<double> &speedQueue, mutex &speedMutex, queue<double> &gyroQueue, mutex &gyroMutex,
			queue<GPSLocation> &gpsQueue, mutex &gpsMutex, outageStatus &currentOutageStatus,
			mutex &mutexCurrentOutageStatus);
private:
	static double averageSpeed;
	static double averageYawRate;

	//Estimated Variables
	static double currentEstimatedDistance;
	static double currentEstimatedBearingAngle;
	static GPSLocation currentEstimatedGPSLocation;

	//True Variables
	static double currentTrueBearingAngle;
	static GPSLocation currentTrueGPSSample;
	static GPSLocation previousTrueGPSSample;


	static int secondsSinceLastOutage;

	static GPSLocation calculateDeadReckoningGPS(GPSLocation initialGPS, double distance, double bearing);
	static double calculateBearingAngle(GPSLocation fromGPS, GPSLocation toGPS);
	static double calculateDegreeDifference(double start_angle, double end_angle);
	static double calculateQueueDifference(queue<double> &angle_queue);
	static double sumQueue(queue<double> &q);
};

#endif /* ADR_H_ */

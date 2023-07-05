/*
 * ADR.h
 *
 *  Created on: Jun 28, 2023
 *      Author: ahmed
 */

#ifndef ADR_H_
#define ADR_H_

struct GPSLocation {
	double latitude; // in degrees
	double longitude; // in degree

};

enum outageStatus {
	OUTAGE, NO_OUTAGE
};


class ADR {
public:
	static string getADR_GPS(queue<double> &speedQueue, queue<double> &gyroQueue, queue<GPSLocation> &gpsQueue,
			const string &currentOutageStatus);

	static void clear();
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

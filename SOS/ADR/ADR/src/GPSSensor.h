/*
 * GPSSensor.h
 *
 *  Created on: Feb 27, 2023
 *      Author: ahmed
 */

#ifndef GPSSENSOR_H_
#define GPSSENSOR_H_

struct GPSLocation {
		double latitude; // in degrees
		double longitude; // in degrees
		//double horizontalAccuracy;
};

enum class GPSStatus{
		UNAVALIABLE,
		AVAILABLE
};

struct GPSReading{
		GPSLocation Location;
		GPSStatus Status;
};

ostream& operator<<(ostream& os, const GPSReading& gps);

class GPSSensor : public SensorFile {
	public:
		GPSSensor(const string &fileName) ;
		GPSLocation getGPS(const int& sampleNumber);
};

#endif /* GPSSENSOR_H_ */

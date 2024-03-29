/*
 * GPSSensor.cpp
 *
 *  Created on: Feb 27, 2023
 *      Author: ahmed
 */

#include <string>
#include <vector>
#include <iostream>

using namespace std;

#include "SensorFile.h"
#include "GPSSensor.h"

#define LATITUDE_COLUMN		1
#define LONGITUDE_COLUMN	2

GPSSensor::GPSSensor(const string &fileName) :
		SensorFile(fileName) {
	
}

GPSLocation GPSSensor::getGPS(const int &sampleNumber) {
	vector<string> data = getSensorData(sampleNumber);
	GPSLocation tempGPS = { stod(data[LATITUDE_COLUMN]), stod(data[LONGITUDE_COLUMN]) };
	return tempGPS;
}


string getGPSString(const GPSReading &gps) {

    string rtnGPSReading;
	switch (gps.Status) {
		case (GPSStatus::AVAILABLE): {
			rtnGPSReading  = "AVAILABLE";
			break;
		}

		case (GPSStatus::UNAVALIABLE): {
			rtnGPSReading = "OUTAGE";
			break;
		}
	}
	rtnGPSReading += ",";


	rtnGPSReading += to_string(gps.Location.latitude) + "," + to_string(gps.Location.longitude);

	return rtnGPSReading;
}


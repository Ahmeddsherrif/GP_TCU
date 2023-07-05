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

string GPSSensor::getGPS(const int &sampleNumber) {
	vector<string> data = getSensorData(sampleNumber);
	string tempGPS = data[LATITUDE_COLUMN] + ',' + data[LONGITUDE_COLUMN];
	return tempGPS;
}

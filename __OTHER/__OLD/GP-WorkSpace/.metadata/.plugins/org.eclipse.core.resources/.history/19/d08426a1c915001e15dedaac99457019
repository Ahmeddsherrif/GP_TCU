/*
 * GyroscopeSensor.cpp
 *
 *  Created on: Feb 27, 2023
 *      Author: ahmed
 */

#include <string>
#include <vector>

using namespace std;

#include "SensorFile.h"
#include "GyroscopeSensor.h"

#define YAW_RATE_COLUMN		1

GyroscopeSensor::GyroscopeSensor(const string &fileName) :
		SensorFile(fileName) {
	
}

double GyroscopeSensor::getYawRate(const int &sampleNumber) {
	vector<string> data = getSensorData(sampleNumber);
	double tempYawRate = stod(data[YAW_RATE_COLUMN]);
	return tempYawRate;
}



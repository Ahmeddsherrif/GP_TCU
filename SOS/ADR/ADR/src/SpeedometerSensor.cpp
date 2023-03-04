/*
 * SpeedometerSensor.cpp
 *
 *  Created on: Feb 27, 2023
 *      Author: ahmed
 */

#include <string>
#include <vector>

using namespace std;

#include "SensorFile.h"

#include "SpeedometerSensor.h"

SpeedometerSensor::SpeedometerSensor(const string &fileName) :
		SensorFile(fileName) {
	
}

double SpeedometerSensor::getSpeed(const int &sampleNumber) {
	vector<string> data = getSensorData(sampleNumber);
	double tempSpeed = stod(data[1]);
	return tempSpeed;
}


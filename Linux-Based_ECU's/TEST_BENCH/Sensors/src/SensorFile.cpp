/*
 * SensorFile.cpp
 *
 *  Created on: Feb 27, 2023
 *      Author: ahmed
 */

#include <string>
#include <fstream>
#include <sstream>
#include <vector>

using namespace std;

#include "SensorFile.h"

SensorFile::SensorFile(const string &fileName) :
		numberOfSamples { 0 } {

	ifstream sensorFile(fileName);
	string line;
	
	if (sensorFile.is_open()) {

		while (getline(sensorFile, line)) {
			sensorData.push_back(line);
			numberOfSamples++;
		}

		sensorFile.close();

	}
}

vector<string> SensorFile::getSensorData(const int &sampleNumber) {

	vector<string> data;
	stringstream ss(sensorData[sampleNumber]);
	string item;

	while (getline(ss, item, ',')) {
		data.push_back(item);
	}

	return data;
}

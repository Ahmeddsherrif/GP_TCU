#include <iostream>
#include <stdint.h>
#include <cstring>
#include <string>
#include <queue>
#include <vector>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <fstream>

using namespace std;

#include "BitStreamEncoder.h"


#include "MSDData.h"
#include "MSDEncoder.h"


constexpr int MSDEncoder::structElementsNumber;
constexpr int MSDEncoder::parseSequence[structElementsNumber];

constexpr int MSDEncoder::printWidth;
constexpr char MSDEncoder::printFillChar;
constexpr int MSDEncoder::printFillWidth;



MSDEncoder::MSDEncoder(const string &fileDir) {
	memset(&ECallMessage, 0, sizeof(ECallMessage));

	ifstream inputConfigFile;

	string msdConfigFilePath = fileDir + "/Config.txt";
	inputConfigFile.open(msdConfigFilePath);

	if (inputConfigFile.is_open()) {

		string currentLine;

		getline(inputConfigFile, currentLine);
		this->ECallMessage.id = stoi(currentLine);

		getline(inputConfigFile, currentLine);
		this->ECallMessage.msd.msdStructure.vehiclenumber = stoi(currentLine);

		getline(inputConfigFile, currentLine);
		this->ECallMessage.msd.msdStructure.control.vehicleType = static_cast<tVehicleType>(stoi(currentLine));

		getline(inputConfigFile, currentLine);
		this->ECallMessage.msd.msdStructure.vehiclePropulsionStorageType = static_cast<tVehiclePropulsionStorageType>(stoi(currentLine));

		inputConfigFile.close();
	}

	string msdPhoneFilePath = fileDir + "/Phone.txt";
	inputConfigFile.open(msdPhoneFilePath);

	if(inputConfigFile.is_open()){
		string currentLine;

		getline(inputConfigFile, currentLine);
		this->emergencyPhoneNumber = currentLine.substr(0,11);

	    inputConfigFile.close();
	}

}

void MSDEncoder::setActivation(const bool &activation) {
	this->ECallMessage.msd.msdStructure.control.automaticActivation = activation;
}

void MSDEncoder::setThreatDetected(const bool &threatDetected) {
	this->ECallMessage.msd.msdStructure.control.threatDetected = threatDetected;
}

void MSDEncoder::setPositionTrust(const bool &positionTrust) {
	this->ECallMessage.msd.msdStructure.control.positionCanBeTrusted = positionTrust;
}

void MSDEncoder::setPositionLatitude(const float &latitude) {
	this->ECallMessage.msd.msdStructure.vehicleLocation.positionLatitude = latitude;
}

void MSDEncoder::setPositionLongitude(const float &longitude) {
	this->ECallMessage.msd.msdStructure.vehicleLocation.positionLongitude = longitude;
}

void MSDEncoder::setVehicleDirection(const uint32_t &vehicleDirection) {
	this->ECallMessage.msd.msdStructure.vehicleDirection = vehicleDirection;
}

void MSDEncoder::setNumberOfPassengers(const uint32_t &numberOfPassengers) {
	this->ECallMessage.msd.msdStructure.numberOfPassengers = numberOfPassengers;
}

string MSDEncoder::getEmergencyPhoneNumber() {
	return this->emergencyPhoneNumber;
}

string MSDEncoder::encode() {

	int i = 0;
	encoder.appendBits(ECallMessage.id, parseSequence[i]);
	i++;

	encoder.appendBits(ECallMessage.msd.msdStructure.vehiclenumber, parseSequence[i]);
	i++;

	encoder.appendBits(ECallMessage.msd.msdStructure.control.automaticActivation, parseSequence[i]);
	i++;

	encoder.appendBits(ECallMessage.msd.msdStructure.control.threatDetected, parseSequence[i]);
	i++;

	encoder.appendBits(ECallMessage.msd.msdStructure.control.positionCanBeTrusted, parseSequence[i]);
	i++;

	encoder.appendBits(static_cast<uint32_t>(ECallMessage.msd.msdStructure.control.vehicleType), parseSequence[i]);
	i++;

	encoder.appendBits(static_cast<uint32_t>(ECallMessage.msd.msdStructure.vehiclePropulsionStorageType), parseSequence[i]);
	i++;

//	encoder.appendBits(ECallMessage.msd.msdStructure.timestamp, parseSequence[i]);
//	i++;

	encoder.appendBits(reinterpret_cast<uint32_t&>(ECallMessage.msd.msdStructure.vehicleLocation.positionLatitude), parseSequence[i]);
	i++;

	encoder.appendBits(reinterpret_cast<uint32_t&>(ECallMessage.msd.msdStructure.vehicleLocation.positionLongitude), parseSequence[i]);
	i++;

	encoder.appendBits(ECallMessage.msd.msdStructure.vehicleDirection, parseSequence[i]);
	i++;

	encoder.appendBits(ECallMessage.msd.msdStructure.numberOfPassengers, parseSequence[i]);
	i++;

	string msd = encoder.get_hex_string();

	return msd;
}

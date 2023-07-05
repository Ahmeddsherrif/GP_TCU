/*
 * MSDDecoder.cpp
 *
 *  Created on: Mar 10, 2023
 *      Author: ahmed
 */

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

#include "MSDData.h"
#include "BitStreamDecoder.h"
#include "MSDDecoder.h"

constexpr int MSDDecoder::structElementsNumber;
constexpr int MSDDecoder::parseSequence[structElementsNumber];

constexpr int MSDDecoder::printWidth;
constexpr char MSDDecoder::printFillChar;
constexpr int MSDDecoder::printFillWidth;

MSDDecoder::MSDDecoder() {
	memset(&ECallMessage, 0, sizeof(ECallMessage));
}


void MSDDecoder::decode(const string &EncodedMSDString) {

	decoder.parse(EncodedMSDString, parseSequence, structElementsNumber);

	queue<uint32_t> structElementQueue = decoder.getStructQueue();

	ECallMessage.id = structElementQueue.front(); //queue and static variable
	structElementQueue.pop();

	ECallMessage.msd.msdStructure.vehiclenumber = structElementQueue.front();
	structElementQueue.pop();

	ECallMessage.msd.msdStructure.control.automaticActivation = static_cast<bool>(structElementQueue.front());
	structElementQueue.pop();

	ECallMessage.msd.msdStructure.control.threatDetected = static_cast<bool>(structElementQueue.front());
	structElementQueue.pop();

	ECallMessage.msd.msdStructure.control.positionCanBeTrusted = static_cast<bool>(structElementQueue.front());
	structElementQueue.pop();

	ECallMessage.msd.msdStructure.control.vehicleType = static_cast<tVehicleType>(structElementQueue.front());
	structElementQueue.pop();

	ECallMessage.msd.msdStructure.vehiclePropulsionStorageType = static_cast<tVehiclePropulsionStorageType>(structElementQueue.front());
	structElementQueue.pop();

//	ECallMessage.msd.msdStructure.timestamp = structElementQueue.front();
//	structElementQueue.pop();

	ECallMessage.msd.msdStructure.vehicleLocation.positionLatitude = reinterpret_cast<float&>(structElementQueue.front());
	structElementQueue.pop();

	ECallMessage.msd.msdStructure.vehicleLocation.positionLongitude = reinterpret_cast<float&>(structElementQueue.front());
	structElementQueue.pop();

	ECallMessage.msd.msdStructure.vehicleDirection = structElementQueue.front();
	structElementQueue.pop();

	ECallMessage.msd.msdStructure.numberOfPassengers = structElementQueue.front();
	structElementQueue.pop();

	parseDecodedMSDToStrings();
}

void MSDDecoder::print() {
	cout << setfill(printFillChar) << setw(60) << "" << endl;
	cout << left << setfill('.');

	cout << setw(printWidth) << "> MSD ID: " << " " << id << endl;
	cout << setw(printWidth) << "> Vehicle Number: " << " " << vehicleNumber << endl;
	cout << setw(printWidth) << "> Activation: " << " " << activation << endl;
	cout << setw(printWidth) << "> Threat Detected: " << " " << threatDetected << endl;
	cout << setw(printWidth) << "> Position Trusted: " << " " << positionTrusted << endl;
	cout << setw(printWidth) << "> Vehicle Type: " << " " << vehicleType << endl;
	cout << setw(printWidth) << "> Propulsion Type: " << " " << propulsionType << endl;
	cout << setw(printWidth) << "> Timestamp: " << " " << timestamp << endl;
	cout << setw(printWidth) << "> Position Latitude: " << " " << latitude << endl;
	cout << setw(printWidth) << "> Position Longitude: " << " " << longitude << endl;
	cout << setw(printWidth) << "> Vehicle Direction: " << " " << vehicleDirection << endl;
	cout << setw(printWidth) << "> Number Of Passengers: " << " " << numberOfPassengers << endl;

	cout << setfill(printFillChar) << setw(60) << "" << endl;
}

void MSDDecoder::log(const string &fileDir) {
	string fileName = "ID-" + this->id + "_" + this->timestamp;
	string filePath = fileDir + "/" + fileName + ".txt";

	ofstream outputFile;
	outputFile.open(filePath, ios::app);

	if (outputFile.is_open()) {

		outputFile << this->id << endl;
		outputFile << this->vehicleNumber << endl;
		outputFile << this->activation << endl;
		outputFile << this->threatDetected << endl;
		outputFile << this->positionTrusted << endl;
		outputFile << this->vehicleType << endl;
		outputFile << this->propulsionType << endl;
		outputFile << this->timestamp << endl;
		outputFile << this->latitude << endl;
		outputFile << this->longitude << endl;
		outputFile << this->vehicleDirection << endl;
		outputFile << this->numberOfPassengers << endl;

		outputFile.close();
	}

}

string MSDDecoder::getTimestamp() {
	// Get the current time point
	auto now = chrono::system_clock::now();

	// Convert the time point to a time_t object
	auto currentTime = chrono::system_clock::to_time_t(now);

	// Extract the microseconds from the time point
	auto microseconds = chrono::duration_cast<chrono::microseconds>(now.time_since_epoch()) % 1000000;

	// Convert the time_t object to a local time struct
	tm *localTime = localtime(&currentTime);

	// Format the timestamp string
	char timestamp[100];
	strftime(timestamp, sizeof(timestamp), "%Y-%m-%d_%H-%M-%S", localTime);

	string rtnStr = string(timestamp);

	return rtnStr;
}

void MSDDecoder::parseDecodedMSDToStrings() {
	this->id = to_string(ECallMessage.id);
	this->vehicleNumber = to_string(ECallMessage.msd.msdStructure.vehiclenumber);

	if (ECallMessage.msd.msdStructure.control.automaticActivation == true) {
		this->activation = "AUTOMATIC";
	} else {
		this->activation = "MANUAL";
	}

	if (ECallMessage.msd.msdStructure.control.threatDetected == true) {
		this->threatDetected = "TRUE";
	} else {
		this->threatDetected = "FALSE";
	}

	if (ECallMessage.msd.msdStructure.control.positionCanBeTrusted == true) {
		this->positionTrusted = "ACCURATE POSITION";
	} else {
		this->positionTrusted = "ESTIMATED POSITION";
	}

	switch (ECallMessage.msd.msdStructure.control.vehicleType) {
		case (tVehicleType::BUSES_AND_COACHES_CLASS_M2): {
			this->vehicleType = "BUSES AND COACHES CLASS M2";
			break;
		}
		case (tVehicleType::BUSES_AND_COACHES_CLASS_M3): {
			this->vehicleType = "BUSES AND COACHES CLASS M3";
			break;
		}
		case (tVehicleType::HEAVY_DUTY_VEHICLES_CLASS_N2): {
			this->vehicleType = "HEAVY DUTY VEHICLES CLASS N2";
			break;
		}
		case (tVehicleType::HEAVY_DUTY_VEHICLES_CLASS_N3): {
			this->vehicleType = "HEAVY DUTY VEHICLES CLASS N3";
			break;
		}
		case (tVehicleType::LIGHT_COMMERCIAL_VEHICLES_CLASS_N1): {
			this->vehicleType = "LIGHT COMMERCIAL VEHICLES CLASS N1";
			break;
		}
		case (tVehicleType::MOTORCYCLE_CLASS_L1E): {
			this->vehicleType = "MOTORCYCLE CLASS L1E";
			break;
		}
		case (tVehicleType::MOTORCYCLE_CLASS_L2E): {
			this->vehicleType = "MOTORCYCLE CLASS L2E";
			break;
		}
		case (tVehicleType::MOTORCYCLE_CLASS_L3E): {
			this->vehicleType = "MOTORCYCLE CLASS L3E";
			break;
		}
		case (tVehicleType::PASSENGER_VEHICLE_CLASS_M1): {
			this->vehicleType = "PASSENGER VEHICLE CLASS M1";
			break;
		}

	}

	switch (ECallMessage.msd.msdStructure.vehiclePropulsionStorageType) {
		case (tVehiclePropulsionStorageType::DIESEL_TANK): {
			this->propulsionType = "DIESEL TANK";
			break;
		}
		case (tVehiclePropulsionStorageType::ELECTRIC_ENERGY): {
			this->propulsionType = "ELECTRIC ENERGY";
			break;
		}
		case (tVehiclePropulsionStorageType::GASOLINE_TANK): {
			this->propulsionType = "GASOLINE TANK";
			break;
		}
		case (tVehiclePropulsionStorageType::NATURAL_GAS): {
			this->propulsionType = "NATURAL GAS";
			break;
		}
	}

	this->timestamp = getTimestamp(); //to_string(ECallMessage.msd.msdStructure.timestamp);
	this->latitude = to_string(ECallMessage.msd.msdStructure.vehicleLocation.positionLatitude);
	this->longitude = to_string(ECallMessage.msd.msdStructure.vehicleLocation.positionLongitude);
	this->vehicleDirection = to_string(ECallMessage.msd.msdStructure.vehicleDirection);
	this->numberOfPassengers = to_string(ECallMessage.msd.msdStructure.numberOfPassengers);
}

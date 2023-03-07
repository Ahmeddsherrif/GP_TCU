#include <iostream>
#include <stdint.h>
#include <cstring>
#include <string>
#include <queue>
#include <vector>
#include <iomanip>

using namespace std;

#include "BitStreamEncoder.h"
#include "BitStreamDecoder.h"
#include "MSD.h"

constexpr uint8_t MSD::structElementsNumber;
constexpr uint8_t MSD::parseSequence[structElementsNumber];
constexpr uint8_t MSD::printWidth;
constexpr char MSD::printFillChar;

MSD::MSD() {
	memset(&ECallMessage, 0, sizeof(ECallMessage));
}

void MSD::config() {
	ECallMessage.id = 29;
	ECallMessage.msd.msdStructure.vehiclenumber = 1;
	ECallMessage.msd.msdStructure.control.automaticActivation = true;
	ECallMessage.msd.msdStructure.control.testCall = true;
	ECallMessage.msd.msdStructure.control.positionCanBeTrusted = true;
	ECallMessage.msd.msdStructure.control.vehicleType = tVehicleType::PASSENGER_VEHICLE_CLASS_M1;
	ECallMessage.msd.msdStructure.vehiclePropulsionStorageType = tVehiclePropulsionStorageType::GASOLINE_TANK;
	ECallMessage.msd.msdStructure.timestamp = 4;
	ECallMessage.msd.msdStructure.vehicleLocation.positionLatitude = 164546;
	ECallMessage.msd.msdStructure.vehicleLocation.positionLongitude = 124672;
	ECallMessage.msd.msdStructure.vehicleDirection = 112455;
	ECallMessage.msd.msdStructure.numberOfPassengers = 3;
}

string MSD::encode() {


	int i=0;
	encoder.appendBits(ECallMessage.id, parseSequence[i]);
	i++;

	encoder.appendBits(ECallMessage.msd.msdStructure.vehiclenumber, parseSequence[i]);
	i++;

	encoder.appendBits(ECallMessage.msd.msdStructure.control.automaticActivation, parseSequence[i]);
	i++;

	encoder.appendBits(ECallMessage.msd.msdStructure.control.testCall, parseSequence[i]);
	i++;

	encoder.appendBits(ECallMessage.msd.msdStructure.control.positionCanBeTrusted, parseSequence[i]);
	i++;

	encoder.appendBits(static_cast<uint32_t>(ECallMessage.msd.msdStructure.control.vehicleType), parseSequence[i]);
	i++;

	encoder.appendBits(static_cast<uint32_t>(ECallMessage.msd.msdStructure.vehiclePropulsionStorageType), parseSequence[i]);
	i++;

	encoder.appendBits(ECallMessage.msd.msdStructure.timestamp, parseSequence[i]);
	i++;

	encoder.appendBits(ECallMessage.msd.msdStructure.vehicleLocation.positionLatitude, parseSequence[i]);
	i++;

	encoder.appendBits(ECallMessage.msd.msdStructure.vehicleLocation.positionLongitude, parseSequence[i]);
	i++;

	encoder.appendBits(ECallMessage.msd.msdStructure.vehicleDirection, parseSequence[i]);
	i++;

	encoder.appendBits(ECallMessage.msd.msdStructure.numberOfPassengers, parseSequence[i]);
	i++;

	string msd = encoder.get_hex_string();

	return msd;
}


void MSD::decode(const string &EncodedMSDString) {

	decoder.parse(EncodedMSDString, parseSequence, structElementsNumber);

	queue<uint32_t> structElementQueue = decoder.getStructQueue();

	ECallMessage.id = structElementQueue.front(); //queue and static variable
	structElementQueue.pop();

	ECallMessage.msd.msdStructure.vehiclenumber = structElementQueue.front();
	structElementQueue.pop();

	ECallMessage.msd.msdStructure.control.automaticActivation = static_cast<bool>(structElementQueue.front());
	structElementQueue.pop();

	ECallMessage.msd.msdStructure.control.testCall = static_cast<bool>(structElementQueue.front());
	structElementQueue.pop();

	ECallMessage.msd.msdStructure.control.positionCanBeTrusted = static_cast<bool>(structElementQueue.front());
	structElementQueue.pop();

	ECallMessage.msd.msdStructure.control.vehicleType = static_cast<tVehicleType>(structElementQueue.front());
	structElementQueue.pop();

	ECallMessage.msd.msdStructure.vehiclePropulsionStorageType = static_cast<tVehiclePropulsionStorageType>(structElementQueue.front());
	structElementQueue.pop();

	ECallMessage.msd.msdStructure.timestamp = structElementQueue.front();
	structElementQueue.pop();

	ECallMessage.msd.msdStructure.vehicleLocation.positionLatitude = static_cast<int32_t>(structElementQueue.front());
	structElementQueue.pop();

	ECallMessage.msd.msdStructure.vehicleLocation.positionLongitude = static_cast<int32_t>(structElementQueue.front());
	structElementQueue.pop();

	ECallMessage.msd.msdStructure.vehicleDirection = structElementQueue.front();
	structElementQueue.pop();

	ECallMessage.msd.msdStructure.numberOfPassengers = structElementQueue.front();
	structElementQueue.pop();
}

void MSD::print() {

	string id = to_string(ECallMessage.id );
	string vehicleNumber = to_string(ECallMessage.msd.msdStructure.vehiclenumber);

	string activation;
	if(ECallMessage.msd.msdStructure.control.automaticActivation == true){
		activation = "AUTOMATIC";
	}else{
		activation = "MANUAL";
	}

	string testCall;
	if(ECallMessage.msd.msdStructure.control.testCall == true){
		testCall = "TRUE";
	}else{
		testCall = "FALSE";
	}

	string positionTrusted;
	if(ECallMessage.msd.msdStructure.control.positionCanBeTrusted == true){
		positionTrusted = "ACCURATE POSITION";
	}else{
		positionTrusted = "ESTIMATED POSITION";
	}

	string vehicleType;
	switch (ECallMessage.msd.msdStructure.control.vehicleType) {
		case (tVehicleType::BUSES_AND_COACHES_CLASS_M2): {
			vehicleType = "BUSES AND COACHES CLASS M2";
			break;
		}
		case (tVehicleType::BUSES_AND_COACHES_CLASS_M3): {
			vehicleType = "BUSES AND COACHES CLASS M3";
			break;
		}
		case (tVehicleType::HEAVY_DUTY_VEHICLES_CLASS_N2): {
			vehicleType = "HEAVY DUTY VEHICLES CLASS N2";
			break;
		}
		case (tVehicleType::HEAVY_DUTY_VEHICLES_CLASS_N3): {
			vehicleType = "HEAVY DUTY VEHICLES CLASS N3";
			break;
		}
		case (tVehicleType::LIGHT_COMMERCIAL_VEHICLES_CLASS_N1): {
			vehicleType = "LIGHT COMMERCIAL VEHICLES CLASS N1";
			break;
		}
		case (tVehicleType::MOTORCYCLE_CLASS_L1E): {
			vehicleType = "MOTORCYCLE CLASS L1E";
			break;
		}
		case (tVehicleType::MOTORCYCLE_CLASS_L2E): {
			vehicleType = "MOTORCYCLE CLASS L2E";
			break;
		}
		case (tVehicleType::MOTORCYCLE_CLASS_L3E): {
			vehicleType = "MOTORCYCLE CLASS L3E";
			break;
		}
		case (tVehicleType::PASSENGER_VEHICLE_CLASS_M1): {
			vehicleType = "PASSENGER VEHICLE CLASS M1";
			break;
		}

	}

	string propulsionType;
	switch (ECallMessage.msd.msdStructure.vehiclePropulsionStorageType) {
		case (tVehiclePropulsionStorageType::DIESEL_TANK): {
			propulsionType = "DIESEL TANK";
			break;
		}
		case (tVehiclePropulsionStorageType::ELECTRIC_ENERGY): {
			propulsionType = "ELECTRIC ENERGY";
			break;
		}
		case (tVehiclePropulsionStorageType::GASOLINE_TANK): {
			propulsionType = "GASOLINE TANK";
			break;
		}
		case (tVehiclePropulsionStorageType::NATURAL_GAS): {
			propulsionType = "NATURAL GAS";
			break;
		}
	}

	string timeStamp = to_string(ECallMessage.msd.msdStructure.timestamp);
	string latitude = to_string(ECallMessage.msd.msdStructure.vehicleLocation.positionLatitude);
	string longitude = to_string(ECallMessage.msd.msdStructure.vehicleLocation.positionLongitude);
	string vehicleDirection = to_string(ECallMessage.msd.msdStructure.vehicleDirection);
	string numberOfPassengers = to_string(ECallMessage.msd.msdStructure.numberOfPassengers);

	cout << setfill('*') << setw(60) << "" << endl;
	cout << uppercase << boolalpha << left << setfill('.');

	cout << setw(printWidth) << "> MSD ID: " << id << endl;
	cout << setw(printWidth) << "> Vehicle Number: " << vehicleNumber << endl;
	cout << setw(printWidth) << "> Activation: " << activation << endl;
	cout << setw(printWidth) << "> Test Call: " << testCall << endl;
	cout << setw(printWidth) << "> Position Trusted: " << positionTrusted << endl;
	cout << setw(printWidth) << "> Vehicle Type: " << vehicleType << endl;
	cout << setw(printWidth) << "> Propulsion Type: " << propulsionType << endl;
	cout << setw(printWidth) << "> Timestamp: " << timeStamp << endl;
	cout << setw(printWidth) << "> Position Latitude: " << latitude << endl;
	cout << setw(printWidth) << "> Position Longitude: " << longitude << endl;
	cout << setw(printWidth) << "> Vehicle Direction: " << vehicleDirection << endl;
	cout << setw(printWidth) << "> Number Of Passengers: " << numberOfPassengers << endl;
	cout << setfill('*') << setw(60) << "" << endl;
}

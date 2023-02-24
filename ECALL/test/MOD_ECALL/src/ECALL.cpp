/*
 * ECALL.cpp
 *
 *  Created on: Feb 18, 2023
 *      Author: ahmed
 */

#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>

using namespace std;

#include "error.h"
#include "configuration.h"
#include "debug.h"
#include "ECALL.h"


void ECALL::printMSD() {

	cout << "\n\n" << setfill('*') << setw(50) << "" << endl;
	cout.fill(' ');

	cout << left << setw(25) << "Trigger Type: " << myMSD.triggerType << endl;
	cout << left << setw(25) << "Vehicle Color: " << myMSD.VehicleColor << endl;
	cout << left << setw(25) << "Vehicle Type: " << myMSD.vehicleType << endl;
	cout << left << setw(25) << "Number of Passengers: " << myMSD.numberOfPassengers << endl;

	cout << setfill('*') << setw(50) << "" << endl;
	cout.fill(' ');
}

void ECALL::init() {
	ifstream MsdFile(MSD_IN_FILE_NAME);

	if (!MsdFile.is_open()) {
		// handle error
		return;
	}

	if (getline(MsdFile, stringMSD)) {
		stringstream ss(stringMSD);
		string field;
		getline(ss, field, ',');
		myMSD.triggerType = field;
		getline(ss, field, ',');
		myMSD.VehicleColor = field;
		getline(ss, field, ',');
		myMSD.vehicleType = field;
		getline(ss, field, ',');
		myMSD.numberOfPassengers = field;
	}

	if (MsdFile.is_open()) {
		MsdFile.close();
	}

	ifstream ConfigFile(CONFIG_FILE_NAME);

	if (!ConfigFile.is_open()) {
		// handle error
		return;
	}

	string str;

	if (getline(ConfigFile, str)) {
		stringstream ss(str);
		string field;
		getline(ss, field, ',');
		myConfig.emergencyPhoneNumber = field;
	}

	if (ConfigFile.is_open()) {
		ConfigFile.close();
	}

}

string ECALL::getMSD(){
	return stringMSD;
}

string ECALL::getEmergencyPhoneNumber(){
	return myConfig.emergencyPhoneNumber;
}


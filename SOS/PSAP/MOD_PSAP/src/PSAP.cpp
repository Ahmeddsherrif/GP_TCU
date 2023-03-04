/*
 * PSAP.cpp
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
#include "PSAP.h"

void PSAP::extractMSDfromString(string &inputStr){

    istringstream ss(inputStr);
    string field;

    // Parse each field from the input string and store it in the corresponding struct field

	if (getline(ss, field, ',')) {
		myMSD.triggerType = field;
	}

	if (getline(ss, field, ',')) {
		myMSD.VehicleColor = field;
	}

	if (getline(ss, field, ',')) {
		myMSD.vehicleType = field;
	}

	if (getline(ss, field, '\n')) {
		myMSD.numberOfPassengers = field;
	}

}

void PSAP::printMSD() {

	cout << "\n\n" << setfill('*') << setw(50) << "" << endl;
	cout.fill(' ');

	cout << left << setw(25) << "Trigger Type: " << myMSD.triggerType << endl;
	cout << left << setw(25) << "Vehicle Color: " << myMSD.VehicleColor << endl;
	cout << left << setw(25) << "Vehicle Type: " << myMSD.vehicleType << endl;
	cout << left << setw(25) << "Number of Passengers: " << myMSD.numberOfPassengers << endl;

	cout << setfill('*') << setw(50) << "" << endl;
	cout.fill(' ');
}

void PSAP::logMSD() {
	ofstream outFile;
	outFile.open(MSD_OUT_FILE_NAME, ios_base::app);

	// Write each field of the MSD struct to the file
	outFile << myMSD.VehicleColor << ",";
	outFile << myMSD.vehicleType << ",";
	outFile << myMSD.numberOfPassengers << "\n";

	outFile.close();
}


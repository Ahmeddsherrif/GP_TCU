/*
 * ECALL.h
 *
 *  Created on: Feb 18, 2023
 *      Author: ahmed
 */

#ifndef ECALL_H_
#define ECALL_H_

struct MSD {
		string triggerType;
		string VehicleColor;
		string vehicleType;
		string numberOfPassengers;

		// Constructor that initializes all fields to empty strings
		MSD() :
				triggerType(""), VehicleColor(""), vehicleType(""), numberOfPassengers("") {}
};

struct Config {
		string emergencyPhoneNumber;
};

//TOD:store stuff on a file

class ECALL {
	public:
		void printMSD();
		void init();
		string getMSD();
		string getEmergencyPhoneNumber();

	private:
		MSD myMSD;
		Config myConfig;
		string stringMSD;

};

#endif /* ECALL_H_ */

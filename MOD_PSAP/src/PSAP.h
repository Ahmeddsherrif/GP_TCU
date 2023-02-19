/*
 * PSAP.h
 *
 *  Created on: Feb 18, 2023
 *      Author: ahmed
 */

#ifndef PSAP_H_
#define PSAP_H_

struct MSD {
		string triggerType;
		string VehicleColor;
		string vehicleType;
		string numberOfPassengers;

		// Constructor that initializes all fields to empty strings
		MSD() :
				triggerType(""), VehicleColor(""), vehicleType(""), numberOfPassengers("") {}
};

//TOD:store stuff on a file

class PSAP {
	public:
		void extractMSDfromString(string &inputStr);
		void printMSD();
		void logMSD();

	private:
		MSD myMSD;


};

#endif /* PSAP_H_ */

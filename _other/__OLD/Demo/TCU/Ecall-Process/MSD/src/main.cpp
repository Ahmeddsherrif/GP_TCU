#include <iostream>
#include <stdint.h>
#include <queue>
#include <vector>
#include <string>
using namespace std;

#include "BitStreamEncoder.h"
#include "BitStreamDecoder.h"

#include "MSDData.h"
#include "MSDEncoder.h"
#include "MSDDecoder.h"
//#include "MSD.h"



int main() {

	MSDEncoder msd("/home/ahmed/Desktop/ECALL_MSD_Config");

	msd.setActivation(false);
	msd.setTestCall(true);
	msd.setPositionTrust(true);
	msd.setPositionLatitude(27.45654);
	msd.setPositionLongitude(27.45631);
	msd.setVehicleDirection(1234);
	msd.setNumberOfPassengers(4);


	string EncodedMSDString = msd.encode();


	cout << "MSD: " << EncodedMSDString << endl;


	MSDDecoder msdd;
	msdd.decode(EncodedMSDString);
	msdd.log("/home/ahmed/Desktop/PSAP_MSD_OUT");

	msdd.print();

	return 0;
}


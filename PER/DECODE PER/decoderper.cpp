#include "decodeper.h"


int decoding::decode(string msdencoded,char start,char len)
{


	// Initializing base value to 1, i.e 16^0
	int base = 1;

	int dec_val = 0;

	// Extracting characters as digits from last
	// character
	for (int decode_count = len - 1; decode_count >= start; decode_count--) {
		// if character lies in '0'-'9', converting
		// it to integral 0-9 by subtracting 48 from
		// ASCII value
		if (msdencoded[decode_count] >= '0' && msdencoded[decode_count] <= '9') {
			dec_val += (int(msdencoded[decode_count]) - 48) * base;

			// incrementing base by power
			base = base * 16;
		}

		// if character lies in 'A'-'F' , converting
		// it to integral 10 - 15 by subtracting 55
		// from ASCII value
		else if (msdencoded[decode_count] >= 'A' && msdencoded[decode_count] <= 'F') {
			dec_val += (int(msdencoded[decode_count]) - 55) * base;

			// incrementing base by power
			base = base * 16;
		}
	}
	return dec_val;
}


void decoding::structure(string eCallMessage){

cout<<"MSD ID: "<<decoding::decode(eCallMessage,0,2)<<endl;

cout<<"vehicle number: "<<decoding::decode(eCallMessage,2,4)<<endl;

cout<<"Activation: "<<decoding::automaticActivation(eCallMessage)<<endl;

cout<<"call: "<<decoding::testCall(eCallMessage)<<endl;

cout<<"call: "<<decoding::positionCanBeTrusted(eCallMessage)<<endl;

cout<<"vehicle Type: "<<decoding::vehicleType(eCallMessage)<<endl;

cout<<"compressed Natural Gas: "<<decoding::compressedNaturalGas(eCallMessage)<<endl;

cout<<"diesel Tank Present: "<<decoding::dieselTankPresent(eCallMessage)<<endl;

cout<<"electric Energy Storage: "<<decoding::electricEnergyStorage(eCallMessage)<<endl;

cout<<"gasoline Tank Present: "<<decoding::gasolineTankPresent(eCallMessage)<<endl;

cout<<"time: "<<decoding::decode(eCallMessage,8,9)<<endl;

cout<<"position Latitude: "<<decoding::decode(eCallMessage,9,17)<<endl;

cout<<"position Longitude: "<<decoding::decode(eCallMessage,17,25)<<endl;

cout<<"vehicle Direction: "<<decoding::decode(eCallMessage,25,27)<<endl;

cout<<"number Of Passengers: "<<decoding::decode(eCallMessage,27,28)<<endl;
}







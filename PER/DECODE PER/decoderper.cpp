#include "decodeper.h"


string automaticActivation(string eCallMessage)
{
    string activation;
    switch(decode(eCallMessage,4,5))
    {
    case 13:
    case 12:
    case 8:
    case 9:
        activation="automatic Activation";
        break;
    default:
                activation="manual Activation";

    }

    return activation;

}

string testCall(string eCallMessage)
{
    string test;
    switch(decode(eCallMessage,4,5))
    {
    case 13:
    case 12:
    case 4:
    case 5:
        test="test Call";
        break;
    default:
                test="emergency call";

    }

    return test;

}


string positionCanBeTrusted(string eCallMessage)
{
    string position;
    switch(decode(eCallMessage,4,5))
    {
    case 13:
    case 9:
    case 1:
    case 5:
        position="high confidence";
        break;
    default:
                position="low confidence";

    }

    return position;

}

string vehicleType(string eCallMessage){
 string vehicle;
    switch(decode(eCallMessage,5,7))
    {
    case 1:
        vehicle="PASSENGER_VEHICLE_CLASS_M1";
        break;
        case 2:
        vehicle="BUSES_AND_COACHES_CLASS_M2";
        break;
        case 3:
        vehicle="BUSES_AND_COACHES_CLASS_M3";
        break;
         case 4:
        vehicle="LIGHT_COMMERCIAL_VEHICLES_CLASS_N1";
        break;
        case 5:
        vehicle="HEAVY_DUTY_VEHICLES_CLASS_N2";
        break;
        case 6:
        vehicle="HEAVY_DUTY_VEHICLES_CLASS_N3";
        break;
        case 7:
        vehicle="MOTORCYCLE_CLASS_L1E";
        break;
        case 8:
        vehicle="MOTORCYCLE_CLASS_L2E";
        break;

    default:
                vehicle="MOTORCYCLE_CLASS_L3E";

    }

    return vehicle;

}


string compressedNaturalGas(string eCallMessage)
{
    string NaturalGas;
    switch(decode(eCallMessage,7,8))
    {
    case 15:
    case 14:
    case 13:
    case 12:
    case 11:
    case 10:
    case 9:
    case 8:
        NaturalGas="compressed Natural Gas";
        break;

    default:
              return "not used";
    }

    return NaturalGas;

}

string dieselTankPresent(string eCallMessage)
{
    string diesel;
    switch(decode(eCallMessage,7,8))
    {
    case 15:
    case 14:
    case 13:
    case 12:
    case 7:
    case 6:
    case 5:
    case 4:
        diesel="diesel Tank";
        break;
    default:
              return "not used";
    }

    return diesel;

}


string electricEnergyStorage(string eCallMessage)
{
    string electric;
    switch(decode(eCallMessage,7,8))
    {
    case 15:
    case 14:
    case 11:
    case 10:
    case 7:
    case 6:
    case 3:
    case 2:
        electric="electric Energy Storage";
        break;
    default:
              return "not used";
    }

    return electric;

}

string gasolineTankPresent(string eCallMessage)
{
    string gasoline;
    switch(decode(eCallMessage,7,8))
    {
    case 15:
    case 13:
    case 11:
    case 9:
    case 7:
    case 5:
    case 3:
    case 1:
        gasoline="gasoline Tank";
        break;
    default:
              return "not used";
    }

    return gasoline;

}


void structure(string eCallMessage){

cout<<"MSD ID: "<<decode(eCallMessage,0,2)<<"\n";
cout<<"vehicle number: "<<decode(eCallMessage,2,4)<<"\n";
cout<<"Activation: "<<automaticActivation(eCallMessage)<<"\n";
cout<<"call: "<<testCall(eCallMessage)<<"\n";
cout<<"call: "<<positionCanBeTrusted(eCallMessage)<<"\n";
cout<<"vehicle Type: "<<vehicleType(eCallMessage)<<"\n";
cout<<"compressed Natural Gas: "<<compressedNaturalGas(eCallMessage)<<"\n";
cout<<"diesel Tank Present: "<<dieselTankPresent(eCallMessage)<<"\n";
cout<<"electric Energy Storage: "<<electricEnergyStorage(eCallMessage)<<"\n";
cout<<"gasoline Tank Present: "<<gasolineTankPresent(eCallMessage)<<"\n";
cout<<"time: "<<decode(eCallMessage,8,9)<<"\n";
cout<<"position Latitude: "<<decode(eCallMessage,9,17)<<"\n";
cout<<"position Longitude: "<<decode(eCallMessage,17,25)<<"\n";
cout<<"vehicle Direction: "<<decode(eCallMessage,25,27)<<"\n";
cout<<"number Of Passengers: "<<decode(eCallMessage,27,28)<<"\n";
}


int decode(string msdencoded,char start,char len)
{


	// Initializing base value to 1, i.e 16^0
	int base = 1;

	int dec_val = 0;

	// Extracting characters as digits from last
	// character
	for (int i = len - 1; i >= start; i--) {
		// if character lies in '0'-'9', converting
		// it to integral 0-9 by subtracting 48 from
		// ASCII value
		if (msdencoded[i] >= '0' && msdencoded[i] <= '9') {
			dec_val += (int(msdencoded[i]) - 48) * base;

			// incrementing base by power
			base = base * 16;
		}

		// if character lies in 'A'-'F' , converting
		// it to integral 10 - 15 by subtracting 55
		// from ASCII value
		else if (msdencoded[i] >= 'A' && msdencoded[i] <= 'F') {
			dec_val += (int(msdencoded[i]) - 55) * base;

			// incrementing base by power
			base = base * 16;
		}
	}
	return dec_val;
}


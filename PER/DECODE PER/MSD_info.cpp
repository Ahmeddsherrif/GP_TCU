#include "decodeper.h"



string decoding::automaticActivation(string eCallMessage)
{
    string activation;
    switch(decoding::decode(eCallMessage,4,5))
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

string decoding::testCall(string eCallMessage)
{
    string test;
    switch(decoding::decode(eCallMessage,4,5))
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


string decoding::positionCanBeTrusted(string eCallMessage)
{
    string position;
    switch(decoding::decode(eCallMessage,4,5))
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

string decoding::vehicleType(string eCallMessage){
 string vehicle;
    switch(decoding::decode(eCallMessage,5,7))
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


string decoding::compressedNaturalGas(string eCallMessage)
{
    string NaturalGas;
    switch(decoding::decode(eCallMessage,7,8))
    {
    case 15:
    case 14:
    case 13:
    case 12:
    case 11:
    case 10:
    case 8:
    case 9:
        NaturalGas="used";
        break;

    default:
              return "not used";
    }

    return NaturalGas;

}

string decoding::dieselTankPresent(string eCallMessage)
{
    string diesel;
    switch(decoding::decode(eCallMessage,7,8))
    {
    case 15:
    case 14:
    case 13:
    case 12:
    case 7:
    case 6:
    case 5:
    case 4:
        diesel="used";
        break;
    default:
              return "not used";
    }

    return diesel;

}


string decoding::electricEnergyStorage(string eCallMessage)
{
    string electric;
    switch(decoding::decode(eCallMessage,7,8))
    {
    case 15:
    case 14:
    case 11:
    case 10:
    case 7:
    case 6:
    case 3:
    case 2:
        electric="used";
        break;
    default:
              return "not used";
    }

    return electric;

}

string decoding::gasolineTankPresent(string eCallMessage)
{
    string gasoline;
    switch(decoding::decode(eCallMessage,7,8))
    {
    case 15:
    case 13:
    case 11:
    case 9:
    case 7:
    case 5:
    case 3:
    case 1:
        gasoline="used";
        break;
    default:
              return "not used";
    }

    return gasoline;

}

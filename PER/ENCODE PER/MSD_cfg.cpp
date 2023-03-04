#include "encodeper.h"

tECallMessage MSD_cfg_data(){
tECallMessage ECallMessage;

ECallMessage.id = 29;
ECallMessage.msd.msdStructure.vehiclenumber = 1;
ECallMessage.msd.msdStructure.control.automaticActivation = true;
ECallMessage.msd.msdStructure.control.testCall = true;
ECallMessage.msd.msdStructure.control.positionCanBeTrusted = true;
ECallMessage.msd.msdStructure.control.vehicleType = PASSENGER_VEHICLE_CLASS_M1;
ECallMessage.msd.msdStructure.vehiclePropulsionStorageType.compressedNaturalGas = false;
ECallMessage.msd.msdStructure.vehiclePropulsionStorageType.dieselTankPresent = true;
ECallMessage.msd.msdStructure.vehiclePropulsionStorageType.electricEnergyStorage = false;
ECallMessage.msd.msdStructure.vehiclePropulsionStorageType.gasolineTankPresent = false;
ECallMessage.msd.msdStructure.timestamp = 4;
ECallMessage.msd.msdStructure.vehicleLocation.positionLatitude =16749 ;
ECallMessage.msd.msdStructure.vehicleLocation.positionLongitude = 574906;
ECallMessage.msd.msdStructure.vehicleDirection = 50;
ECallMessage.msd.msdStructure.numberOfPassengers = 3;

return ECallMessage;
}

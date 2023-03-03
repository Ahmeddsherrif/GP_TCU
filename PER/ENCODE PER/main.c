#include "encodeper.h"

#include "encodeper.h"
#include <stdint.h>

uint8_t encodeMSD( tECallMessage *eCallMessage, uint8_t *msdAsByte )
{



	appendBits( msdAsByte, eCallMessage->id, 8 );



	tMSD msd = eCallMessage->msd;


	tMSDStructure msdStructure = msd.msdStructure;

		appendBits( msdAsByte, msdStructure.vehiclenumber, 8 );

	tControlType control = msdStructure.control;
	appendBits( msdAsByte, control.automaticActivation, 1 );

	appendBits( msdAsByte, control.testCall, 1 );

	appendBits( msdAsByte, control.positionCanBeTrusted, 2 );


	appendBits( msdAsByte, control.vehicleType, 8 );



	tVehiclePropulsionStorageType vehiclePropulsionStorageType = msdStructure.vehiclePropulsionStorageType;

		appendBits( msdAsByte, vehiclePropulsionStorageType.compressedNaturalGas, 1 );
			appendBits( msdAsByte, vehiclePropulsionStorageType.dieselTankPresent, 1 );
				appendBits( msdAsByte, vehiclePropulsionStorageType.electricEnergyStorage, 1 );
	appendBits( msdAsByte, vehiclePropulsionStorageType.gasolineTankPresent, 1 );



	appendBits( msdAsByte, msdStructure.timestamp, 4 );


	appendBits( msdAsByte, msdStructure.vehicleLocation.positionLatitude, 32 );
	appendBits( msdAsByte, msdStructure.vehicleLocation.positionLongitude, 32 );

	appendBits( msdAsByte, msdStructure.vehicleDirection, 8 );

		appendBits( msdAsByte, msdStructure.numberOfPassengers, 4 );

	return byteIndex + 1;
}

void appendBits( uint8_t *msdAsByte, uint32_t src, uint8_t length )
{
	position = bitIndex;
	if ( length <= position )
	{
		msdAsByte[byteIndex] |= src << ( position - length );
		updateIndexes ( length );
	}
	else
	{
		msdAsByte[byteIndex] |= ( src >> (length - position) ) & 0xFF;
		updateIndexes ( bitIndex );
		appendBits( msdAsByte, src, length - position );
	}
}

void updateIndexes(uint8_t consumedBits )
{
	if ( !( consumedBits % 8 ) )
	{
		byteIndex += ( consumedBits / 8 );
	}
	else if ( bitIndex <= consumedBits )
	{
	bitIndex = ( 8 - ( consumedBits - bitIndex ) % 8 );
		byteIndex += ( ( consumedBits - bitIndex ) / 8 ) + 1;
	}
	else
	{
		bitIndex -= consumedBits;
	}
}

uint8_t getEncodeValue( char character) {
	uint8_t adjustNumber;

	if ( character >= 'A' && character <= 'H' )
	{
		adjustNumber = 55;
	}
	else if ( character >= 'J' && character <= 'N' )
	{
		adjustNumber = 56;
	}
	else if ( character == 'P' )
	{
		adjustNumber = 57;
	}
	else if ( character >= 'R' && character <= 'Z' )
	{
		adjustNumber = 58;
	}
	else if ( character >= '0' && character <= '9' )
	{
		adjustNumber = '0';
	}
	else
	{
		adjustNumber = character;
	}

	return character - adjustNumber;
}


int main(){

int cnt;
tECallMessage ECallMessage;
uint8_t msd[140];
uint8_t msdsize;

memset(&ECallMessage, 0, sizeof(ECallMessage));
memset(msd, 0, sizeof(msd));

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





msdsize = encodeMSD(&ECallMessage, &msd[0]);

printf("MSD[%u]: ", msdsize);
for (cnt = 0; cnt < msdsize; cnt++)
  printf("%.2X", msd[cnt]);
printf("\n");


return 0;
}


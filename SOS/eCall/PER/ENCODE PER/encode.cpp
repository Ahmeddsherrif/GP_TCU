#include "encodeper.h"


uint8_t encoding::encodeMSD( tECallMessage *eCallMessage, uint8_t *msdAsByte )
{
	tMSD msd = eCallMessage->msd;
    tMSDStructure msdStructure = msd.msdStructure;
    tControlType control = msdStructure.control;
    tVehiclePropulsionStorageType vehiclePropulsionStorageType = msdStructure.vehiclePropulsionStorageType;

	encoding::appendBits( msdAsByte, eCallMessage->id, 8 );

    encoding::appendBits( msdAsByte, msdStructure.vehiclenumber, 8 );

	encoding::appendBits( msdAsByte, control.automaticActivation, 1 );

	encoding::appendBits( msdAsByte, control.testCall, 1 );

	encoding::appendBits( msdAsByte, control.positionCanBeTrusted, 2 );

	encoding::appendBits( msdAsByte, control.vehicleType, 8 );

    encoding::appendBits( msdAsByte, vehiclePropulsionStorageType.compressedNaturalGas, 1 );

    encoding::appendBits( msdAsByte, vehiclePropulsionStorageType.dieselTankPresent, 1 );

    encoding::appendBits( msdAsByte, vehiclePropulsionStorageType.electricEnergyStorage, 1 );

	encoding::appendBits( msdAsByte, vehiclePropulsionStorageType.gasolineTankPresent, 1 );

	encoding::appendBits( msdAsByte, msdStructure.timestamp, 4 );

	encoding::appendBits( msdAsByte, msdStructure.vehicleLocation.positionLatitude, 32 );

	encoding::appendBits( msdAsByte, msdStructure.vehicleLocation.positionLongitude, 32 );

	encoding::appendBits( msdAsByte, msdStructure.vehicleDirection, 8 );

    encoding::appendBits( msdAsByte, msdStructure.numberOfPassengers, 4 );

	return byteIndex + 1;
}

void encoding::appendBits( uint8_t *msdAsByte, uint32_t src, uint8_t length )
{
	position = bitIndex;
	if ( length <= position )
	{
		msdAsByte[byteIndex] |= src << ( position - length );
		encoding::updateIndexes ( length );
	}
	else
	{
		msdAsByte[byteIndex] |= ( src >> (length - position) ) & 0xFF;
		encoding::updateIndexes ( bitIndex );
		encoding::appendBits( msdAsByte, src, length - position );
	}
}

void encoding::updateIndexes(uint8_t consumedBits )
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

uint8_t encoding::getEncodeValue( char character) {
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

#ifndef ENCODEPER_H_
#define ENCODEPER_H_

#ifndef ECALL_MSD_MAX_LENGTH
#define ECALL_MSD_MAX_LENGTH	(140)	/* maximum length of MSD message in bytes */
#endif



#ifndef VEHICLE_PROPULSION_STORAGE_TYPE_SIZE
#define VEHICLE_PROPULSION_STORAGE_TYPE_SIZE	(6)
#endif






#include <stdint.h>

#define bool int
#define true 1
#define false 0


typedef enum{
	PASSENGER_VEHICLE_CLASS_M1 			=1,
	BUSES_AND_COACHES_CLASS_M2 			= 2,
	BUSES_AND_COACHES_CLASS_M3 			= 3,
	LIGHT_COMMERCIAL_VEHICLES_CLASS_N1	 	= 4,
	HEAVY_DUTY_VEHICLES_CLASS_N2 			= 5,
	HEAVY_DUTY_VEHICLES_CLASS_N3 			= 6,
	MOTORCYCLE_CLASS_L1E 				= 7,
	MOTORCYCLE_CLASS_L2E 				= 8,
	MOTORCYCLE_CLASS_L3E 				= 9,

}tVehicleType;

typedef struct{
	bool automaticActivation;	/* true = Automatic activiation, false = Manual activation */
	bool testCall;				/* true = Test call, false = Emergency */
	bool positionCanBeTrusted;	/* true = Position can be trusted, false = low confidence in position - "Low confidence in position" if not within limits of +-150m with 95% confidence */
	tVehicleType vehicleType;
}tControlType;


typedef struct{
	bool gasolineTankPresent; 	/* DEFAULT FALSE */
	bool dieselTankPresent; 	/* DEFAULT FALSE */
	bool compressedNaturalGas; 	/* DEFAULT FALSE */
	bool electricEnergyStorage; 	/* DEFAULT FALSE */
}tVehiclePropulsionStorageType;


typedef struct{
	int32_t positionLatitude;	/* INTEGER(-2147483648..2147483647), value 0x7FFFFFFF = 2147483647 shall be used in case of invalid or unknown */
	int32_t positionLongitude;	/* INTEGER(-2147483648..2147483647), value 0x7FFFFFFF = 2147483647 shall be used in case of invalid or unknown */
}tVehicleLocation;


typedef struct{
    	uint8_t vehiclenumber;
	tControlType control;
	tVehiclePropulsionStorageType vehiclePropulsionStorageType;
	uint32_t timestamp;
	tVehicleLocation vehicleLocation;
	uint8_t vehicleDirection;
	uint8_t numberOfPassengers;
}tMSDStructure;



typedef struct{
	tMSDStructure msdStructure;
}tMSD;

typedef struct{
	uint8_t id;
	tMSD msd;
}tECallMessage;



/**
* static global variables
*/
uint8_t byteIndex = 0;
uint8_t bitIndex = 8;
uint8_t position;

/* convert character to actual encode value */
uint8_t getEncodeValue( char character);

void updateIndexes( uint8_t consumedBits );

/* serialize MSD bits into MSD buffer */
void appendBits( uint8_t *msdAsByte, uint32_t src, uint8_t length );


#endif

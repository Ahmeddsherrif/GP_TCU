#ifndef MSD_DATA_H_INCLUDED
#define MSD_DATA_H_INCLUDED

#include "encodeper.h"


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
	int32_t positionLatitude;
	int32_t positionLongitude;
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


tECallMessage MSD_cfg_data();


#endif // MSD_DATA_H_INCLUDED

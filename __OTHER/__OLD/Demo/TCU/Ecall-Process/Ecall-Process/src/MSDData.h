/*
 * MSDData.h
 *
 *  Created on: Mar 10, 2023
 *      Author: ahmed
 */

#ifndef MSDDATA_H_
#define MSDDATA_H_


#define BITS_MSD_ID					8
#define BITS_VEHICLE_NUMBER			32
#define	BITS_ACTIVATION				1
#define BITS_TEST_CALL				1
#define BITS_POSITION_TRUSTED		1
#define BITS_VEHICLE_TYPE			3
#define BITS_PPROPULSION_TYPE		4
#define BITS_TIMESTAMP				32
#define BITS_LATITUDE				32
#define BITS_LONGITUDE				32
#define BITS_VEHICLE_DIRECTION		32
#define BITS_NUMBER_OF_PASSENGERS	3



enum class tVehiclePropulsionStorageType {
		GASOLINE_TANK,
		DIESEL_TANK,
		NATURAL_GAS,
		ELECTRIC_ENERGY
};

struct tVehicleLocation {
		float positionLatitude;
		float positionLongitude;
};

enum class tVehicleType {
	PASSENGER_VEHICLE_CLASS_M1,
	BUSES_AND_COACHES_CLASS_M2,
	BUSES_AND_COACHES_CLASS_M3,
	LIGHT_COMMERCIAL_VEHICLES_CLASS_N1,
	HEAVY_DUTY_VEHICLES_CLASS_N2,
	HEAVY_DUTY_VEHICLES_CLASS_N3,
	MOTORCYCLE_CLASS_L1E,
	MOTORCYCLE_CLASS_L2E,
	MOTORCYCLE_CLASS_L3E
};

struct tControlType {
		bool automaticActivation; /* true = Automatic activiation, false = Manual activation */
		bool testCall; /* true = Test call, false = Emergency */
		bool positionCanBeTrusted; /* true = Position can be trusted, false = low confidence in position - "Low confidence in position" if not within limits of +-150m with 95% confidence */
		tVehicleType vehicleType;
};

struct tMSDStructure {
		uint32_t vehicleDirection;
		uint32_t numberOfPassengers;
		uint32_t vehiclenumber;
//		char timestamp[25];
		tVehicleLocation vehicleLocation;
		tVehiclePropulsionStorageType vehiclePropulsionStorageType;
		tControlType control;
};

struct tMSD {
		tMSDStructure msdStructure;
};

struct tECallMessage {
		uint32_t id;
		tMSD msd;
};


#endif /* MSDDATA_H_ */

#ifndef MSD_DATA_H_INCLUDED
#define MSD_DATA_H_INCLUDED


#define BITS_MSD_ID					8
#define BITS_VEHICLE_NUMBER			8
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
		int32_t positionLatitude;
		int32_t positionLongitude;
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
		uint32_t timestamp;
		tVehicleLocation vehicleLocation;
		tVehiclePropulsionStorageType vehiclePropulsionStorageType;
		tControlType control;
};

struct tMSD {
		tMSDStructure msdStructure;
		//optional data
};

struct tECallMessage {
		uint32_t id;
		tMSD msd;
};

class MSD {
	public:
		MSD();
		void config();
		void decode(const string&);
		string encode();
		void print();

	private:
		tECallMessage ECallMessage;
		BitStreamDecoder decoder;
		BitStreamEncoder encoder;

		static constexpr uint8_t structElementsNumber = 12;
		static constexpr uint8_t parseSequence[structElementsNumber] = {
			BITS_MSD_ID,
			BITS_VEHICLE_NUMBER,
			BITS_ACTIVATION,
			BITS_TEST_CALL,
			BITS_POSITION_TRUSTED,
			BITS_VEHICLE_TYPE,
			BITS_PPROPULSION_TYPE,
			BITS_TIMESTAMP,
			BITS_LATITUDE,
			BITS_LONGITUDE,
			BITS_VEHICLE_DIRECTION,
			BITS_NUMBER_OF_PASSENGERS
		};
		static constexpr uint8_t printWidth = 30;
		static constexpr char printFillChar = '.';

};

#endif // MSD_DATA_H_INCLUDED

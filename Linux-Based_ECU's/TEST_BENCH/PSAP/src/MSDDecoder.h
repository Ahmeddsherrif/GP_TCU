/*
 * MSDDecoder.h
 *
 *  Created on: Mar 10, 2023
 *      Author: ahmed
 */

#ifndef MSDDECODER_H_
#define MSDDECODER_H_

class MSDDecoder {
	public:

		MSDDecoder();
		void decode(const string&);
		void log(const string& fileDir);
		void print();

	private:

		tECallMessage ECallMessage;
		BitStreamDecoder decoder;

		string id;
		string vehicleNumber;
		string timestamp;
		string activation;
		string threatDetected;
		string positionTrusted;
		string vehicleType;
		string propulsionType;
		string latitude;
		string longitude;
		string vehicleDirection;
		string numberOfPassengers;

		static constexpr int structElementsNumber = 11;
		static constexpr int parseSequence[structElementsNumber] = {
			BITS_MSD_ID,
			BITS_VEHICLE_NUMBER,
			BITS_ACTIVATION,
			BITS_TEST_CALL,
			BITS_POSITION_TRUSTED,
			BITS_VEHICLE_TYPE,
			BITS_PPROPULSION_TYPE,
//			BITS_TIMESTAMP,
			BITS_LATITUDE,
			BITS_LONGITUDE,
			BITS_VEHICLE_DIRECTION,
			BITS_NUMBER_OF_PASSENGERS
		};
		static constexpr int printWidth = 30;
		static constexpr char printFillChar = '*';
		static constexpr int printFillWidth = 60;

		void parseDecodedMSDToStrings();
		string getTimestamp();
};

#endif /* MSDDECODER_H_ */

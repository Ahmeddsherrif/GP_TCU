/*
 * MSDEncoder.h
 *
 *  Created on: Mar 10, 2023
 *      Author: ahmed
 */

#ifndef MSDENCODER_H_
#define MSDENCODER_H_

class MSDEncoder {

	public:
		MSDEncoder(const string& msdConfigFilePath);

		void setActivation(const bool&);
		void setTestCall(const bool&);
		void setPositionTrust(const bool&);
		void setPositionLatitude(const float&);
		void setPositionLongitude(const float&);
		void setVehicleDirection(const uint32_t&);
		void setNumberOfPassengers(const uint32_t&);

		string getEmergencyPhoneNumber();

		string encode();


	private:
		tECallMessage ECallMessage;
		BitStreamEncoder encoder;

		string emergencyPhoneNumber;



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

};

#endif /* MSDENCODER_H_ */

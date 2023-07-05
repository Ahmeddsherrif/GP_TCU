/*
 * GyroscopeSensor.h
 *
 *  Created on: Feb 27, 2023
 *      Author: ahmed
 */

#ifndef GYROSCOPESENSOR_H_
#define GYROSCOPESENSOR_H_

class GyroscopeSensor : public SensorFile {
	public:
		GyroscopeSensor(const string& fileName);
		string getYawRate(const int& sampleNumber);
};

#endif /* GYROSCOPESENSOR_H_ */

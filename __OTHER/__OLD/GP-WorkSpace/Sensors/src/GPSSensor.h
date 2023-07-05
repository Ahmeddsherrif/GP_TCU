/*
 * GPSSensor.h
 *
 *  Created on: Feb 27, 2023
 *      Author: ahmed
 */

#ifndef GPSSENSOR_H_
#define GPSSENSOR_H_


class GPSSensor : public SensorFile {
	public:
		GPSSensor(const string &fileName) ;
		string getGPS(const int& sampleNumber);
};

#endif /* GPSSENSOR_H_ */

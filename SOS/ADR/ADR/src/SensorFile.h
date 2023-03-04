/*
 * SensorFile.h
 *
 *  Created on: Feb 27, 2023
 *      Author: ahmed
 */

#ifndef SENSORFILE_H_
#define SENSORFILE_H_

class SensorFile {

	public:

		int getNumberOfSamples() {
			return numberOfSamples;
		}

	protected:

		SensorFile(const string &fileName);
		vector<string> getSensorData(const int &sampleNumber);

	private:

		vector<string> sensorData;
		int numberOfSamples;
};

#endif /* SENSORFILE_H_ */

/*
 * Serial.h
 *
 *  Created on: Feb 18, 2023
 *      Author: ahmed
 */

#ifndef SERIAL_H_
#define SERIAL_H_




class Serial {
	public:
		Serial();
		Error_t begin(const char *device, speed_t baudrate);
		Error_t send(const string &writeData);
		Error_t recieve(string &readData);
		Error_t dump();
		virtual ~Serial();

	private:
		int fd;
		char serialBuffer[SERIAL_BUFFER_MAX_SIZE];
		string response;
		int responseFrameStartIndex;
};

#endif /* SERIAL_H_ */

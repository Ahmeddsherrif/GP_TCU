/*
 * Serial.cpp
 *
 *  Created on: Feb 18, 2023
 *      Author: ahmed
 */

#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

#include <string>
#include <vector>

#include <thread>
#include <chrono>

using namespace std;

#include "error.h"
#include "configuration.h"

#include "Serial.h"

Serial::Serial() {
	fd = -1;
	responseFrameStartIndex = 0;
	response = "";

}

Error_t Serial::begin(const char *device, speed_t baudrate) {

	Error_t rtnError = Error_t::NO_ERROR;

	do {
		fd = open(device, O_RDWR | O_NOCTTY | O_NDELAY);

		if (fd == -1) {
			rtnError = Error_t::CANT_OPEN_DEVICE_FILE;
			break;
		}

		struct termios options;
		tcgetattr(fd, &options);
		options.c_cflag = baudrate | CS8 | CLOCAL | CREAD;
		options.c_iflag = IGNPAR;
		options.c_oflag = 0;
		options.c_lflag = 0;
		tcflush(fd, TCIFLUSH);
		tcsetattr(fd, TCSANOW, &options);

	} while (false);

	return rtnError;
}

Error_t Serial::send(const string &data) {
	Error_t rtnError = Error_t::NO_ERROR;

	string data_2_send = data + "\r\n";

	int bytes_written = write(fd, data_2_send.c_str(), data_2_send.length());

	std::this_thread::sleep_for(std::chrono::milliseconds(500));

	if (bytes_written == -1) {
		rtnError = Error_t::BYTES_NOT_WRITTEN;
	}

	return rtnError;
}

Error_t Serial::recieve(string &readData) {
	Error_t rtnError = Error_t::NO_ERROR;

	readData = "";

	int bytesRead = read(fd, serialBuffer, SERIAL_BUFFER_MAX_SIZE);

	if (bytesRead > 0) {
		response.append(serialBuffer, bytesRead);
	} else if (bytesRead < 0) {
		rtnError = Error_t::READ_FROM_SERIAL_ERROR;
	}


	if(response != ""){
		size_t currentIndexPosition = response.find("\r\n", responseFrameStartIndex);

		if (currentIndexPosition != string::npos) {
			int frameLength = currentIndexPosition - responseFrameStartIndex;
			readData = response.substr(responseFrameStartIndex, frameLength);
			response = response.erase(0, currentIndexPosition + 2);
			responseFrameStartIndex = 0;
		}
	}

	return rtnError;
}

Error_t Serial::dump() {
	Error_t rtnError = Error_t::NO_ERROR;

	read(fd, serialBuffer, SERIAL_BUFFER_MAX_SIZE);

	response.clear();
	response = "";

	return rtnError;
}

Serial::~Serial() {
	if (fd != -1) {
		close(fd);
	}
}


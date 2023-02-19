/*
 * error.h
 *
 *  Created on: Feb 18, 2023
 *      Author: ahmed
 */

#ifndef ERROR_H_
#define ERROR_H_

enum class Error_t {
		NO_ERROR,
		CANT_OPEN_DEVICE_FILE,
		BYTES_NOT_WRITTEN,
		READ_FROM_SERIAL_ERROR,
		MOBILE_CMD_NACK_ERROR,
		UNEXPECTED_RESPONSE
};



#endif /* ERROR_H_ */

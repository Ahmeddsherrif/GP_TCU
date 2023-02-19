/*
 * Event.h
 *
 *  Created on: Feb 18, 2023
 *      Author: ahmed
 */

#ifndef EVENT_H_
#define EVENT_H_


enum class eventType {
		MANUAL_CMD,
		SERIAL_CMD,
		SMS_CMD,
		IPC_CMD
};

struct Event {

	eventType myEventType;
	string data;

};

#endif /* EVENT_H_ */

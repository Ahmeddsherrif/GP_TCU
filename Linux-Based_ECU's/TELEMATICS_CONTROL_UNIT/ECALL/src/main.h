/*
 * main.h
 *
 *  Created on: Jun 28, 2023
 *      Author: ahmed
 */

#ifndef MAIN_H_
#define MAIN_H_

#include <iostream>
#include <string.h>

#include <thread>
#include <chrono>

#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

#include <string>
#include <vector>
#include <cstdio>

#include <queue>
#include <mutex>
#include <sstream>

#include <mosquitto.h>

using namespace std;

#define BROKER_ADDRESS				"192.168.1.11"
#define BROKER_PORT					1883

#define TOPIC_ADR					"sensors/adr"
#define TOPIC_IMU					"sensors/imu"

#define TOPIC_SOS					"sos"
#define MESSAGE_ECALL				"ecall"
#define MESSAGE_BCALL				"bcall"
#define MESSAGE_GUN					"gun"
#define MESSAGE_SIGN				"sign"
#define MESSAGE_END					"end"

#define TOPIC_SEATBELT 					 "seatbelt"
#define MESSAGE_REQUEST_SEAT_BELT        "request"
#define MESSAGE_RIGHT_SEAT_BELT_HOOKED   "right_hooked"
#define MESSAGE_LEFT_SEAT_BELT_HOOKED    "left_hooked"
#define MESSAGE_RIGHT_SEAT_BELT_UNHOOKED "right_unhooked"
#define MESSAGE_LEFT_SEAT_BELT_UNHOOKED  "left_unhooked"

#define TOPIC_CMD				    "cmd"
#define TOPIC_CMD_PROCESS			"cmd/ecall"
#define MESSAGE_START				"start"
#define MESSAGE_KILL				"kill"
#define MESSAGE_TERMINATE			"terminate"
#define MESSAGE_STATUS				"status"

#define TOPIC_STATUS_PROCESS		"status/ecall"

#define MESSAGE_STATUS_IDLE			"idle"
#define MESSAGE_STATUS_SOS			"sos"
#define MESSAGE_STATUS_CALL			"call"
#define MESSAGE_STATUS_DEAD			"dead"
#define MESSAGE_STATUS_TERMINATE	"terminate"

#define STATUS_OUTAGE				"outage"
#define STATUS_NO_OUTAGE			"no_outage"

#define SMS_CMD_END					"END"

#include "error.h"
#include "configuration.h"
#include "Utilities.h"
#include "debug.h"

#include "MSDData.h"
#include "BitStreamEncoder.h"
#include "MSDEncoder.h"

#include "Serial.h"
#include "Mobile.h"

struct GPSLocation {
	float latitude; // in degrees
	float longitude; // in degrees
	//double horizontalAccuracy;
};

struct varMSD {

	varMSD() {
		this->clear();
	}

	void clear(){
		memset(this, 0, sizeof(*this));
	}

	bool activation;
	bool threatDetected;
	GPSLocation gpsLocation;
	bool PositionTrust;
	float direction;
	bool passenger_left;
	bool passenger_right;
};

#define MSD_AUTOMATIC_ACTIVATION		true
#define MSD_MANUAL_ACTIVATION			false
#define MSD_THREAT						true
#define MSD_NO_THREAT					false
#define MSD_POSITION_TRUSTED			true
#define MSD_POSITION_ESTIMATED			false

#define TRACE_PRINT(line)  			cout << line << endl



#define STATE_SYS_INTIAL			STATE_SYS_DEAD

enum SystemState {
	STATE_SYS_DEAD, STATE_SYS_IDLE, STATE_SYS_SOS, STATE_SYS_CALL,
};

enum SystemEvent {
	EVENT_SYS_IDLE,
	EVENT_SYS_START,
	EVENT_SYS_KILL,
	EVENT_SYS_END,
	EVENT_SYS_ECALL,
	EVENT_SYS_BCALL,
	EVENT_SYS_SIGN,
	EVENT_SYS_GUN,
	EVENT_SYS_CALL_IN,
	EVENT_SYS_CALL_OUT,
	EVENT_SYS_TERMINATE,
	EVENT_STATUS,
	EVENT_SYS_ADR_RECIEVED,
	EVENT_SEATBELT_RECIEVED,
	EVENT_IMU_RECIEVED
};

struct SystemEventMessage {
	SystemEvent event;
	string data;

	SystemEventMessage() :
			SystemEventMessage(EVENT_SYS_IDLE, "") {

	}

	SystemEventMessage(const string &data) :
			SystemEventMessage(EVENT_SYS_IDLE, data) {

	}

	SystemEventMessage(const SystemEvent &event, const string &data) :
			event { event }, data { data } {

	}

};

// Function Declerations
void state_sys_dead_handler();
void state_sys_dead_handler_processing();

void state_sys_idle_handler();
void state_sys_idle_handler_processing();

void state_sys_sos_handler();
void state_sys_sos_handler_processing();

void state_sys_call_handler();
void state_sys_call_handler_processing();

void system_onetime_intialization();
void system_constant_reintialization();
void system_end();

void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg);
void on_connect(struct mosquitto *mosq, void *obj, int rc);

void getEvents();
void sendMSD();

// Shared Resources
extern queue<SystemEventMessage> queueCurrentSystemEventMessage;
extern mutex mutexQueueCurrentSystemEventMessage;

// State Machine Variables
extern bool stateSysEntry;
extern bool stateSysExit;
extern SystemEventMessage currentSystemEventMessage;
extern SystemState currentSystemState;

// Global Variables
extern struct mosquitto *mosq;
extern Mobile myMobile;
extern varMSD currentVarMSD;

#endif /* MAIN_H_ */

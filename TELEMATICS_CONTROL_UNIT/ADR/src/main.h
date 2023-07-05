/*
 * main.h
 *
 *  Created on: Jun 28, 2023
 *      Author: ahmed
 */

#ifndef MAIN_H_
#define MAIN_H_

#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <queue>
#include <string.h>
#include <string>
#include <cstdlib>
#include <sstream>
#include <mutex>
#include <chrono>
#include <thread>
#include <iostream>
#include <sstream>

#include <mosquitto.h>

using namespace std;

#include "ADR.h"

#define BROKER_ADDRESS				"192.168.1.11"
#define BROKER_PORT					1883

#define TOPIC_IMU					"sensors/imu"
#define TOPIC_SPEEDOMETER			"sensors/speedometer"
#define TOPIC_GPS					"sensors/gps"
#define TOPIC_ADR					"sensors/adr"

#define TOPIC_CMD				    "simulation/cmd"
#define MESSAGE_START				"start"
#define MESSAGE_KILL				"kill"
#define MESSAGE_TERMINATE			"terminate"
#define MESSAGE_SEND_STATUS			"status"

#define STATUS_OUTAGE				"outage"
#define STATUS_NO_OUTAGE			"no_outage"


#define STATE_SYS_INTIAL			STATE_SYS_DEAD

enum SystemState {
	STATE_SYS_DEAD,
	STATE_SYS_ACTIVE
};

enum SystemEvent {
	EVENT_SYS_IDLE,
	EVENT_SYS_START,
	EVENT_SYS_KILL,
	EVENT_SYS_GPS_SAMPLE_RECIVED,
	EVENT_SYS_IMU_SAMPLE_RECIVED,
	EVENT_SYS_SPEEDOMETER_SAMPLE_RECIVED,
	EVENT_SYS_TERMINATE
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

void state_sys_active_handler();
void state_sys_active_handler_processing();
void state_sys_active_process_event_gps_sample_recieved();

void system_onetime_intialization();
void system_constant_reintialization();

void system_end();

void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg);
void on_connect(struct mosquitto *mosq, void *obj, int rc);


// Shared Resources
extern queue<SystemEventMessage> queueCurrentSystemEventMessage;
extern mutex mutexQueueCurrentSystemEventMessage;

// State Machine Variables
extern bool stateSysEntry;
extern bool stateSysExit;
extern SystemEventMessage currentSystemEventMessage;
extern SystemState currentSystemState;

// Global Variables
extern queue<double> queue_sensorReading_gyro;
extern queue<double> queue_sensorReading_speedometer;
extern queue<GPSLocation> queue_sensorReading_gps;
extern outageStatus currentOutageStatus;

extern struct mosquitto *mosq;

#define TRACE_PRINT(line)  cout << line << endl

#endif /* MAIN_H_ */

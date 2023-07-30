/*
 * main.h
 *
 *  Created on: Jun 28, 2023
 *      Author: ahmed
 */

#ifndef MAIN_H_
#define MAIN_H_

#include <stdio.h>
#include <mosquitto.h>

#include <iostream>
#include <chrono>
#include <thread>
#include <string>
#include <vector>
#include <string.h>
#include <mutex>
#include <queue>
#include <memory>

using namespace std;
using namespace chrono;

#include "SensorFile.h"
#include "GPSSensor.h"
#include "GyroscopeSensor.h"
#include "SpeedometerSensor.h"

//#define PROCESS_NAME				"sensors"

#define DATA_DIR 					"/home/ahmed/Desktop/TEST_BENCH/Sensors/Data/"

#define BROKER_ADDRESS				"192.168.1.11"
#define BROKER_PORT					1883

#define DATA_IMU_FILE				"Orientationx.csv"
#define DATA_SPEEDOMETER_FILE		"Speedx.csv"
#define DATA_GPS_FILE				"Positionx.csv"

#define PERIOD_IMU					10
#define PERIOD_SPEEDOMETER			40
#define PERIOD_GPS					1000


#define TOPIC_CMD					"cmd"
#define TOPIC_CMD_PROCESS			"cmd/sensors"
#define MESSAGE_START				"start"
#define MESSAGE_KILL				"kill"
#define MESSAGE_TERMINATE			"terminate"
#define MESSAGE_STATUS				"status"
#define MESSAGE_PAUSE				"pause"
#define MESSAGE_RESUME				"resume"

#define TOPIC_STATUS_PROCESS		"status/sensors"
#define MESSAGE_STATUS_ACTIVE		"active"
#define MESSAGE_STATUS_DEAD			"dead"
#define MESSAGE_STATUS_HALT			"halt"
#define MESSAGE_STATUS_TERMINATE	"terminate"

#define TOPIC_IMU					"sensors/imu"
#define TOPIC_SPEEDOMETER			"sensors/speedometer"
#define TOPIC_GPS					"sensors/gps"

#define TOPIC_OUTAGE				"sensors/outage"
#define MESSAGE_OUTAGE				"outage"
#define MESSAGE_NO_OUTAGE			"no_outage"

#define STATE_SYS_INTIAL			STATE_SYS_DEAD

enum SystemState {
	STATE_SYS_DEAD,
	STATE_SYS_ACTIVE,
	STATE_SYS_HALTED
};

enum SystemEvent {
	EVENT_SYS_IDLE,
	EVENT_SYS_START,
	EVENT_SYS_KILL,
	EVENT_SYS_RESUME,
	EVENT_SYS_OUTAGE_STATUS_RECIVED,
	EVENT_SYS_PAUSE,
	EVENT_STATUS,
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

void state_sys_halted_handler();
void state_sys_halted_handler_processing();

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
extern string outageStatus;

extern string gyroFileName;
extern unique_ptr<GyroscopeSensor> gyro;
extern int maxGyroSampleNumber;
extern int gyroSampleNumber;
extern string currentGyroSample;

extern string speedometerFileName;
extern unique_ptr<SpeedometerSensor> speedometer;
extern int maxSpeedometerSampleNumber;
extern int speedometerSampleNumber;
extern string currentSpeedSample;

extern string gpsFileName;
extern unique_ptr<GPSSensor> gps;
extern int maxGpsSampleNumber;
extern int gpsSampleNumber;
extern string currentGpsSample;

extern steady_clock::time_point lastTimeImu;
extern steady_clock::time_point lastTimeSpeedometer;
extern steady_clock::time_point lastTimeGps;

extern struct mosquitto *mosq;

#define TRACE_PRINT(line)  cout << line << endl

#endif /* MAIN_H_ */

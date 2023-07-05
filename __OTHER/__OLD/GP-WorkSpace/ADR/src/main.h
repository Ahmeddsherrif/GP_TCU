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
#include <cstdlib>
#include <sstream>
#include <mutex>
#include <chrono>
#include <thread>
#include <iostream>

#include <mosquitto.h>

using namespace std;

#define TOPIC_IMU					"sensors/imu"
#define TOPIC_SPEEDOMETER			"sensors/speedometer"
#define TOPIC_GPS					"sensors/gps"
#define TOPIC_ADR					"sensors/adr"

#define TOPIC_CMD				    "simulation/cmd"
#define MESSAGE_START				"start"
#define MESSAGE_KILL				"kill"

#define STATUS_OUTAGE				"outage"
#define STATUS_NO_OUTAGE			"no_outage"

struct GPSLocation {
	double latitude; // in degrees
	double longitude; // in degree
};



enum outageStatus {
	OUTAGE, NO_OUTAGE
};

#define STATUS_OUTAGE				"outage"
#define STATUS_NO_OUTAGE			"no_outage"



enum mainEvent{
	EVENT_IDLE,
	EVENT_NEW_GPS_SAMPLE
};

enum mainState{
	STATE_IDLE,
	STATE_PROCESS_GPS_SAMPLE
};



#include "ADR.h"


#endif /* MAIN_H_ */

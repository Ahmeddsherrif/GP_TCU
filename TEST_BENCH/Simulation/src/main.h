/*
 * main.h
 *
 *  Created on: Jun 29, 2023
 *      Author: ahmed
 */

#ifndef MAIN_H_
#define MAIN_H_


#include <iostream>
#include <stdlib.h>
#include <string>

#include <chrono>
#include <thread>

#include <mosquitto.h>

using namespace std;

#define BROKER_ADDRESS				"192.168.1.11"
#define BROKER_PORT					1883

#define TOPIC_OUTAGE				"simulation/outage"
#define MESSAGE_OUTAGE				"outage"
#define MESSAGE_NO_OUTAGE			"no_outage"

#define TOPIC_SOS					"sos"
#define MESSAGE_ECALL				"ecall"
#define MESSAGE_BCALL				"bcall"
#define MESSAGE_SIGN				"sign"
#define MESSAGE_GUN					"gun"

#define TOPIC_CONTROL_VIDEO			"simulation/control/video"
#define TOPIC_CONTROL_SENSORS		"simulation/control/sensors"
#define MESSAGE_PAUSE				"pause"
#define MESSAGE_RESUME				"resume"

#define TOPIC_CMD				    "simulation/cmd"
#define MESSAGE_START				"start"
#define MESSAGE_KILL				"kill"
#define MESSAGE_TERMINATE			"terminate"

#define TOPIC_DMS                   "dms"
#define MESSAGE_SLEEP               "sleep"
#define MESSAGE_DISTRACTED          "distracted"




#endif /* MAIN_H_ */

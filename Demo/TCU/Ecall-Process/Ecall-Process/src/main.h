/*
 * main.h
 *
 *  Created on: Feb 18, 2023
 *      Author: ahmed
 */

#ifndef MAIN_H_
#define MAIN_H_


#include <iostream>

#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

#include <string>
#include <vector>
#include <cstdio>

#include <queue>
#include <mutex>

#include <thread>
#include <chrono>

#include <cstring>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>

using namespace std;


#include "error.h"
#include "configuration.h"
#include "Utilities.h"
#include "debug.h"

#include "MessageQueue.h"

#include "MSDData.h"
#include "BitStreamEncoder.h"
#include "MSDEncoder.h"

#include "Serial.h"
#include "Mobile.h"

#define EVENT_ECALL 	"ECALL"
#define EVENT_END		"END"
#define EVENT_HANG		"HANG"
#define EVENT_KILL		"KILL"



struct GPSLocation {
		float latitude; // in degrees
		float longitude; // in degrees
		//double horizontalAccuracy;
};



struct GPSReading{
		GPSLocation Location;
		string Status;
};



enum class State {
	IDLE,
	ACTIVE,
	IN_CALL
};



#endif /* MAIN_H_ */

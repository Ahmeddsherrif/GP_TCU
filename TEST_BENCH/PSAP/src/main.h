/*
 * main.h
 *
 *  Created on: Feb 18, 2023
 *      Author: ahmed
 */

#ifndef MAIN_H_
#define MAIN_H_


#include <iostream>
#include <stdint.h>

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

using namespace std;


#include "error.h"
#include "configuration.h"
#include "Utilities.h"
#include "debug.h"


#include "MSDData.h"
#include "BitStreamDecoder.h"
#include "MSDDecoder.h"

#include "Serial.h"
#include "Mobile.h"



enum class State {
	IDLE,
	ACTIVE,
	IN_CALL
};

#define EVENT_DIAL  "DIAL"
#define EVENT_END	"END"
#define EVENT_HANG	"HANG"
#define EVENT_KILL	"KILL"





#endif /* MAIN_H_ */

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


#include "Serial.h"
#include "Mobile.h"
#include "ECALL.h"



enum class State {
	IDLE,
	ACTIVE,
	IN_CALL
};



#endif /* MAIN_H_ */

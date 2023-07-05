/*
 * main.h
 *
 *  Created on: Mar 2, 2023
 *      Author: ahmed
 */

#ifndef MAIN_H_
#define MAIN_H_


#include <iostream>
#include <cmath>
#include <thread>
#include <chrono>
#include <fstream>
#include <string>
#include <cstring>
#include <vector>
#include <queue>
#include <mutex>
#include <ctime>
#include <condition_variable>
#include <iomanip>

using namespace std;
using namespace chrono;

#include "SensorFile.h"
#include "GPSSensor.h"
#include "GyroscopeSensor.h"
#include "SpeedometerSensor.h"

#include "debug.h"
#include "MessageQueue.h"


constexpr double pi = 3.14159265358979323846;
constexpr double earthRadius = 6371000.0; // Earth's radius in meters

#define KMH_TO_MS(speed) 								(speed*1000.0/3600.0)
#define MILLISECONDS_TO_SECONDS(inputMilliseconds)		(inputMilliseconds / 1000)

#define DATA_DIR "/home/pi/Desktop/Graduation-Project/Demo/Simulation-Control/Sensors-Data-Process/Actual Data/Full-Track/"



#endif /* MAIN_H_ */

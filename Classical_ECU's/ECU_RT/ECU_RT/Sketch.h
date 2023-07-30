#ifndef SKETCH_H_
#define SKETCH_H_

#include <stdbool.h>

#include <Arduino.h>
#include <mcp2515.h>

#include "can_frames.h"

#include "FreeRTOS.h"
#include "FreeAct.h"

#include "BSP.h"



void setup();
void loop();

#define NUMBER_OF_BUZZER_TOGGLE_FAST				12
#define BUZZER_ON_TIME_FAST_MS						250
#define BUZZER_OFF_TIME_FAST_MS						250

#define NUMBER_OF_BUZZER_TOGGLE_SLOW				3
#define BUZZER_ON_TIME_SLOW_MS						500
#define BUZZER_OFF_TIME_SLOW_MS						1500



#endif
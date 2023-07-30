#ifndef SKETCH_H_
#define SKETCH_H_

#include <stdbool.h>

#include <Arduino.h>
#include <mcp2515.h>


#include "FreeRTOS.h"
#include "FreeAct.h"

#include "BSP.h"

#include "can_frames.h"

void setup();
void loop();

#define SOS_NUMBER_OF_LED_BLINK				3
#define LED_BLINK_PERIOD_MS					1000


#endif
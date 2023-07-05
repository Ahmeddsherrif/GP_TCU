/*
 * Sketch.h
 *
 * Created: 6/17/2023 6:48:25 AM
 *  Author: Master
 */ 


#ifndef SKETCH_H_
#define SKETCH_H_


/*Begining of Auto generated code by Atmel studio */
#include <Arduino.h>

/*End of auto generated code by Atmel studio */

#include <SPI.h>
#include <mcp2515.h>
#include <LiquidCrystal.h>


#include "BTN.h"
#include "lcd_assert.h"

#include <stdlib.h>

#define SS_PIN							53

#define LCD_ROWS						2
#define	LCD_COLUMNS						16

#define SPEED_REFRESH_TIME_MS			200

#define CAN_MSG_DLC						1

#define CAN_MSG_SIMULATION_CMD_ID				0x09
#define CAN_MSG_SIMULATION_CMD_DATA_KILL		0x01
#define CAN_MSG_SIMULATION_CMD_DATA_START		0x02

#define CAN_MSG_SOS_ID							0x10
#define CAN_MSG_SOS_DATA_ECALL					0x01
#define CAN_MSG_SOS_DATA_BCALL					0x02
#define CAN_MSG_SOS_DATA_REQUEST_BCALL			0x03
#define CAN_MSG_SOS_DATA_END					0x04

#define CAN_MSG_SPEED_ID						0x12


const int rs = 8, en = 9, d4 = 4, d5 = 5, d6 = 6, d7 = 7;


typedef struct can_frame Frame; 

typedef enum{
	STATE_HALT,
	STATE_IDLE,
	STATE_ACTIVE
}State_t;


#define STATE_INIT				STATE_HALT

typedef enum{
	EVENT_IDLE,
	EVENT_START_SIMULATION,
	EVENT_KILL_SIMULATION,
	EVENT_ECALL,
	EVENT_BCALL,
	EVENT_END,
	EVENT_SPEED,
	EVENT_REQ_BCALL
}Event_t;



void can_message_recievied();
void can_message_simulation_cmd_frame_received();
void can_message_sos_frame_recievied();
void can_message_speed_frame_recievied();

void stateIDLE_handler();
void stateACTIVE_handler();
void stateHALT_handler();

void superStateRunning_handler();


void BTN_right_handler();
void BTN_up_handler();
void BTN_down_handler();
void BTN_left_handler();
void BTN_select_handler();


extern uint8_t speed;
extern Frame frameResponse;
extern Frame frameRequest;
extern LiquidCrystal lcd;
extern MCP2515 mcp2515;
extern State_t mainState;
extern Event_t mainEvent;
extern bool stateEntry, stateExit;
extern volatile unsigned long int stateEntryIntialTime;


#endif /* SKETCH_H_ */
/*
 * Sketch.h
 *
 * Created: 6/17/2023 6:48:25 AM
 *  Author: Master
 */ 


#ifndef SKETCH_H_
#define SKETCH_H_


/*Beginning of Auto generated code by Atmel studio */
#include <Arduino.h>

/*End of auto generated code by Atmel studio */

#include <SPI.h>
#include <mcp2515.h>

#define PIN_SS									53
#define PIN_BUZZER								2
#define PIN_LED_RED								3
#define PIN_LED_GREEN							4

#define CAN_MSG_DLC								1

#define CAN_MSG_SIMULATION_CMD_ID				0x09
#define CAN_MSG_SIMULATION_CMD_DATA_KILL		0x01
#define CAN_MSG_SIMULATION_CMD_DATA_START		0x02

#define CAN_MSG_DMS_ID							0x11
#define CAN_MSG_DMS_DATA_DISTRACTED				0x01
#define CAN_MSG_DMS_DATA_SLEEP					0x02

#define CAN_MSG_SPEED_ID						0x12


typedef struct can_frame Frame; 

typedef enum{
	STATE_HALT,
	STATE_IDLE,
	STATE_ACTIVE
}State_t;


#define STATE_INIT								STATE_HALT

typedef enum{
	EVENT_IDLE,
	EVENT_START_SIMULATION,
	EVENT_KILL_SIMULATION,
	EVENT_SLEEP,
	EVENT_DISTRACTED,
	EVENT_END_ALERT
}Event_t;



void can_message_recievied();
void can_message_simulation_cmd_frame_received();
void can_message_dms_frame_recievied();

void stateIDLE_handler();
void stateACTIVE_handler();
void stateHALT_handler();

void superStateRunning_handler();


extern Frame frameResponse;
extern Frame frameRequest;
extern MCP2515 mcp2515;
extern State_t mainState;
extern Event_t mainEvent;
extern bool stateEntry, stateExit;
extern volatile unsigned long int stateEntryIntialTime;


#define BUZZER_ON_TIME_MS			3000

#endif /* SKETCH_H_ */
#include "Sketch.h"

MCP2515 mcp2515(PIN_SS);


Frame frameResponse;
Frame frameRequest;

State_t mainState;
Event_t mainEvent;
volatile unsigned long int stateEntryIntialTime;
bool stateEntry, stateExit;


void setup() {
	mainState = STATE_INIT;
	mainEvent = EVENT_IDLE;
	
	stateEntry = true;
	stateExit = false;

	
	mcp2515.reset();
	mcp2515.setBitrate(CAN_125KBPS, MCP_8MHZ);
	mcp2515.setNormalMode();
	
	pinMode(PIN_BUZZER, OUTPUT);
	pinMode(PIN_LED_RED, OUTPUT);
	pinMode(PIN_LED_GREEN, OUTPUT);
	
	digitalWrite(PIN_BUZZER, HIGH);
	digitalWrite(PIN_LED_RED, HIGH);
	digitalWrite(PIN_LED_GREEN, HIGH);
	
	delay(2000);
	
	digitalWrite(PIN_BUZZER, LOW);
	digitalWrite(PIN_LED_RED, LOW);
	digitalWrite(PIN_LED_GREEN, LOW);
	

}

void loop() {
	
	
	//	Get Events

	if(mcp2515.readMessage(&frameResponse) == MCP2515::ERROR_OK){
		can_message_recievied();
	}
	
	// Handle Events
	switch(mainState){
		
		case STATE_HALT:{
			stateHALT_handler();
			break;
		}
		
		case STATE_IDLE:{
			stateIDLE_handler();
			break;
		}
		
		case STATE_ACTIVE:{
			stateACTIVE_handler();
			break;
		}
		default:{
			
		}
	}
	
	delay(10);
}
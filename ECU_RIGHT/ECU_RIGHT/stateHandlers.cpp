/*
 * stateHandlers.cpp
 *
 * Created: 6/17/2023 8:26:33 PM
 *  Author: Master
 */ 

#include "Sketch.h"

//
//void superStateRunning_handler(){
	// TODO: maybe one day i will do it right with C OOP and QPC HSM patterns, but now its GOOD ENOUGH
//}

void stateHALT_handler(){
	if(stateEntry == true){
		stateEntry = false;
		
		digitalWrite(PIN_LED_RED, HIGH);
		digitalWrite(PIN_BUZZER, LOW);
		digitalWrite(PIN_LED_GREEN, LOW);
	}
	
	switch(mainEvent){
		case EVENT_IDLE:{
			
			break;
		}
		
		case EVENT_START_SIMULATION:{
			mainEvent = EVENT_IDLE;
			
			mainState = STATE_IDLE;
			stateExit = true;
			
			break;
		}
		
		default:{
			mainEvent = EVENT_IDLE;
			break;
		}
	}
	
	if(stateExit == true){
		stateExit = false;
		stateEntry = true;
		
		
		digitalWrite(PIN_LED_RED, LOW);
	}
}

void stateIDLE_handler(){
	if(stateEntry == true){
		stateEntry = false;
		
		
		digitalWrite(PIN_LED_GREEN, HIGH);
	}
	

	switch(mainEvent){
		case EVENT_IDLE:{
			
			break;
		}
		
		case EVENT_SLEEP:
		case EVENT_DISTRACTED:{
			mainEvent = EVENT_IDLE;
			
			if(isDmsActive == true){
				mainState = STATE_ACTIVE;
				stateExit = true;
			}
	
			break;
		}
		
		case EVENT_BUTTON_PRESSED:{
			isDmsActive = true;
			digitalWrite(PIN_LED_YELLOW, HIGH);
			break;
		}
		
		case EVENT_BUTTON_NOT_PRESSED:{
			isDmsActive = false;
			digitalWrite(PIN_LED_YELLOW, LOW);
			break;
		}
		
		case EVENT_KILL_SIMULATION:{
			mainEvent = EVENT_IDLE;
			
			mainState = STATE_HALT;
			stateExit = true;
			
			break;	
		}
			
		default:{
			mainEvent = EVENT_IDLE;
			
			break;
		}
	}
	
	
	if(stateExit == true){
		stateExit = false;
		stateEntry = true;
		
		digitalWrite(PIN_LED_GREEN, LOW);
	}
	
}


void stateACTIVE_handler(){
	if(stateEntry == true){
		stateEntry = false;
		
		stateEntryIntialTime = millis();
		digitalWrite(PIN_BUZZER, HIGH);
	}
	
	
	if(millis() - stateEntryIntialTime >= BUZZER_ON_TIME_MS){
		mainEvent = EVENT_END_ALERT;
	}
	
	switch(mainEvent){
		
		case EVENT_END_ALERT:{
			mainEvent = EVENT_IDLE;
			
			mainState = STATE_IDLE;
			stateExit = true;
			break;
		}
		
		case EVENT_KILL_SIMULATION:{
			mainEvent = EVENT_IDLE;
			
			mainState = STATE_HALT;
			stateExit = true;
			
			break;
		}
		
		case EVENT_BUTTON_PRESSED:{
			isDmsActive = true;
			digitalWrite(PIN_LED_YELLOW, HIGH);
			
			mainEvent = EVENT_END_ALERT;					// needs to be inside an event queue
			
			break;
		}
		
		case EVENT_BUTTON_NOT_PRESSED:{
			isDmsActive = false;
			digitalWrite(PIN_LED_YELLOW, LOW);
			break;
		}

		default:{
			mainEvent = EVENT_IDLE;
			
			break;
		}
	}
	
	if(stateExit == true){
		stateExit = false;
		stateEntry = true;
		
		digitalWrite(PIN_BUZZER, LOW);
	}
}
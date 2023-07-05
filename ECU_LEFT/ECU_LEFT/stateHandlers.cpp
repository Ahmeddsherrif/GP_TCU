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
		
		lcd.clear();
		lcd.setCursor(0,0);
		lcd.print("WAITING ...");

		lcd.setCursor(0,1);
		
		speed = 0;
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
		
	}
}

void stateIDLE_handler(){
	if(stateEntry == true){
		stateEntry = false;
		
		lcd.clear();
		lcd.setCursor(0,1);
		lcd.print("STATE     : IDLE");
		stateEntryIntialTime = millis();
	}
	
	
	if((millis() - stateEntryIntialTime) % SPEED_REFRESH_TIME_MS == 0){
		stateEntryIntialTime = millis();
		lcd.setCursor(0,0);
		lcd.print("SPEED(KMH): ");
		lcd.print(speed);
	}

	switch(mainEvent){
		case EVENT_IDLE:{
			
			break;
		}
		
		case EVENT_ECALL:{
			mainEvent = EVENT_IDLE;
			
			lcd.clear();
			lcd.setCursor(0,0);
			lcd.print("SOS TRIG: AUTO");
			
			mainState = STATE_ACTIVE;
			stateExit = true;
			
			break;
		}
		
		
		
		case EVENT_SPEED:{
			speed = frameResponse.data[0];
			break;
		}

		case EVENT_BCALL:{
			mainEvent = EVENT_IDLE;
			
			lcd.clear();
			lcd.setCursor(0,0);
			lcd.print("SOS TRIG: MANUAL");
				
			mainState = STATE_ACTIVE;
			stateExit = true;
			
			break;
		}
		
		case EVENT_REQ_BCALL:{
			mainEvent = EVENT_IDLE;
			
			frameRequest.can_id = CAN_MSG_SOS_ID;
			frameRequest.can_dlc = CAN_MSG_DLC;
			frameRequest.data[0] = CAN_MSG_SOS_DATA_REQUEST_BCALL;
			
			mcp2515.sendMessage(&frameRequest);
			
			Serial.println("OVER HERE.");
			Serial.println("OVER HERE..")	;
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
		
	}
	
}


void stateACTIVE_handler(){
	if(stateEntry == true){
		stateEntry = false;
		
		lcd.setCursor(0,1);
		lcd.print("STATE   : ACTIVE");
		
	}
	
	
	switch(mainEvent){
		case EVENT_IDLE:{
			
			break;
		}
		
		case EVENT_END:{
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

		default:{
			mainEvent = EVENT_IDLE;
			
			break;
		}
	}
	

	if(stateExit == true){
		stateExit = false;
		stateEntry = true;
		
	}
}
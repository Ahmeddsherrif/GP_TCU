#include "Sketch.h"


LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
MCP2515 mcp2515(SS_PIN);


uint8_t speed;

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

	lcd.begin(LCD_COLUMNS, LCD_ROWS);
	lcd.clear();
	
	mcp2515.reset();
	mcp2515.setBitrate(CAN_125KBPS, MCP_8MHZ);
	mcp2515.setNormalMode();
	
	lcd.setCursor(0,0);
	lcd.print("< GRAD PROJECT >");
	lcd.setCursor(0,1);
	lcd.print("HELWAN ECE  2023");
	delay(2000);
	lcd.clear();
	
	Serial.begin(9600);

}

void loop() {
	
	
	//	Get Events
	getBTN();
	
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
	
}




void BTN_left_handler(){
	mainEvent = EVENT_START_SIMULATION;
}

void BTN_up_handler(){
	mainEvent = EVENT_BCALL;
}

void BTN_down_handler(){
	
}

void BTN_right_handler(){
	
}

void BTN_select_handler(){
	mainEvent = EVENT_REQ_BCALL;
}

/*
 * BTN.cpp
 *
 * Created: 6/17/2023 2:50:40 PM
 *  Author: Master
 */ 


/*
 * BTN.c
 *
 * Created: 6/17/2023 6:42:24 AM
 *  Author: Master
 */ 

#include <Arduino.h>

#include "BTN.h"
#include "lcd_assert.h"

#define BTN_ADC_CHANNEL							0
#define BTN_ADC_READING_MARGIN					100
#define NUMBER_OF_CONSECUTIVE_SAMPLES			10
#define BTN_DEFAULT_ADC_VALUE					1023


#define NUMBER_OF_BUTTONS						5

#define BTN_RIGHT								0
#define BTN_UP									1
#define BTN_DOWN								2
#define BTN_LEFT								3
#define BTN_SELECT								4


#define BTN_RIGHT_ADC_VALUE						0
#define BTN_UP_ADC_VALUE						120
#define BTN_DOWN_ADC_VALUE						256
#define BTN_LEFT_ADC_VALUE						410
#define BTN_SELECT_ADC_VALUE					640






#define BTN_UP_ADC_VALUE_MAX			(BTN_UP_ADC_VALUE + BTN_ADC_READING_MARGIN)
#define BTN_UP_ADC_VALUE_MIN			(BTN_UP_ADC_VALUE - BTN_ADC_READING_MARGIN)

#define BTN_DOWN_ADC_VALUE_MAX			(BTN_DOWN_ADC_VALUE + BTN_ADC_READING_MARGIN)
#define BTN_DOWN_ADC_VALUE_MIN			(BTN_DOWN_ADC_VALUE - BTN_ADC_READING_MARGIN)

#define BTN_LEFT_ADC_VALUE_MAX			(BTN_LEFT_ADC_VALUE + BTN_ADC_READING_MARGIN)
#define BTN_LEFT_ADC_VALUE_MIN			(BTN_LEFT_ADC_VALUE - BTN_ADC_READING_MARGIN)

#define BTN_SELECT_ADC_VALUE_MAX		(BTN_SELECT_ADC_VALUE + BTN_ADC_READING_MARGIN)
#define BTN_SELECT_ADC_VALUE_MIN		(BTN_SELECT_ADC_VALUE - BTN_ADC_READING_MARGIN)


static void (*ptrFunc[NUMBER_OF_BUTTONS])(void) = {
	BTN_RIGHT_HANDLER,
	BTN_UP_HANDLER,
	BTN_DOWN_HANDLER,
	BTN_LEFT_HANDLER,
	BTN_SELECT_HANDLER
};


void getBTN(){
	
	int analogReading = BTN_DEFAULT_ADC_VALUE;
	int first_sample, i;
	
	first_sample = analogRead(BTN_ADC_CHANNEL);
	

	
	for(i=0; i<NUMBER_OF_CONSECUTIVE_SAMPLES; i++){
		if(analogRead(BTN_ADC_CHANNEL) != first_sample){
			break;
		}
	}
	
	if(i==NUMBER_OF_CONSECUTIVE_SAMPLES){
		analogReading = first_sample;
	}
	
	if(analogReading == BTN_RIGHT_ADC_VALUE){
		ptrFunc[BTN_RIGHT]();
	}
	else if(analogReading <= BTN_UP_ADC_VALUE_MAX && analogReading >= BTN_UP_ADC_VALUE_MIN){
		ptrFunc[BTN_UP]();
	}
	else if(analogReading <= BTN_DOWN_ADC_VALUE_MAX && analogReading >= BTN_DOWN_ADC_VALUE_MIN){
		ptrFunc[BTN_DOWN]();
	}
	else if(analogReading <= BTN_SELECT_ADC_VALUE_MAX && analogReading >= BTN_SELECT_ADC_VALUE_MIN){
		ptrFunc[BTN_SELECT]();
	}
	
	else if(analogReading <= BTN_LEFT_ADC_VALUE_MAX && analogReading >= BTN_LEFT_ADC_VALUE_MIN){
		ptrFunc[BTN_LEFT]();
	}
	else{
		/* return error */
	}
}
/*
 * lcd_assert.cpp
 *
 * Created: 6/17/2023 7:04:43 AM
 *  Author: Master
 */ 


#include <Arduino.h>
#include <LiquidCrystal.h>

#include "lcd_assert.h"

extern LiquidCrystal lcd;

void (*ptrNULL_resetVect)(void) = 0;

void deadEnd(){
	noInterrupts();
	
	
	
	lcd.clear();
	lcd.setCursor(0,0);
	lcd.print("FATAL ERROR");
	lcd.setCursor(0,1);
	lcd.print("RESET IN ");
	
	
	for(int c=5; c>=0; c--){
		lcd.setCursor(10,1);
		lcd.print(c);
		delay(1000);	
	}
	
	ptrNULL_resetVect();
	
	
	while(1);
}

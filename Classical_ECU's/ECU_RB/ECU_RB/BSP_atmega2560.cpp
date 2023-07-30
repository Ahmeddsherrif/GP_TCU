#include "Sketch.h"


#define SS_PIN							49
#define PIN_SWITCH						48
#define PIN_BUZZER						45
#define PIN_LED_RED						44
#define PIN_LED_YELLOW					42
#define PIN_LED_GREEN					41


static MCP2515 mcp2515(SS_PIN);

void BSP_init(void){
	pinMode(PIN_LED_GREEN, OUTPUT);
	pinMode(PIN_LED_RED, OUTPUT);
	pinMode(PIN_LED_YELLOW, OUTPUT);
	pinMode(PIN_BUZZER, OUTPUT);
	pinMode(PIN_SWITCH, INPUT_PULLUP);
	
	mcp2515.reset();
	mcp2515.setBitrate(CAN_125KBPS, MCP_8MHZ);
	mcp2515.setNormalMode();
}

void BSP_start(void){
	
}

void BSP_led_red_on(void){
	digitalWrite(PIN_LED_RED, HIGH);
}

void BSP_led_red_off(void){
	digitalWrite(PIN_LED_RED, LOW);
}

void BSP_led_yellow_on(void){
	digitalWrite(PIN_LED_YELLOW, HIGH);
}

void BSP_led_yellow_off(void){
	digitalWrite(PIN_LED_YELLOW, LOW);
}


void BSP_led_green_on(void){
	digitalWrite(PIN_LED_GREEN, HIGH);
}

void BSP_led_green_off(void){
	digitalWrite(PIN_LED_GREEN, LOW);
}

void BSP_buzzer_on(void){
	digitalWrite(PIN_BUZZER, HIGH);
}

void BSP_buzzer_off(void){
	digitalWrite(PIN_BUZZER, LOW);
}


int BSP_get_switch(void){
	return digitalRead(PIN_SWITCH);
}

void BSP_send_can_frame(Can_frame_t * frame){
	mcp2515.sendMessage(frame);
}

int BSP_get_can_frame(Can_frame_t *frame){
	int rtnValue = 0;
	if (mcp2515.readMessage(frame) == MCP2515::ERROR_OK){
		rtnValue = 1;
	}
	return rtnValue;
}




typedef enum{
	STATE_SWITCH_INIT,
	STATE_SWITCH_PRESSED,
	STATE_SWITCH_NOT_PRESSED	
}State_switch_t;



static State_switch_t state_switch = STATE_SWITCH_INIT;


static void state_switch_pressed_handler(const int * reading, BaseType_t * xHigherPriorityTaskWoken){
	if(*reading == SWITCH_NOT_PRESSED){
		static Event const switchRelasedEvt = {SWITCH_RELEASED_SIG};
		Active_postFromISR(AO_blinkyButton, &switchRelasedEvt, xHigherPriorityTaskWoken);
		state_switch = STATE_SWITCH_NOT_PRESSED;
	}
}
static void state_switch_not_pressed_handler(const int * reading, BaseType_t * xHigherPriorityTaskWoken){
	if(*reading == SWITCH_PRESSED){
		static Event const switchPressedEvt = {SWITCH_PRESSED_SIG};
		Active_postFromISR(AO_blinkyButton, &switchPressedEvt, xHigherPriorityTaskWoken);
		state_switch = STATE_SWITCH_PRESSED;
	}
}

void vApplicationTickHook(void) {
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	int current = BSP_get_switch();
	
	switch(state_switch){
		case STATE_SWITCH_INIT:{
			state_switch_pressed_handler(&current, &xHigherPriorityTaskWoken);	
			state_switch_not_pressed_handler(&current, &xHigherPriorityTaskWoken);
			break;
		}
		case STATE_SWITCH_PRESSED:{
			state_switch_pressed_handler(&current, &xHigherPriorityTaskWoken);	
			break;
		}	
		case STATE_SWITCH_NOT_PRESSED:{					
			state_switch_not_pressed_handler(&current, &xHigherPriorityTaskWoken);
			break;
		}
	}
	
	CanFrameEvent_fromISR(&xHigherPriorityTaskWoken);

}

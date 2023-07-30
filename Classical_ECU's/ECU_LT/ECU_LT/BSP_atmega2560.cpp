#include "Sketch.h"


#define SS_PIN							49
#define PIN_BUTTON						9
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
	pinMode(PIN_BUTTON, INPUT_PULLUP);
	
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


int BSP_get_button(void){
	return digitalRead(PIN_BUTTON);
}

void BSP_send_can_frame(Can_frame_t *frame){
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
	STATE_BUTTON_INIT,
	STATE_BUTTON_PRESSED,
	STATE_BUTTON_NOT_PRESSED	
}State_button_t;

#define BUTTON_PRESSED		0
#define BUTTON_NOT_PRESSED	1

static State_button_t state_button = STATE_BUTTON_INIT;


static void state_button_pressed_handler(const int * reading, BaseType_t * xHigherPriorityTaskWoken){
	if(*reading == BUTTON_NOT_PRESSED){
		static Event const switchRelasedEvt = {BUTTON_RELEASED_SIG};
		Active_postFromISR(AO_blinkyButton, &switchRelasedEvt, xHigherPriorityTaskWoken);
		state_button = STATE_BUTTON_NOT_PRESSED;
	}
}
static void state_button_not_pressed_handler(const int * reading, BaseType_t * xHigherPriorityTaskWoken){
	if(*reading == BUTTON_PRESSED){
		static Event const switchPressedEvt = {BUTTON_PRESSED_SIG};
		Active_postFromISR(AO_blinkyButton, &switchPressedEvt, xHigherPriorityTaskWoken);
		state_button = STATE_BUTTON_PRESSED;
	}
}

void vApplicationTickHook(void) {
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	int current = BSP_get_button();
	
	switch(state_button){
		case STATE_BUTTON_INIT:{
			state_button_pressed_handler(&current, &xHigherPriorityTaskWoken);	
			state_button_not_pressed_handler(&current, &xHigherPriorityTaskWoken);
			break;
		}
		case STATE_BUTTON_PRESSED:{
			state_button_pressed_handler(&current, &xHigherPriorityTaskWoken);	
			break;
		}	
		case STATE_BUTTON_NOT_PRESSED:{					
			state_button_not_pressed_handler(&current, &xHigherPriorityTaskWoken);
			break;
		}
	}
}

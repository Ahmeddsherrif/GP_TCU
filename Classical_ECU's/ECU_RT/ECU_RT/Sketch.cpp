#include "Sketch.h"






/* The BlinkyButton AO ===========================================================*/
typedef struct {
	Active super; /* inherit Active base class */
	/* add private data for the AO... */
	TimeEvent te;
	CanFrameEvent cfe;
	
	bool isBuzzerOn;
	bool isBuzzerEnabled;
	
	int counter;
	
	int buzzer_on_time_ms;
	int buzzer_off_time_ms;
	int number_of_buzzer_toggle;
} BlinkyButton;

/* Function Prototype ============================================================*/
void BlinkyButton_ctor(BlinkyButton * const me);


static void BlinkyButton_dispatch(BlinkyButton * const me, Event const * const e) {
	switch (e->sig) {
		case INIT_SIG:{
			BSP_led_green_on();
			break;
		}
		
		case SWITCH_PRESSED_SIG:{
			me->isBuzzerEnabled = true;
			BSP_led_yellow_on();
			break;
		}
		
		case SWITCH_RELEASED_SIG: {
			me->isBuzzerEnabled = false;
			BSP_led_yellow_off();
			break;
		}
		
		case CAN_FRAME_RECIVED_SIG: {
			
			if(me->cfe.frame.can_id == CAN_ALARM_ID && me->cfe.frame.can_dlc == CAN_ALARM_DLC){
				BSP_led_green_off();	
				BSP_led_red_on();					
				me->counter = 0;
				me->isBuzzerOn = false;
				
				if(me->cfe.frame.data[0] == CAN_ALARM_MESSAGE_FAST){
					me->buzzer_on_time_ms = BUZZER_ON_TIME_FAST_MS;
					me->buzzer_off_time_ms = BUZZER_OFF_TIME_FAST_MS;
					me->number_of_buzzer_toggle = NUMBER_OF_BUZZER_TOGGLE_FAST;	
					static Event const initialBuzzerEvt = {INTIAL_BUZZER_SIG};
					Active_post(&me->super, &initialBuzzerEvt);
				}
				else if(me->cfe.frame.data[0] == CAN_ALARM_MESSAGE_SLOW){
					me->buzzer_on_time_ms = BUZZER_ON_TIME_SLOW_MS;
					me->buzzer_off_time_ms = BUZZER_OFF_TIME_SLOW_MS;
					me->number_of_buzzer_toggle = NUMBER_OF_BUZZER_TOGGLE_SLOW;
					static Event const initialBuzzerEvt = {INTIAL_BUZZER_SIG};
					Active_post(&me->super, &initialBuzzerEvt);
				}
				else if(me->cfe.frame.data[0] == CAN_ALARM_MESSAGE_STOP){
					static Event const stopBuzzerEvt = {STOP_BUZZER_SIG};
					Active_post(&me->super, &stopBuzzerEvt);
				}
			}
			
			break;
			
		}
		
		case INTIAL_BUZZER_SIG:
		case TIMEOUT_SIG: {

			if (me->isBuzzerOn == false) { /*Buzzer not on */
				
				// Break Condition
				(me->counter)++;
				if(me->counter > me->number_of_buzzer_toggle){
					static Event const stopBuzzerEvt = {STOP_BUZZER_SIG};
					Active_post(&me->super, &stopBuzzerEvt);
				}
				
				else{
					if(me->isBuzzerEnabled == true){
						BSP_buzzer_on();
					}
					me->isBuzzerOn = true;
					TimeEvent_arm(&me->te, me->buzzer_on_time_ms);
				}
			}
			else {  /* buzzer is on */
				BSP_buzzer_off();
				me->isBuzzerOn = false;
				TimeEvent_arm(&me->te, me->buzzer_off_time_ms);
			}
			
			break;
		}
		
		case STOP_BUZZER_SIG:{
			TimeEvent_disarm(&me->te);
			BSP_led_green_on();
			BSP_led_red_off();
			BSP_buzzer_off();
			me->isBuzzerOn = false;
			break;
		}
				
		
		default: {
			break;
		}
		

	}
}
void BlinkyButton_ctor(BlinkyButton * const me) {
	Active_ctor(&me->super, (DispatchHandler)&BlinkyButton_dispatch);
	me->te.type = TYPE_ONE_SHOT;
	TimeEvent_ctor(&me->te, TIMEOUT_SIG, &me->super);
	me->isBuzzerOn = false;
	me->isBuzzerEnabled = false;
	me->counter = 0;
	CanFrameEvent_ctor(&me->cfe, CAN_FRAME_RECIVED_SIG, &me->super);
}
static StackType_t blinkyButton_stack[configMINIMAL_STACK_SIZE]; /* task stack */
static Event *blinkyButton_queue[10];
static BlinkyButton blinkyButton;
Active *AO_blinkyButton = &blinkyButton.super;

void setup() {
	BSP_init();
	
	   /* create and start the BlinkyButton AO */
	   BlinkyButton_ctor(&blinkyButton);
	   Active_start(AO_blinkyButton,
					1U,
					blinkyButton_queue,
					sizeof(blinkyButton_queue)/sizeof(blinkyButton_queue[0]),
					blinkyButton_stack,
					sizeof(blinkyButton_stack),
					0U);
	   
	vTaskStartScheduler();
}

void loop() {

}
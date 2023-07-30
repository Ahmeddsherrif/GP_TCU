#include "Sketch.h"


/* The BlinkyButton AO ===========================================================*/
typedef struct {
	Active super; /* inherit Active base class */
	/* add private data for the AO... */
	TimeEvent te;
	bool isLedOn;
	int counter;
} BlinkyButton;

/* Function Prototype ============================================================*/
void BlinkyButton_ctor(BlinkyButton * const me);


static void BlinkyButton_dispatch(BlinkyButton * const me, Event const * const e) {
	switch (e->sig) {
		case INIT_SIG:{
			BSP_led_green_on();
			break;
		}
		
		case BUTTON_PRESSED_SIG:{
			BSP_led_green_off();
				
			BSP_send_can_frame(&frame_sos_triggered);
			
			me->counter = 0;
			TimeEvent_arm(&me->te, LED_BLINK_PERIOD_MS);
			BSP_led_red_on();
			me->isLedOn = true;

			break;
		}
			
		case TIMEOUT_SIG: {
			if (!me->isLedOn) { /* LED not on */
				(me->counter)++;
				if(me->counter >= SOS_NUMBER_OF_LED_BLINK){
					TimeEvent_disarm(&me->te);
					BSP_led_green_on();
					BSP_led_red_off();
				}else{
					BSP_led_red_on();
					me->isLedOn = true;
					TimeEvent_arm(&me->te, LED_BLINK_PERIOD_MS);
				}
			}
			else {  /* LED is on */
				BSP_led_red_off();
				me->isLedOn = false;
				TimeEvent_arm(&me->te, LED_BLINK_PERIOD_MS);
			}
			
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
	me->isLedOn = false;
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
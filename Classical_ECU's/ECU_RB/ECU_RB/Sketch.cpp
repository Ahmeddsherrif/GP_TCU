#include "Sketch.h"






/* The BlinkyButton AO ===========================================================*/
typedef struct {
	Active super; /* inherit Active base class */
	/* add private data for the AO... */
	TimeEvent te;
	TimeEvent te2;
	CanFrameEvent cfe;
	
	bool isLedOn;
} BlinkyButton;

/* Function Prototype ============================================================*/
void BlinkyButton_ctor(BlinkyButton * const me);


static void BlinkyButton_dispatch(BlinkyButton * const me, Event const * const e) {
	switch (e->sig) {
		case INIT_SIG:{
			break;
		}
		
		case SWITCH_PRESSED_SIG:{
			BSP_led_red_off();
			TimeEvent_disarm(&me->te);
			me->isLedOn = false;
			
			BSP_led_green_on();
			BSP_send_can_frame(&frame_right_seatbelt_hoocked);
			break;
		}
		
		
		case SWITCH_RELEASED_SIG: {
			BSP_send_can_frame(&frame_right_seatbelt_unhoocked);
			BSP_led_green_off();
			
			static Event const initLedRedBlinkEvt = {INIT_LED_RED_BLINK_SIG};
			Active_post(&me->super, &initLedRedBlinkEvt);
			break;
		}
		
		
		case INIT_LED_RED_BLINK_SIG:
		case TIMEOUT_LED_RED_SIG: {
			if (!me->isLedOn) { /* LED not on */
				BSP_led_red_on();
				me->isLedOn = true;
				TimeEvent_arm(&me->te, 1000);
			}
			else {  /* LED is on */
				BSP_led_red_off();
				me->isLedOn = false;
				TimeEvent_arm(&me->te, 1000);
			}
			break;
		}
		
		
		
		case TIMEOUT_LED_YELLOW_SIG: {
			BSP_led_yellow_off();
			break;
		}
		
		case CAN_FRAME_RECIVED_SIG:{
			if(areFramesEqual(&me->cfe.frame, &frame_request_seatbelt) == true){
				BSP_led_yellow_on();
				
				if(BSP_get_switch() == SWITCH_PRESSED){
					BSP_send_can_frame(&frame_right_seatbelt_hoocked);
				} else {
					BSP_send_can_frame(&frame_right_seatbelt_unhoocked);
				}
				
				TimeEvent_arm(&me->te2, 1000);
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
	TimeEvent_ctor(&me->te, TIMEOUT_LED_RED_SIG, &me->super);
	TimeEvent_ctor(&me->te2, TIMEOUT_LED_YELLOW_SIG, &me->super);
	me->isLedOn = false;
	
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
/*
 * BSP.h
 *
 * Created: 7/29/2023 2:09:05 AM
 *  Author: Master
 */ 


#ifndef BSP_H_
#define BSP_H_

enum Signals {
	TIMEOUT_SIG = USER_SIG,
	BUTTON_PRESSED_SIG,
	BUTTON_RELEASED_SIG,
};

extern Active *AO_blinkyButton;
typedef struct can_frame Can_frame_t;

void BSP_init(void);
void BSP_start(void);
void BSP_led_red_on(void);
void BSP_led_red_off(void);
void BSP_led_yellow_on(void);
void BSP_led_yellow_off(void);
void BSP_led_green_on(void);
void BSP_led_green_off(void);
void BSP_buzzer_on(void);
void BSP_buzzer_off(void);

int BSP_get_button(void);

void BSP_send_can_frame(Can_frame_t *frame);
int BSP_get_can_frame(Can_frame_t *frame);

#endif /* BSP_H_ */
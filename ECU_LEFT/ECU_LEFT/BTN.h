/*
 * BTN.h
 *
 * Created: 6/17/2023 6:39:56 AM
 *  Author: Master
 */ 


#ifndef BTN_H_
#define BTN_H_


/********************************************************************************************/
/* Edit these handler with the corresponding functions
/********************************************************************************************/
#define	BTN_RIGHT_HANDLER		BTN_right_handler	
#define	BTN_UP_HANDLER			BTN_up_handler
#define	BTN_DOWN_HANDLER		BTN_down_handler
#define	BTN_LEFT_HANDLER		BTN_left_handler
#define BTN_SELECT_HANDLER		BTN_select_handler


extern void BTN_right_handler();
extern void BTN_up_handler();
extern void BTN_down_handler();
extern void BTN_left_handler();
extern void BTN_select_handler();

/********************************************************************************************/




void getBTN();

#endif /* BTN_H_ */
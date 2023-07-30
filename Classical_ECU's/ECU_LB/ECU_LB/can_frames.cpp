/*
 * can_frames.h
 *
 * Created: 7/29/2023 11:30:38 AM
 *  Author: Master
 */ 


#ifndef CAN_FRAMES_H_
#define CAN_FRAMES_H_

#include <mcp2515.h>

#define CAN_SEAT_BELT_ID							0x13
#define CAN_SEAT_BELT_DLC							1
#define CAN_LEFT_SEAT_BELT_HOOKED					0x1
#define CAN_LEFT_SEAT_BELT_UNHOOKED					0x2
#define CAN_REQUEST_SEAT_BELT						0x5

can_frame frame_left_seatbelt_hoocked = {
	CAN_SEAT_BELT_ID,
	CAN_SEAT_BELT_DLC,
	{
		CAN_LEFT_SEAT_BELT_HOOKED
	}
};

can_frame frame_left_seatbelt_unhoocked = {
	CAN_SEAT_BELT_ID,
	CAN_SEAT_BELT_DLC,
	{
		CAN_LEFT_SEAT_BELT_UNHOOKED
	}
};

can_frame frame_request_seatbelt = {
	CAN_SEAT_BELT_ID,
	CAN_SEAT_BELT_DLC,
	{
		CAN_REQUEST_SEAT_BELT
	}
};


int areFramesEqual(can_frame * const frame1, can_frame * const frame2){
	int rtnValue = false;
	if( frame1->can_id == frame2->can_id &&
	frame1->can_dlc == frame2->can_dlc)
	{
		int i;
		for(i=0; i< frame1->can_dlc; i++){
			if(frame1->data[i] != frame2->data[i]){
				break;
			}
		}
		
		if(i == frame1->can_dlc){
			rtnValue = true;
		}
	}
	
	return rtnValue;
}




#endif /* CAN_FRAMES_H_ */
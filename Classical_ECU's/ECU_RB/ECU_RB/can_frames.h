/*
 * can_frames.h
 *
 * Created: 7/29/2023 11:30:25 AM
 *  Author: Master
 */ 


#ifndef CAN_FRAMES_H_
#define CAN_FRAMES_H_



extern can_frame frame_right_seatbelt_hoocked;
extern can_frame frame_right_seatbelt_unhoocked;
extern can_frame frame_request_seatbelt;

int areFramesEqual(can_frame * const frame1, can_frame * const frame2);


#endif /* CAN_FRAMES_H_ */
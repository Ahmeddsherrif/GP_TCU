/*
 * can_frames.h
 *
 * Created: 7/29/2023 11:30:25 AM
 *  Author: Master
 */ 


#ifndef CAN_FRAMES_H_
#define CAN_FRAMES_H_

#define CAN_ALARM_ID					0x11
#define CAN_ALARM_DLC					1
#define CAN_ALARM_MESSAGE_FAST			0x1
#define CAN_ALARM_MESSAGE_SLOW			0x2
#define CAN_ALARM_MESSAGE_STOP			0x3

extern can_frame frame_alarm_triggered_fast;
extern can_frame frame_alarm_triggered_slow;

int areFramesEqual(can_frame * const frame1, can_frame * const frame2);

#endif /* CAN_FRAMES_H_ */
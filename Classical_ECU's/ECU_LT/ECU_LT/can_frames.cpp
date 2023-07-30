/*
 * can_frames.h
 *
 * Created: 7/29/2023 11:30:38 AM
 *  Author: Master
 */ 


#ifndef CAN_FRAMES_H_
#define CAN_FRAMES_H_

#include "Sketch.h"

#define CAN_SOS_ID						0x09
#define CAN_SOS_DLC						1
#define CAN_BCALL_MESSAGE				0x02


can_frame frame_sos_triggered = {
	CAN_SOS_ID,
	CAN_SOS_DLC,
	{
		CAN_BCALL_MESSAGE
	}
};


#endif /* CAN_FRAMES_H_ */
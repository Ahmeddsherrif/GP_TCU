#include "Sketch.h"

void can_message_recievied(){
	switch(frameResponse.can_id){
		case CAN_MSG_SIMULATION_CMD_ID:{
			can_message_simulation_cmd_frame_received();
			break;
		}
			
		case CAN_MSG_SOS_ID:{
			can_message_sos_frame_recievied();
			break;
		}
			
		case CAN_MSG_SPEED_ID:{
			can_message_speed_frame_recievied();
			break;
		}
	}
}
void can_message_simulation_cmd_frame_received(){
	switch(frameResponse.data[0]){
		case CAN_MSG_SIMULATION_CMD_DATA_KILL:{
			mainEvent = EVENT_KILL_SIMULATION;
			break;
		}
		case CAN_MSG_SIMULATION_CMD_DATA_START:{
			mainEvent = EVENT_START_SIMULATION;
			break;
		}

		default:{
				
		}
	}
}

void can_message_sos_frame_recievied(){
	switch(frameResponse.data[0]){
		case CAN_MSG_SOS_DATA_ECALL:{
			mainEvent = EVENT_ECALL;
			break;
		}
		case CAN_MSG_SOS_DATA_BCALL:{
			mainEvent = EVENT_BCALL;
			break;
		}
		case CAN_MSG_SOS_DATA_END:{
			mainEvent = EVENT_END;
			break;
		}
		default:{
				
			break;
		}
	}
}
void can_message_speed_frame_recievied(){
	mainEvent = EVENT_SPEED;
}
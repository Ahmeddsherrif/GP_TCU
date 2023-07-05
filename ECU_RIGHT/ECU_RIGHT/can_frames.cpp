#include "Sketch.h"

void can_message_recievied(){
	switch(frameResponse.can_id){
		case CAN_MSG_SIMULATION_CMD_ID:{
			can_message_simulation_cmd_frame_received();
			break;
		}
			
		case CAN_MSG_DMS_ID:{
			can_message_dms_frame_recievied();
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


void can_message_dms_frame_recievied(){
	switch(frameResponse.data[0]){
		case CAN_MSG_DMS_DATA_SLEEP:{
			mainEvent = EVENT_SLEEP;
			break;
		}
		case CAN_MSG_DMS_DATA_DISTRACTED:{
			mainEvent = EVENT_DISTRACTED;
			break;
		}

		default:{
			
		}
	}	
}
#include "main.h"

// Shared Resources
queue<SystemEventMessage> queueCurrentSystemEventMessage;
mutex mutexQueueCurrentSystemEventMessage;

// State Machine Variables
bool stateSysEntry;
bool stateSysExit;
SystemEventMessage currentSystemEventMessage;
SystemState currentSystemState;

// Global Variables
Mobile myMobile;
struct mosquitto *mosq;
varMSD currentVarMSD;

void on_connect(struct mosquitto *mosq, void *obj, int rc) {

	if (rc) {
		printf("Error with result code: %d\n", rc);
		exit(-1);
	}

	mosquitto_subscribe(mosq, NULL, TOPIC_ADR, 0);
	mosquitto_subscribe(mosq, NULL, TOPIC_SOS, 0);
	mosquitto_subscribe(mosq, NULL, TOPIC_SEATBELT, 0);
	mosquitto_subscribe(mosq, NULL, TOPIC_CMD, 0);
	mosquitto_subscribe(mosq, NULL, TOPIC_CMD_PROCESS, 0);
	mosquitto_subscribe(mosq, NULL, TOPIC_IMU, 0);
}

void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg) {
	string topic = string((char*) msg->topic);
	string payload = string((char*) msg->payload);

	SystemEventMessage tempSystemEventMessage(payload);
	if (topic == TOPIC_SOS) {
		if (payload == MESSAGE_ECALL) {
			tempSystemEventMessage.event = EVENT_SYS_ECALL;
		}
		else if (payload == MESSAGE_BCALL) {
			tempSystemEventMessage.event = EVENT_SYS_BCALL;
		}
		else if (payload == MESSAGE_SIGN) {
			tempSystemEventMessage.event = EVENT_SYS_SIGN;
		}
		else if (payload == MESSAGE_GUN) {
			tempSystemEventMessage.event = EVENT_SYS_GUN;
		}
	}
	else if (topic == TOPIC_ADR) {
		tempSystemEventMessage.event = EVENT_SYS_ADR_RECIEVED;
	}
	else if (topic == TOPIC_CMD || topic == TOPIC_CMD_PROCESS) {
		if (payload == MESSAGE_START) {
			tempSystemEventMessage.event = EVENT_SYS_START;
		}
		else if (payload == MESSAGE_KILL) {
			tempSystemEventMessage.event = EVENT_SYS_KILL;
		}
		else if (payload == MESSAGE_TERMINATE) {
			tempSystemEventMessage.event = EVENT_SYS_TERMINATE;
		}
		else if (payload == MESSAGE_STATUS) {
			tempSystemEventMessage.event = EVENT_STATUS;
		}
	}

	else if (topic == TOPIC_SEATBELT){
		tempSystemEventMessage.event = EVENT_SEATBELT_RECIEVED;
	}

	else if (topic == TOPIC_IMU){
		tempSystemEventMessage.event = EVENT_IMU_RECIEVED;
	}

	unique_lock<mutex> lockMutexQueueCurrentSystemEventMessage(mutexQueueCurrentSystemEventMessage);
	queueCurrentSystemEventMessage.push(tempSystemEventMessage);
	lockMutexQueueCurrentSystemEventMessage.unlock();
}

int main() {

	system_onetime_intialization();

	while (true) {

		getEvents();

		//break condition
		if (currentSystemEventMessage.event == EVENT_SYS_TERMINATE) {
			string message = MESSAGE_STATUS_TERMINATE;
			mosquitto_publish(mosq, NULL, TOPIC_STATUS_PROCESS, message.length(), message.c_str(), 0, false);
			break;
		}

		// Process Events
		switch (currentSystemState) {
			case STATE_SYS_DEAD: {
				state_sys_dead_handler();
				break;
			}
			case STATE_SYS_IDLE: {
				state_sys_idle_handler();
				break;
			}
			case STATE_SYS_SOS: {
				state_sys_sos_handler();
				break;
			}
			case STATE_SYS_CALL: {
				state_sys_call_handler();
				break;
			}
		}

	}

	system_end();

	return 0;
}


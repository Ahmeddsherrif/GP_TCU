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
queue<double> queue_sensorReading_gyro;
queue<double> queue_sensorReading_speedometer;
queue<GPSLocation> queue_sensorReading_gps;


struct mosquitto *mosq;

void on_connect(struct mosquitto *mosq, void *obj, int rc) {

	if (rc) {
		printf("Error with result code: %d\n", rc);
		exit(-1);
	}

	mosquitto_subscribe(mosq, NULL, TOPIC_IMU, 0);
	mosquitto_subscribe(mosq, NULL, TOPIC_SPEEDOMETER, 0);
	mosquitto_subscribe(mosq, NULL, TOPIC_GPS, 0);
	mosquitto_subscribe(mosq, NULL, TOPIC_CMD, 0);
	mosquitto_subscribe(mosq, NULL, TOPIC_CMD_PROCESS, 0);
}

void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg) {
	string topic = string((char*) msg->topic);
	string payload = string((char*) msg->payload);

	SystemEventMessage tempSystemEventMessage(payload);

	if (topic == TOPIC_IMU) {
		tempSystemEventMessage.event = EVENT_SYS_IMU_SAMPLE_RECIVED;
	}
	else if (topic == TOPIC_SPEEDOMETER) {
		tempSystemEventMessage.event = EVENT_SYS_SPEEDOMETER_SAMPLE_RECIVED;
	}
	else if (topic == TOPIC_GPS) {
		tempSystemEventMessage.event = EVENT_SYS_GPS_SAMPLE_RECIVED;
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
		else if (payload == MESSAGE_STATUS){
			tempSystemEventMessage.event = EVENT_STATUS;
		}
	}


	unique_lock<mutex> lockMutexQueueCurrentSystemEventMessage(mutexQueueCurrentSystemEventMessage);
	queueCurrentSystemEventMessage.push(tempSystemEventMessage);
	lockMutexQueueCurrentSystemEventMessage.unlock();
}

int main() {

	system_onetime_intialization();

	while (true) {
		// get Events
		currentSystemEventMessage.event = EVENT_SYS_IDLE;
		currentSystemEventMessage.data = "";

		unique_lock<mutex> lockMutexQueueCurrentSystemEventMessage(mutexQueueCurrentSystemEventMessage);
		if (queueCurrentSystemEventMessage.empty() == false) {
			currentSystemEventMessage = queueCurrentSystemEventMessage.front();
			queueCurrentSystemEventMessage.pop();
		}

		lockMutexQueueCurrentSystemEventMessage.unlock();

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
			case STATE_SYS_ACTIVE: {
				state_sys_active_handler();
				break;
			}
		}

	}

	system_end();

	return 0;
}
#include "main.h"

void getEvents() {

	currentSystemEventMessage.event = EVENT_SYS_IDLE;
	currentSystemEventMessage.data = "";

	// get SIM events
	myMobile.PollLine();

	if (myMobile.isRinging() == true) {
		currentSystemEventMessage.event = EVENT_SYS_CALL_IN;
	}
	else if (myMobile.isNoCarrier() == true) {
		currentSystemEventMessage.event = EVENT_SYS_CALL_OUT;
	}
	else if (myMobile.isNoCarrier() == true) {
		currentSystemEventMessage.event = EVENT_SYS_CALL_OUT;
	}
	else if (myMobile.isSMSRecieved() == true) {
		vector<string> SMS_Data;
		vector<string> parsedString;

		myMobile.readLatestSMS(SMS_Data);

		if (SMS_Data[1] == SMS_CMD_END) {
			currentSystemEventMessage.event = EVENT_SYS_END;
		}
		else {
			myMobile.clearAllSMS();
		}
	}

	else {
		// get MQTT Events
		unique_lock<mutex> lockMutexQueueCurrentSystemEventMessage(mutexQueueCurrentSystemEventMessage);
		if (queueCurrentSystemEventMessage.empty() == false) {
			currentSystemEventMessage = queueCurrentSystemEventMessage.front();
			queueCurrentSystemEventMessage.pop();
		}
		lockMutexQueueCurrentSystemEventMessage.unlock();
	}
}

void system_end() {
	mosquitto_loop_stop(mosq, true);
	mosquitto_disconnect(mosq);
	mosquitto_destroy(mosq);
	mosquitto_lib_cleanup();

	TRACE_PRINT("HASTA LAVISTA!");
}

void system_onetime_intialization() {
	currentSystemState = STATE_SYS_INTIAL;
	stateSysEntry = true;
	stateSysExit = false;

	myMobile.begin();

	int rc;

	mosquitto_lib_init();

	mosq = mosquitto_new(NULL, true, NULL);
	mosquitto_connect_callback_set(mosq, on_connect);
	mosquitto_message_callback_set(mosq, on_message);

	rc = mosquitto_connect(mosq, BROKER_ADDRESS, BROKER_PORT, 60);

	if (rc != 0) {
		printf("Client could not connect to broker! Error Code: %d\n", rc);
		mosquitto_destroy(mosq);
		exit(-1);
	}

	mosquitto_loop_start(mosq);

}

void system_constant_reintialization() {
	myMobile.clearAllSMS();
	currentVarMSD.clear();
}

void state_sys_dead_handler() {
	if (stateSysEntry == true) {
		stateSysEntry = false;

		string message = MESSAGE_STATUS_DEAD;
		mosquitto_publish(mosq, NULL, TOPIC_STATUS_PROCESS, message.length(), message.c_str(), 0, false);

		TRACE_PRINT("Entering Dead State.");
		TRACE_PRINT("Waiting ...");
	}

	// Process State
	state_sys_dead_handler_processing();

	// Process Event

	switch (currentSystemEventMessage.event) {

		case EVENT_SYS_START: {
			currentSystemState = STATE_SYS_IDLE;
			stateSysExit = true;
			break;
		}

		case EVENT_STATUS: {
			string message = MESSAGE_STATUS_DEAD;
			mosquitto_publish(mosq, NULL, TOPIC_STATUS_PROCESS, message.length(), message.c_str(), 0, false);
			break;
		}

		case EVENT_SYS_IDLE:
		case EVENT_SYS_KILL:
		case EVENT_SYS_END:
		case EVENT_SYS_ECALL:
		case EVENT_SYS_BCALL:
		case EVENT_SYS_SIGN:
		case EVENT_SYS_GUN:
		case EVENT_SYS_CALL_IN:
		case EVENT_SYS_CALL_OUT:
		case EVENT_SYS_ADR_RECIEVED:
		case EVENT_SYS_TERMINATE:
		default: {

		}
	}

	if (stateSysExit == true) {
		stateSysExit = false;
		TRACE_PRINT("Exiting Dead State.");

		// reintialize variables
		system_constant_reintialization();

		stateSysEntry = true;
	}
}

void state_sys_idle_handler() {
	if (stateSysEntry == true) {
		stateSysEntry = false;

		string message = MESSAGE_STATUS_IDLE;
		mosquitto_publish(mosq, NULL, TOPIC_STATUS_PROCESS, message.length(), message.c_str(), 0, false);


		message = MESSAGE_REQUEST_SEAT_BELT;
		mosquitto_publish(mosq, NULL, TOPIC_SEATBELT, message.length(), message.c_str(), 0, false);

		TRACE_PRINT("Entering Idle State.");
	}

	// Process The State
	state_sys_idle_handler_processing();

	switch (currentSystemEventMessage.event) {

		case EVENT_SYS_KILL: {
			currentSystemState = STATE_SYS_DEAD;
			stateSysExit = true;
			break;
		}

		case EVENT_SYS_ECALL: {
			currentVarMSD.activation = MSD_AUTOMATIC_ACTIVATION;
			currentVarMSD.threatDetected = MSD_NO_THREAT;

			sendMSD();

			currentSystemState = STATE_SYS_SOS;
			stateSysExit = true;
			break;
		}
		case EVENT_SYS_BCALL: {
			currentVarMSD.activation = MSD_MANUAL_ACTIVATION;
			currentVarMSD.threatDetected = MSD_NO_THREAT;

			sendMSD();

			currentSystemState = STATE_SYS_SOS;
			stateSysExit = true;
			break;
		}
		case EVENT_SYS_SIGN: {
			currentVarMSD.activation = MSD_AUTOMATIC_ACTIVATION;
			currentVarMSD.threatDetected = MSD_THREAT;

			sendMSD();

			currentSystemState = STATE_SYS_SOS;
			stateSysExit = true;
			break;
		}
		case EVENT_SYS_GUN: {
			currentVarMSD.activation = MSD_AUTOMATIC_ACTIVATION;
			currentVarMSD.threatDetected = MSD_THREAT;

			sendMSD();

			currentSystemState = STATE_SYS_SOS;
			stateSysExit = true;
			break;
		}
		case EVENT_SYS_CALL_IN: {
			myMobile.Hang();
			break;
		}

		case EVENT_SYS_ADR_RECIEVED: {
			// Parse CSV String
			string csvString = currentSystemEventMessage.data;
			stringstream ss(csvString);
			string item;

			getline(ss, item, ',');
			if (item == STATUS_OUTAGE) {
				currentVarMSD.PositionTrust = MSD_POSITION_ESTIMATED;
			}
			else if (item == STATUS_NO_OUTAGE) {
				currentVarMSD.PositionTrust = MSD_POSITION_TRUSTED;
			}

			getline(ss, item, ',');
			currentVarMSD.gpsLocation.latitude = stod(item);

			getline(ss, item, ',');
			currentVarMSD.gpsLocation.longitude = stod(item);

			break;
		}

		case EVENT_STATUS: {
			string message = MESSAGE_STATUS_IDLE;
			mosquitto_publish(mosq, NULL, TOPIC_STATUS_PROCESS, message.length(), message.c_str(), 0, false);
			break;
		}
		case EVENT_SEATBELT_RECIEVED: {
			if (currentSystemEventMessage.data == MESSAGE_LEFT_SEAT_BELT_HOOKED) {
				currentVarMSD.passenger_left = true;
			}
			else if (currentSystemEventMessage.data == MESSAGE_LEFT_SEAT_BELT_UNHOOKED) {
				currentVarMSD.passenger_left = false;
			}
			else if (currentSystemEventMessage.data == MESSAGE_RIGHT_SEAT_BELT_HOOKED) {
				currentVarMSD.passenger_right = true;
			}
			else if (currentSystemEventMessage.data == MESSAGE_RIGHT_SEAT_BELT_UNHOOKED) {
				currentVarMSD.passenger_right = false;
			}
			break;
		}

		case EVENT_IMU_RECIEVED:{
			currentVarMSD.direction = stod(currentSystemEventMessage.data);
			break;
		}
		case EVENT_SYS_START:
		case EVENT_SYS_IDLE:
		case EVENT_SYS_END:
		case EVENT_SYS_CALL_OUT:
		case EVENT_SYS_TERMINATE:
		default: {

			break;
		}
	}

	if (stateSysExit == true) {
		stateSysExit = false;

		TRACE_PRINT("Exiting Idle State.");
		stateSysEntry = true;
	}
}

void state_sys_sos_handler() {
	if (stateSysEntry == true) {
		stateSysEntry = false;

		string message = MESSAGE_STATUS_SOS;
		mosquitto_publish(mosq, NULL, TOPIC_STATUS_PROCESS, message.length(), message.c_str(), 0, false);

		TRACE_PRINT("Entering SOS State.");
	}

	// Process State
	state_sys_sos_handler_processing();

	switch (currentSystemEventMessage.event) {

		case EVENT_SYS_KILL: {
			currentSystemState = STATE_SYS_DEAD;
			stateSysExit = true;
			break;
		}
		case EVENT_SYS_END: {

			string message = MESSAGE_END;
			mosquitto_publish(mosq, NULL, TOPIC_SOS, message.length(), message.c_str(), 0, false);

			currentSystemState = STATE_SYS_IDLE;
			stateSysExit = true;
			break;
		}
		case EVENT_SYS_CALL_IN: {
			myMobile.Answer();
			currentSystemState = STATE_SYS_CALL;
			stateSysExit = true;
			break;
		}

		case EVENT_STATUS: {
			string message = MESSAGE_STATUS_SOS;
			mosquitto_publish(mosq, NULL, TOPIC_STATUS_PROCESS, message.length(), message.c_str(), 0, false);
			break;
		}

		case EVENT_SYS_START:
		case EVENT_SYS_IDLE:
		case EVENT_SYS_ECALL:
		case EVENT_SYS_BCALL:
		case EVENT_SYS_SIGN:
		case EVENT_SYS_GUN:
		case EVENT_SYS_CALL_OUT:
		case EVENT_SYS_TERMINATE:
		case EVENT_SYS_ADR_RECIEVED:
		default: {

			break;
		}
	}

	if (stateSysExit == true) {
		stateSysExit = false;

		TRACE_PRINT("Exiting SOS State.");

		stateSysEntry = true;
	}
}

void state_sys_call_handler() {
	if (stateSysEntry == true) {
		stateSysEntry = false;
		string message = MESSAGE_STATUS_CALL;
		mosquitto_publish(mosq, NULL, TOPIC_STATUS_PROCESS, message.length(), message.c_str(), 0, false);

		TRACE_PRINT("Entering CALL State.");
	}

	// Process State
	state_sys_call_handler_processing();

	switch (currentSystemEventMessage.event) {

		case EVENT_SYS_KILL: {
			myMobile.Hang();
			currentSystemState = STATE_SYS_DEAD;
			stateSysExit = true;
			break;
		}
		case EVENT_SYS_END: {
			myMobile.Hang();
			currentSystemState = STATE_SYS_IDLE;
			stateSysExit = true;
			break;
		}
		case EVENT_SYS_CALL_OUT: {
			currentSystemState = STATE_SYS_SOS;
			stateSysExit = true;
			break;
		}

		case EVENT_STATUS: {
			string message = MESSAGE_STATUS_CALL;
			mosquitto_publish(mosq, NULL, TOPIC_STATUS_PROCESS, message.length(), message.c_str(), 0, false);
			break;
		}
		case EVENT_SYS_START:
		case EVENT_SYS_IDLE:
		case EVENT_SYS_ECALL:
		case EVENT_SYS_BCALL:
		case EVENT_SYS_SIGN:
		case EVENT_SYS_GUN:
		case EVENT_SYS_CALL_IN:
		case EVENT_SYS_TERMINATE:
		case EVENT_SYS_ADR_RECIEVED:
		default: {

			break;
		}
	}

	if (stateSysExit == true) {
		stateSysExit = false;

		TRACE_PRINT("Exiting CALL State.");

		stateSysEntry = true;
	}
}

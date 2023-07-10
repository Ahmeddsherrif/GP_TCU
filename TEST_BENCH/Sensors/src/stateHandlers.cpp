#include "main.h"

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

	gyroFileName = string(DATA_DIR) + string(DATA_IMU_FILE);
	gyro = make_unique<GyroscopeSensor>(gyroFileName);
	maxGyroSampleNumber = gyro->getNumberOfSamples() - 1;

	speedometerFileName = string(DATA_DIR) + string(DATA_SPEEDOMETER_FILE);
	speedometer = make_unique<SpeedometerSensor>(speedometerFileName);
	maxSpeedometerSampleNumber = speedometer->getNumberOfSamples() - 1;

	gpsFileName = string(DATA_DIR) + string(DATA_GPS_FILE);
	gps = make_unique<GPSSensor>(gpsFileName);
	maxGpsSampleNumber = gps->getNumberOfSamples() - 1;

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
	outageStatus = MESSAGE_NO_OUTAGE;

	gyroSampleNumber = 0;
	currentGyroSample = "";

	speedometerSampleNumber = 0;
	currentSpeedSample = "";

	gpsSampleNumber = 0;
	currentGpsSample = "";
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
			currentSystemState = STATE_SYS_ACTIVE;
			stateSysExit = true;
			break;
		}

		case EVENT_STATUS:{
			string message = MESSAGE_STATUS_DEAD;
			mosquitto_publish(mosq, NULL, TOPIC_STATUS_PROCESS, message.length(), message.c_str(), 0, false);
			break;
		}

		case EVENT_SYS_IDLE:
		case EVENT_SYS_KILL:
		case EVENT_SYS_PAUSE:
		case EVENT_SYS_RESUME:
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

void state_sys_active_handler() {
	if (stateSysEntry == true) {
		stateSysEntry = false;

		string message = MESSAGE_STATUS_ACTIVE;
		mosquitto_publish(mosq, NULL, TOPIC_STATUS_PROCESS, message.length(), message.c_str(), 0, false);

		TRACE_PRINT("Entering Active State.");
	}

	// Process The State
	state_sys_active_handler_processing();

	switch (currentSystemEventMessage.event) {

		case EVENT_SYS_KILL: {
			currentSystemState = STATE_SYS_DEAD;
			stateSysExit = true;
			break;
		}
		case EVENT_SYS_PAUSE: {
			currentSystemState = STATE_SYS_HALTED;
			stateSysExit = true;
			break;
		}

		case EVENT_SYS_OUTAGE_STATUS_RECIVED: {
			outageStatus = currentSystemEventMessage.data;
			break;
		}

		case EVENT_STATUS:{
			string message = MESSAGE_STATUS_ACTIVE;
			cout << "SENT STATUS" << endl;
			mosquitto_publish(mosq, NULL, TOPIC_STATUS_PROCESS, message.length(), message.c_str(), 0, false);
			break;
		}

		case EVENT_SYS_RESUME:
		case EVENT_SYS_START:
		case EVENT_SYS_IDLE:
		case EVENT_SYS_TERMINATE:
		default: {

			break;
		}
	}

	if (stateSysExit == true) {
		stateSysExit = false;

		TRACE_PRINT("Exiting Active State.");
		stateSysEntry = true;
	}
}

void state_sys_halted_handler() {
	if (stateSysEntry == true) {
		stateSysEntry = false;

		string message = MESSAGE_STATUS_HALT;
		mosquitto_publish(mosq, NULL, TOPIC_STATUS_PROCESS, message.length(), message.c_str(), 0, false);
		TRACE_PRINT("Entering Halted State.");
	}

	// Process State
	state_sys_halted_handler_processing();

	switch (currentSystemEventMessage.event) {

		case EVENT_SYS_RESUME: {
			currentSystemState = STATE_SYS_ACTIVE;
			stateSysExit = true;
			break;
		}

		case EVENT_SYS_KILL: {
			currentSystemState = STATE_SYS_DEAD;
			stateSysExit = true;
			break;
		}

		case EVENT_STATUS:{
			string message = MESSAGE_STATUS_HALT;
			mosquitto_publish(mosq, NULL, TOPIC_STATUS_PROCESS, message.length(), message.c_str(), 0, false);
			cout << "SENT STATUS" << endl;
			break;
		}

		case EVENT_SYS_IDLE:
		case EVENT_SYS_PAUSE:
		case EVENT_SYS_START:
		case EVENT_SYS_TERMINATE:
		default: {

			break;
		}
	}

	if (stateSysExit == true) {
		stateSysExit = false;

		TRACE_PRINT("Exiting Halted State.");

		stateSysEntry = true;
	}
}

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
	while (queue_sensorReading_gyro.empty() == false) {
		queue_sensorReading_gyro.pop();
	}
	while (queue_sensorReading_speedometer.empty() == false) {
		queue_sensorReading_speedometer.pop();
	}
	while (queue_sensorReading_gps.empty() == false) {
		queue_sensorReading_gps.pop();
	}

	ADR::clear();
}

void state_sys_dead_handler() {
	if (stateSysEntry == true) {
		stateSysEntry = false;

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

		case EVENT_SYS_IDLE:
		case EVENT_SYS_KILL:
		case EVENT_SYS_GPS_SAMPLE_RECIVED:
		case EVENT_SYS_IMU_SAMPLE_RECIVED:
		case EVENT_SYS_SPEEDOMETER_SAMPLE_RECIVED:
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

		case EVENT_SYS_IMU_SAMPLE_RECIVED: {
			double currentGyroSample = stod(currentSystemEventMessage.data);
			queue_sensorReading_gyro.push(currentGyroSample);
			break;
		}
		case EVENT_SYS_SPEEDOMETER_SAMPLE_RECIVED: {
			double currentSpeedSample = stod(currentSystemEventMessage.data);
			queue_sensorReading_speedometer.push(currentSpeedSample);
			break;
		}

		case EVENT_SYS_GPS_SAMPLE_RECIVED: {
			state_sys_active_process_event_gps_sample_recieved();
			break;
		}

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


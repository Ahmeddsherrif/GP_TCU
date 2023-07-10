#include "main.h"

void state_sys_dead_handler_processing() {
	std::this_thread::sleep_for(std::chrono::milliseconds(1));
}

void state_sys_active_handler_processing() {
	std::this_thread::sleep_for(std::chrono::milliseconds(1));
}

void state_sys_active_process_event_gps_sample_recieved() {
	GPSLocation currentGpsSample;

	// Parse CSV String
	string csvString = currentSystemEventMessage.data;
	stringstream ss(csvString);
	string item;

	getline(ss, item, ',');
	string currentOutageStatus = item;

	getline(ss, item, ',');
	currentGpsSample.latitude = stod(item);

	getline(ss, item, ',');
	currentGpsSample.longitude = stod(item);

	queue_sensorReading_gps.push(currentGpsSample);

	string currentADR = currentOutageStatus + ",";

	currentADR += ADR::getADR_GPS(queue_sensorReading_speedometer, queue_sensorReading_gyro, queue_sensorReading_gps,
			currentOutageStatus);

	mosquitto_publish(mosq, NULL, TOPIC_ADR, currentADR.length(), currentADR.c_str(), 0, false);
}


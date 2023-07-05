#include "main.h"

void state_sys_dead_handler_processing() {
	std::this_thread::sleep_for(std::chrono::milliseconds(1));
}

void state_sys_halted_handler_processing() {
	std::this_thread::sleep_for(std::chrono::milliseconds(1));
}

void state_sys_active_handler_processing() {
	std::chrono::steady_clock::time_point currentTime = std::chrono::steady_clock::now();

	// Code block to execute every 10ms
	if (currentTime - lastTimeImu >= milliseconds(PERIOD_IMU)) {
		lastTimeImu = currentTime;
		// Insert your code here
		currentGyroSample = gyro->getYawRate(gyroSampleNumber);
		gyroSampleNumber++;

		if (gyroSampleNumber >= maxGyroSampleNumber) {
			unique_lock<mutex> lockMutexQueueCurrentSystemEventMessage(mutexQueueCurrentSystemEventMessage);
			queueCurrentSystemEventMessage.emplace(EVENT_SYS_TERMINATE, "");
			lockMutexQueueCurrentSystemEventMessage.unlock();
		}

		mosquitto_publish(mosq, NULL, TOPIC_IMU, currentGyroSample.length(), currentGyroSample.c_str(), 0, false);

	}

	// Code block to execute every 40ms
	if (currentTime - lastTimeSpeedometer >= milliseconds(PERIOD_SPEEDOMETER)) {
		lastTimeSpeedometer = currentTime;
		// Insert your code here
		currentSpeedSample = speedometer->getSpeed(speedometerSampleNumber);
		speedometerSampleNumber++;

		if (speedometerSampleNumber >= maxSpeedometerSampleNumber) {
			unique_lock<mutex> lockMutexQueueCurrentSystemEventMessage(mutexQueueCurrentSystemEventMessage);
			queueCurrentSystemEventMessage.emplace(EVENT_SYS_TERMINATE, "");
			lockMutexQueueCurrentSystemEventMessage.unlock();
		}

		mosquitto_publish(mosq, NULL, TOPIC_SPEEDOMETER, currentSpeedSample.length(), currentSpeedSample.c_str(), 0,
				false);

	}

	// Code block to execute every 1000ms
	if (currentTime - lastTimeGps >= milliseconds(PERIOD_GPS)) {
		lastTimeGps = currentTime;
		// Insert your code here
		currentGpsSample = gps->getGPS(gpsSampleNumber);
		gpsSampleNumber++;

		if (gpsSampleNumber >= maxGpsSampleNumber) {
			unique_lock<mutex> lockMutexQueueCurrentSystemEventMessage(mutexQueueCurrentSystemEventMessage);
			queueCurrentSystemEventMessage.emplace(EVENT_SYS_TERMINATE, "");
			lockMutexQueueCurrentSystemEventMessage.unlock();
		}

		// Adds The outage status
		currentGpsSample += "," + outageStatus;

		mosquitto_publish(mosq, NULL, TOPIC_GPS, currentGpsSample.length(), currentGpsSample.c_str(), 0, false);
	}

	std::this_thread::sleep_for(std::chrono::milliseconds(1));
}


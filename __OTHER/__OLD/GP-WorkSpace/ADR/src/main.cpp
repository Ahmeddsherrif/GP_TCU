#include "main.h"

queue<double> queue_sensorReading_gyro;
mutex mutexQueue_sensorReading_gyro;

queue<double> queue_sensorReading_speedometer;
mutex mutexQueue_sensorReading_speedometer;

queue<GPSLocation> queue_sensorReading_gps;
mutex mutexQueue_sensorReading_gps;

outageStatus currentOutageStatus;
mutex mutexCurrentOutageStatus;

mainEvent currentMainEvent = EVENT_IDLE;
mutex mutexCurrentMainEvent;

mainState currentMainState = STATE_IDLE;

bool isActive = false;
bool isHalted = false;

void on_connect(struct mosquitto *mosq, void *obj, int rc) {

	if (rc) {
		printf("Error with result code: %d\n", rc);
		exit(-1);
	}

	mosquitto_subscribe(mosq, NULL, TOPIC_IMU, 0);
	mosquitto_subscribe(mosq, NULL, TOPIC_SPEEDOMETER, 0);
	mosquitto_subscribe(mosq, NULL, TOPIC_GPS, 0);
	mosquitto_subscribe(mosq, NULL, TOPIC_CMD, 0);
}

void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg) {

	if (strcmp(msg->topic, TOPIC_IMU) == 0) {
		double currentGyroSample = strtod((char*) msg->payload, nullptr);

		unique_lock<mutex> lockMutexQueue_sensorReading_gyro(mutexQueue_sensorReading_gyro);
		queue_sensorReading_gyro.push(currentGyroSample);
		lockMutexQueue_sensorReading_gyro.unlock();
	}
	else if (strcmp(msg->topic, TOPIC_SPEEDOMETER) == 0) {
		double currentSpeedSample = strtod((char*) msg->payload, nullptr);

		unique_lock<mutex> lockMutexQueue_sensorReading_speedometer(mutexQueue_sensorReading_speedometer);
		queue_sensorReading_speedometer.push(currentSpeedSample);
		lockMutexQueue_sensorReading_speedometer.unlock();
	}
	else if (strcmp(msg->topic, TOPIC_GPS) == 0) {
		GPSLocation currentGpsSample;

		char *token = strtok((char*) msg->payload, ",");
		currentGpsSample.latitude = atof(token);

		token = strtok(NULL, ",");
		currentGpsSample.longitude = atof(token);

		token = strtok(NULL, ",");

		unique_lock<mutex> lockMutexCurrentOutageStatus(mutexCurrentOutageStatus);
		if (strcmp(token, STATUS_OUTAGE) == 0) {
			currentOutageStatus = OUTAGE;
		}
		else if (strcmp(token, STATUS_NO_OUTAGE) == 0) {
			currentOutageStatus = NO_OUTAGE;
		}
		lockMutexCurrentOutageStatus.unlock();

		unique_lock<mutex> lockMutexQueue_sensorReading_gps(mutexQueue_sensorReading_gps);
		queue_sensorReading_gps.push(currentGpsSample);
		lockMutexQueue_sensorReading_gps.unlock();

		unique_lock<mutex> lockMutexCurrentMainEvent(mutexCurrentMainEvent);
		currentMainEvent = EVENT_NEW_GPS_SAMPLE;
		lockMutexCurrentMainEvent.unlock();
	}

	else if (strcmp(msg->topic, TOPIC_CMD) == 0) {
		if (strcmp((char*) msg->payload, MESSAGE_START) == 0) {
			isActive = true;
			cout << "Starting..." << endl;
		}
		else if (strcmp((char*) msg->payload, MESSAGE_KILL) == 0) {
			isActive = false;
			cout << "Ending..." << endl;
		}
	}
}

int main() {

	int rc;

	mosquitto_lib_init();

	struct mosquitto *mosq;

	mosq = mosquitto_new(NULL, true, NULL);
	mosquitto_connect_callback_set(mosq, on_connect);
	mosquitto_message_callback_set(mosq, on_message);

	rc = mosquitto_connect(mosq, "localhost", 1883, 10);
	if (rc) {
		printf("Could not connect to Broker with return code %d\n", rc);
		return -1;
	}

	mosquitto_loop_start(mosq);

	cout << "Waiting ..." << endl;

	while (isActive == false) {
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	while (isActive == true) {
		unique_lock<mutex> lockMutexCurrentMainEvent(mutexCurrentMainEvent);
		switch (currentMainEvent) {
			case EVENT_IDLE: {
				/*DO NOTHING*/
				break;
			}
			case EVENT_NEW_GPS_SAMPLE: {
				currentMainEvent = EVENT_IDLE;
				currentMainState = STATE_PROCESS_GPS_SAMPLE;
				break;
			}
		}
		lockMutexCurrentMainEvent.unlock();

		switch (currentMainState) {
			case STATE_IDLE: {
				/*DO NOTHING*/
				break;
			}
			case STATE_PROCESS_GPS_SAMPLE: {
				currentMainState = STATE_IDLE;


				string currentADR_gpsSample;

				unique_lock<mutex> lockMutexCurrentOutageStatus(mutexCurrentOutageStatus);
				switch(currentOutageStatus){
					case OUTAGE:{
						currentADR_gpsSample = STATUS_OUTAGE;
						break;
					}
					case NO_OUTAGE:{
						currentADR_gpsSample = STATUS_NO_OUTAGE;
						break;
					}
				}
				lockMutexCurrentOutageStatus.unlock();

				currentADR_gpsSample += ",";

				currentADR_gpsSample += ADR::getADR_GPS(queue_sensorReading_speedometer,
						mutexQueue_sensorReading_speedometer, queue_sensorReading_gyro, mutexQueue_sensorReading_gyro,
						queue_sensorReading_gps, mutexQueue_sensorReading_gps, currentOutageStatus,
						mutexCurrentOutageStatus);

				mosquitto_publish(mosq, NULL, TOPIC_ADR, currentADR_gpsSample.length(), currentADR_gpsSample.c_str(), 0,
						false);

				break;
			}
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}

	mosquitto_loop_stop(mosq, true);

	mosquitto_disconnect(mosq);
	mosquitto_destroy(mosq);
	mosquitto_lib_cleanup();

	return 0;
}

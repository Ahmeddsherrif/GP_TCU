#include "main.h"

bool isActive = false;

sos currentSOS = SOS_IDLE;
mutex mutexCurrentSOS;

GPSReading currentGPSReading;
mutex mutexCurrentGPSReading;

void on_connect(struct mosquitto *mosq, void *obj, int rc) {

	if (rc) {
		printf("Error with result code: %d\n", rc);
		exit(-1);
	}

	mosquitto_subscribe(mosq, NULL, TOPIC_ADR, 0);
	mosquitto_subscribe(mosq, NULL, TOPIC_SOS, 0);
	mosquitto_subscribe(mosq, NULL, TOPIC_CMD, 0);
}

void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg) {
	if (strcmp(msg->topic, TOPIC_CMD) == 0) {
		if (strcmp((char*) msg->payload, MESSAGE_START) == 0) {

			isActive = true;
			cout << "Starting..." << endl;
		}
		else if (strcmp((char*) msg->payload, MESSAGE_KILL) == 0) {
			isActive = false;
			cout << "Ending..." << endl;
		}
	}

	else if (strcmp(msg->topic, TOPIC_SOS) == 0) {

		unique_lock<mutex> lockMutexCurrentSOS(mutexCurrentSOS);

		if (strcmp((char*) msg->payload, MESSAGE_ECALL) == 0) {
			currentSOS = SOS_ECALL;
		}
		else if (strcmp((char*) msg->payload, MESSAGE_BCALL) == 0) {
			currentSOS = SOS_BCALL;
		}
		else if (strcmp((char*) msg->payload, MESSAGE_GUN) == 0) {
			currentSOS = SOS_GUN;
		}
		else if (strcmp((char*) msg->payload, MESSAGE_SIGN) == 0) {
			currentSOS = SOS_SIGN;
		}

		lockMutexCurrentSOS.unlock();
	}

	else if (strcmp(msg->topic, TOPIC_ADR) == 0) {

		unique_lock<mutex> lockMutexCurrentGPSReading(mutexCurrentGPSReading);

		char *token = strtok((char*) msg->payload, ",");
		currentGPSReading.Status = string((char*) token);


		token = strtok(NULL, ",");
		currentGPSReading.Location.latitude = atof(token);


		token = strtok(NULL, ",");
		currentGPSReading.Location.longitude = atof(token);

		lockMutexCurrentGPSReading.unlock();
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

	Mobile myMobile;
	myMobile.begin();

	State state = State::IDLE;

	while (isActive == true) {

		switch (state) {
			case State::IDLE: {
				myMobile.PollLine();

				if (myMobile.isRinging() == true) {
					myMobile.Hang();
				}

				unique_lock<mutex> lockMutexCurrentSOS(mutexCurrentSOS);

				if (currentSOS != SOS_IDLE) {

					MSDEncoder msd(CONFIG_FILE_DIR);
					string emergencyPhoneNumber = msd.getEmergencyPhoneNumber();

					switch (currentSOS) {
						case SOS_IDLE: {

							break;
						}
						case SOS_ECALL: {
							msd.setActivation(true);
							break;
						}

						case SOS_BCALL: {
							msd.setActivation(false);
							break;
						}

						case SOS_GUN: {

							break;
						}

						case SOS_SIGN: {

							break;
						}
					}

					currentSOS = SOS_IDLE;

					unique_lock<mutex> lockMutexCurrentGPSReading(mutexCurrentGPSReading);

					msd.setPositionLatitude(currentGPSReading.Location.latitude);
					msd.setPositionLongitude(currentGPSReading.Location.longitude);

					if (currentGPSReading.Status == STATUS_NO_OUTAGE) {
						msd.setPositionTrust(true);
					}

					else if (currentGPSReading.Status == STATUS_OUTAGE) {
						msd.setPositionTrust(false);
					}

					lockMutexCurrentGPSReading.unlock();

					msd.setTestCall(false);

					msd.setVehicleDirection(50);
					msd.setNumberOfPassengers(4);

					string encodedMSDString = msd.encode();
					myMobile.sendSMS(emergencyPhoneNumber, encodedMSDString);

					state = State::ACTIVE;
					LOG("STATE: IDLE > ACTIVE");
				}

				lockMutexCurrentSOS.unlock();

				break;
			}

			case State::ACTIVE: {

				myMobile.PollLine();

				if (myMobile.isRinging() == true) {
					myMobile.Answer();
					state = State::IN_CALL;
					LOG("STATE: ACTIVE > IN_CALL");
					LOG("=============================== IN CALL ===============================");
				}

				if (myMobile.isSMSRecieved()) {
					vector<string> SMS_Data;
					vector<string> parsedString;

					myMobile.readLatestSMS(SMS_Data);

					if (SMS_Data[1] == SMS_CMD_END) {
						state = State::IDLE;
						LOG("STATE: ACTIVE > IDLE");

					}
				}

				break;
			}

			case State::IN_CALL: {

				myMobile.PollLine();

				if (myMobile.isNoCarrier() == true) {
					state = State::ACTIVE;
					LOG("STATE: IN_CALL > ACTIVE");
					LOG("============================= CALL ENDED =============================");
				}

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

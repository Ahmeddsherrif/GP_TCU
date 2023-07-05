#include "main.h"

void on_connect(struct mosquitto *mosq, void *obj, int rc) {

	if (rc) {
		printf("Error with result code: %d\n", rc);
		exit(-1);
	}

	// subscribe to sign to halt the sensors
	mosquitto_subscribe(mosq, NULL, TOPIC_SOS, 0);

}

void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg) {
	string topic = string(msg->topic);
	string payload = string((char*) msg->payload);

	if (topic == TOPIC_SOS) {
		if (payload == MESSAGE_SIGN || payload == MESSAGE_GUN) {
			string message = MESSAGE_PAUSE;
			mosquitto_publish(mosq, NULL, TOPIC_CONTROL_SENSORS, message.length(), message.c_str(), 0, false);
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

	rc = mosquitto_connect(mosq, BROKER_ADDRESS, BROKER_PORT, 10);
	if (rc) {
		printf("Could not connect to Broker with return code %d\n", rc);
		return -1;
	}

	mosquitto_loop_start(mosq);

	cout << "--------------------------------------------" << endl;
	cout << "S -> Start Simulation" << endl;
	cout << "O -> Start a GPS Outage" << endl;
	cout << "N -> End a GPS Outage" << endl;
	cout << "E -> Generate ECALL" << endl;
	cout << "B -> Generate BCALL" << endl;
	cout << "P -> Pause Sensor Readings" << endl;
	cout << "R -> Resume Sensor Readings" << endl;
	cout << "D -> Distracted Driver" << endl;
	cout << "Z -> Sleeping Driver (zZzZZ)" << endl;
	cout << "K -> reset Simulation" << endl;
	cout << "T -> Terminate all" << endl;
	cout << "--------------------------------------------" << endl;

	bool isActive = true;
	string message;
	char userInput;

	while (isActive == true) {
		cin >> userInput;

		switch (userInput) {
			case 'o':
			case 'O': {
				message = MESSAGE_OUTAGE;
				mosquitto_publish(mosq, NULL, TOPIC_OUTAGE, message.length(), message.c_str(), 0, false);
				break;
			}

			case 'n':
			case 'N': {
				message = MESSAGE_NO_OUTAGE;
				mosquitto_publish(mosq, NULL, TOPIC_OUTAGE, message.length(), message.c_str(), 0, false);

				break;
			}

			case 'e':
			case 'E': {
				message = MESSAGE_ECALL;
				mosquitto_publish(mosq, NULL, TOPIC_SOS, message.length(), message.c_str(), 0, false);

				message = MESSAGE_PAUSE;
				mosquitto_publish(mosq, NULL, TOPIC_CONTROL_SENSORS, message.length(), message.c_str(), 0, false);
				break;
			}

			case 'b':
			case 'B': {
				message = MESSAGE_BCALL;
				mosquitto_publish(mosq, NULL, TOPIC_SOS, message.length(), message.c_str(), 0, false);

				message = MESSAGE_PAUSE;
				mosquitto_publish(mosq, NULL, TOPIC_CONTROL_SENSORS, message.length(), message.c_str(), 0, false);
				break;
			}

			case 'p':
			case 'P': {
				message = MESSAGE_PAUSE;
				mosquitto_publish(mosq, NULL, TOPIC_CONTROL_SENSORS, message.length(), message.c_str(), 0, false);
				mosquitto_publish(mosq, NULL, TOPIC_CONTROL_VIDEO, message.length(), message.c_str(), 0, false);
				break;
			}

			case 's':
			case 'S': {
				message = MESSAGE_START;
				mosquitto_publish(mosq, NULL, TOPIC_CMD, message.length(), message.c_str(), 0, false);
				break;
			}

			case 'k':
			case 'K': {
				message = MESSAGE_KILL;
				mosquitto_publish(mosq, NULL, TOPIC_CMD, message.length(), message.c_str(), 0, false);

				break;
			}

			case 'r':
			case 'R': {
				message = MESSAGE_RESUME;
				mosquitto_publish(mosq, NULL, TOPIC_CONTROL_SENSORS, message.length(), message.c_str(), 0, false);
				mosquitto_publish(mosq, NULL, TOPIC_CONTROL_VIDEO, message.length(), message.c_str(), 0, false);
				break;
			}

			case 'z':
			case 'Z': {
				message = MESSAGE_SLEEP;
				mosquitto_publish(mosq, NULL, TOPIC_DMS, message.length(), message.c_str(), 0, false);
				break;
			}

			case 'd':
			case 'D': {
				message = MESSAGE_DISTRACTED;
				mosquitto_publish(mosq, NULL, TOPIC_DMS, message.length(), message.c_str(), 0, false);
				break;
			}

			case 't':
			case 'T': {
				isActive = false;

				message = MESSAGE_TERMINATE;
				mosquitto_publish(mosq, NULL, TOPIC_CMD, message.length(), message.c_str(), 0, false);
				break;
			}

			default: {

				break;
			}
		}

		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	mosquitto_loop_stop(mosq, true);

	mosquitto_disconnect(mosq);
	mosquitto_destroy(mosq);
	mosquitto_lib_cleanup();

	return 0;
}

//============================================================================
// Name        : ECALL.cpp
// Author      : ahmed elfeqy
// Version     :
// Copyright   : Graduation project
// Description : Hello World in C++, Ansi-style
//============================================================================

#include "main.h"

queue<string> event_queue;
mutex event_queue_mutex;

mutex isDone_mutex;
bool isDone = false;

GPSReading currentGPSReading;

void IPC_Thread() {
	MessageQueue thisProcessMessageQueue(ECALL_PROCESS_MSGQ, ECALL_PROCESS_MSGQ_KEY);
	string event;
	int messageID = 0;

	while (true) {

		unique_lock<mutex> isDone_mutexLock(isDone_mutex);
		if (isDone == true) {
			break;
		}
		isDone_mutexLock.unlock();

		thisProcessMessageQueue.waitForMessage();

		event = thisProcessMessageQueue.getLatestMessageString();
		messageID = thisProcessMessageQueue.getLatestMessageID();

		LOG("EVENT RECIEVED : " << event);

		switch (messageID) {
			case 1: {
				if (event == EVENT_KILL) {
					unique_lock<mutex> isDone_mutexLock(isDone_mutex);
					isDone = true;
					isDone_mutexLock.unlock();
				}

				break;
			}
			case 2: {
				string currentGPSReadingString = event;
				vector<string> parsedGPSReading;
				Utilities::parseCommaSepratedString(currentGPSReadingString, parsedGPSReading);

				currentGPSReading.Status = parsedGPSReading[0];
				currentGPSReading.Location.latitude = stof(parsedGPSReading[1]);
				currentGPSReading.Location.longitude = stof(parsedGPSReading[2]);

				break;
			}
			case 3: {
				unique_lock<mutex> lock(event_queue_mutex);
				event_queue.push(event);
				lock.unlock();
				break;
			}
		}

	}
}

int main() {

	thread t1(IPC_Thread);

	Mobile myMobile;
	myMobile.begin();

	string event;
	State state = State::IDLE;

	while (true) {

		unique_lock<mutex> isDone_mutexLock(isDone_mutex);
		if (isDone == true) {
			break;
		}
		isDone_mutexLock.unlock();

		if (event_queue.empty() == false) {

			unique_lock<mutex> lock(event_queue_mutex);

			event = event_queue.front();
			event_queue.pop();

			lock.unlock();

			LOG("EVENT: " << event);

		} else {
			event = "";
		}

		switch (state) {
			case State::IDLE: {
				myMobile.PollLine();

				if (myMobile.isRinging() == true) {
					myMobile.Hang();
				}

				if (event == EVENT_ECALL) {
					MSDEncoder msd("/home/ahmed/Desktop/ECALL_MSD_Config");

					msd.setActivation(false);
					msd.setTestCall(true);

					msd.setPositionLatitude(currentGPSReading.Location.latitude);
					msd.setPositionLongitude(currentGPSReading.Location.longitude);

					if (currentGPSReading.Status == "AVAILABLE") {
						msd.setPositionTrust(true);

					}

					else if (currentGPSReading.Status == "OUTAGE") {
						msd.setPositionTrust(false);

					}

					msd.setVehicleDirection(50);
					msd.setNumberOfPassengers(4);

					string encodedMSDString = msd.encode();
					string emergencyPhoneNumber = msd.getEmergencyPhoneNumber();
					myMobile.sendSMS(emergencyPhoneNumber, encodedMSDString);

					state = State::ACTIVE;
					LOG("STATE: IDLE > ACTIVE");
				}

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

					if (SMS_Data[1] == EVENT_END) {
						state = State::IDLE;
						LOG("STATE: ACTIVE > IDLE");

					}
				}

				// For test Purposes Only
				if (event == EVENT_END) {
					state = State::IDLE;
					LOG("STATE: ACTIVE > IDLE");
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

				if (event == EVENT_HANG) {
					myMobile.Hang();

					state = State::ACTIVE;
					LOG("STATE: IN_CALL > ACTIVE");
					LOG("============================= CALL ENDED =============================");

				}

				break;
			}
		}

	}

	t1.join();
	return 0;
}

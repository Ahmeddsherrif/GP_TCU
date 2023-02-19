//============================================================================
// Name        : ECALL.cpp
// Author      : ahmed elfeqy
// Version     :
// Copyright   : Graduation project
// Description : Hello World in C++, Ansi-style
//============================================================================


#include "main.h"

#define EVENT_ECALL 	"ECALL"
#define EVENT_END		"END"
#define EVENT_KILL		"KILL"

queue <string> event_queue;
mutex event_queue_mutex;


bool isDone = false;

void task(){
	string event;
	while(isDone == false){
		cin >> event;

		if (event == EVENT_KILL) {
			isDone = true;
		}

		unique_lock<mutex> lock(event_queue_mutex);
		event_queue.push(event);
		lock.unlock();
	}
}



int main() {

	thread t1(task);

	Mobile myMobile;
	myMobile.begin();

	ECALL myECALL;
	myECALL.init();

	string event;
	State state = State::IDLE;

	vector <string> SMS_Data;
	vector <string> parsedString;



	while (isDone == false) {

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

				if(myMobile.isRinging() == true){
					myMobile.Hang();
				}

				if (event == EVENT_ECALL) {
					myMobile.sendSMS(myECALL.getEmergencyPhoneNumber(), myECALL.getMSD());
					state = State::ACTIVE;
					LOG("STATE: IDLE > ACTIVE");
				}

				break;
			}

			case State::ACTIVE: {

				myMobile.PollLine();

				if(myMobile.isRinging() == true){
					myMobile.Answer();
					state = State::IN_CALL;
					LOG("STATE: ACTIVE > IN_CALL");
				}

				if(myMobile.isSMSRecieved()){
					myMobile.readLatestSMS(SMS_Data);
					if(SMS_Data[1] == EVENT_END){
						state = State::IDLE;
						LOG("STATE: ACTIVE > IDLE");

					}
				}

				// For test Purposes Only
				if(event == EVENT_END){
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
				}

				break;
			}
		}



	}

	t1.join();
	return 0;
}

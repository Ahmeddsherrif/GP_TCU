//============================================================================
// Name        : PSAP.cpp
// Author      : ahmed elfeqy
// Version     :
// Copyright   : Graduation project
// Description : Hello World in C++, Ansi-style
//============================================================================


#include "main.h"

#define EVENT_DIAL  "DIAL"
#define EVENT_END	"END"
#define EVENT_HANG	"HANG"
#define EVENT_KILL	"KILL"

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

	PSAP myPSAP;

	string event;
	State state = State::IDLE;

	vector <string> SMS_Data;
	vector <string> parsedString;
	string eCallPhoneNumber;



	while (isDone == false) {

		if (event_queue.empty() == false) {

			unique_lock<mutex> lock(event_queue_mutex);

			event = event_queue.front();
			event_queue.pop();

			lock.unlock();

			LOG("EVENT: " << event);

		}else{
			event = "";
		}


		switch (state) {
			case State::IDLE: {

				myMobile.PollLine();

				if(myMobile.isSMSRecieved() == true){
					myMobile.readLatestSMS(SMS_Data);

					Utilities::parseCommaSepratedString(SMS_Data[0], parsedString);
					Utilities::deleteCharFromString('\"', parsedString[1]);
					eCallPhoneNumber = parsedString[1];

					cout << "\n\n" << "An MSD Received From [" << eCallPhoneNumber << "] : " << endl;

					myPSAP.extractMSDfromString(SMS_Data[1]);
					myPSAP.printMSD();
					myPSAP.logMSD();

					state = State::ACTIVE;
					LOG("STATE: IDLE > ACTIVE");
				}

				break;
			}

			case State::ACTIVE: {

				if (event == EVENT_DIAL) {
					myMobile.Call(eCallPhoneNumber);
					state = State::IN_CALL;
					LOG("STATE: ACTIVE > IN_CALL");
					LOG("============================= IN CALL ================================");

				} else if (event == EVENT_END) {
					myMobile.sendSMS(eCallPhoneNumber, EVENT_END);
					state = State::IDLE;
					LOG("STATE: ACTIVE > IDLE");
				}

				break;
			}


			case State::IN_CALL:{
				myMobile.PollLine();

				if(myMobile.isNoCarrier() || myMobile.isBusy()){
					state = State::ACTIVE;
					LOG("STATE: IN_CALL > ACTIVE");
					LOG("============================= CALL ENDED =============================");

				}


				if(event == EVENT_HANG){
					myMobile.Hang();
					state = State::ACTIVE;
					LOG("STATE: IN_CALL > ACTIVE");
					LOG("============================= CALL ENDED =============================");

				}
			}
		}



	}

	t1.join();

	return 0;
}

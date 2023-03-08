//============================================================================
// Name        : ECALL.cpp
// Author      : ahmed elfeqy
// Version     :
// Copyright   : Graduation project
// Description : Hello World in C++, Ansi-style
//============================================================================


#include "main.h"




#define MAX_MSG_SIZE 1024
#define MSG_TYPE 1

// Function to initialize the message queue
int init() {
    key_t key;
    int msgid;

    // Generate a unique key for the message queue
    key = ftok("thisISaUniqueString", 'A');

    // Create a message queue
    msgid = msgget(key, 0666 | IPC_CREAT);
    if (msgid == -1) {
        cerr << "Failed to create message queue" << endl;
        return -1;
    }

    return msgid;
}

// Function to receive a message
string receive(int msgid) {
    const int BUFF_SIZE = 100;
    char buff[BUFF_SIZE];

    // Receive the message
    msgrcv(msgid, &buff, BUFF_SIZE, 0, 0);

    string x = buff;
    return x;
}

#define EVENT_ECALL 	"ECALL"
#define EVENT_END		"END"
#define EVENT_HANG		"HANG"
#define EVENT_KILL		"KILL"


queue <string> event_queue;
mutex event_queue_mutex;


bool isDone = false;

void task() {

	int msgid = init();
	if (msgid == -1) {
		return;
	}

	string event;
	while (isDone == false) {
		event = receive(msgid);

		if (event == EVENT_KILL) {
			isDone = true;
		}

		unique_lock<mutex> lock(event_queue_mutex);
		event_queue.push(event);
		lock.unlock();
	}

	// Delete the message queue
	if (msgctl(msgid, IPC_RMID, NULL) == -1) {
		cerr << "Failed to delete message queue" << endl;
		return;
	}
	cout << "Message queue deleted" << endl;
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
					LOG("=============================== IN CALL ===============================");
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
					LOG("============================= CALL ENDED =============================");
				}

				if(event == EVENT_HANG){
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

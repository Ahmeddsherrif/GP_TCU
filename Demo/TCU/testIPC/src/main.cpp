//============================================================================
// Name        : testIPC.cpp
// Author      : ahmed
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <sstream>
#include <chrono>
#include <thread>
#include <mutex>

using namespace std;
using namespace chrono;

#include "debug.h"
#include "MessageQueue.h"

string GPSReading;
mutex GPSReading_mutex;
void IPC_Thread(){

	MessageQueue thisProcessMsgQueue(COMM_PROCESS_MSGQ, COMM_PROCESS_MSGQ_KEY);
	string messageString;
	while(true){
		thisProcessMsgQueue.waitForMessage();
		messageString = thisProcessMsgQueue.getLatestMessageString();

		unique_lock<mutex> GPSReading_mutexLock(GPSReading_mutex);
		GPSReading = messageString;
		GPSReading_mutexLock.unlock();
	}
}

int main() {

	thread t1(IPC_Thread);

	MessageQueue ecallProcessesMsgQueue(ECALL_PROCESS_MSGQ, ECALL_PROCESS_MSGQ_KEY);
	MessageQueue adrProcessesMsgQueue(ADR_PROCESS_MSGQ, ADR_PROCESS_MSGQ_KEY);

	Message message;
	message.id = 5;

	string event;

	while (true) {
		cout << "Enter an Event: ";

		getline(cin, event);

		cout << "EVENT ENTERED: " << event << endl;

		memset(&message, 0, sizeof(message));
		strcpy(message.data, event.c_str());

		if (event == "KILL") {
			message.id = 1;

			adrProcessesMsgQueue.sendMessage(message);
			LOG("SENDING TO ADR PROCESS");

			ecallProcessesMsgQueue.sendMessage(message);
			LOG("SENDING TO ECALL PROCESS");

			break;
		}

		if (event == "OUTAGE" || event == "NO_OUTAGE") {
			message.id = 11;

			adrProcessesMsgQueue.sendMessage(message);
			LOG("SENDING TO ADR PROCESS");
		}

		else if (event == "START") {
			message.id = 12;

			adrProcessesMsgQueue.sendMessage(message);
			LOG("SENDING TO ADR PROCESS");

//			ecallProcessesMsgQueue.sendMessage(message);
//			LOG("SENDING TO ECALL PROCESS");
		}

		else if (event == "ECALL" || event == "BCALL") {
			LOG("REQUESTING GPS FROM ADR PROCESS");
			Message tempMessage;

			memset(&tempMessage, 0, sizeof(tempMessage));
			tempMessage.id = 1;
			strcpy(tempMessage.data, "KILL");
			adrProcessesMsgQueue.sendMessage(tempMessage);


			this_thread::sleep_for(seconds(1));


			LOG("SENDING GPS DATA TO ECALL PROCESS");
			memset(&tempMessage, 0, sizeof(tempMessage));
			tempMessage.id = 2;
			strcpy(tempMessage.data, GPSReading.c_str());
			ecallProcessesMsgQueue.sendMessage(tempMessage);

			LOG("SENDING EVENT TO ECALL PROCESS");
			message.id = 3;
			ecallProcessesMsgQueue.sendMessage(message);
		}

		else if (event == "DIAL" || event == "HANG" || event == "END") {
			message.id = 3;
			ecallProcessesMsgQueue.sendMessage(message);
			LOG("SENDING TO ECALL PROCESS");
		}
	}
	t1.join();
	return 0;
}

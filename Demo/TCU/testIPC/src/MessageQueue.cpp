/*
 * IPC.cpp
 *
 *  Created on: Mar 10, 2023
 *      Author: ahmed
 */

#include <iostream>
#include <cstring>
#include <string>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>

using namespace std;

#include "debug.h"
#include "MessageQueue.h"


MessageQueue::MessageQueue(const char *messageQueueName, char messageQueueKey){
	memset(&(this->message), 0, sizeof(this->message));

	char msgQFilePath[100] = DIR_NAME;

	strcat(msgQFilePath, messageQueueName);

	// Generate a unique key for the message queue
	key_t key;
	key = ftok(msgQFilePath, messageQueueKey);

	// Create a message queue
	this->msgid = msgget(key, 0666 | IPC_CREAT);

	LOG("MESSAGEQ ID = " << msgid);

	if (msgid == -1) {
		cerr << "Failed to create message queue" << endl;
	}
}


//TODO: Fix Bug
void MessageQueue::waitForMessage(){
	memset(&(this->message), 0, sizeof(this->message));
	msgrcv(this->msgid, &(this->message), sizeof(this->message), 0, 0);
}

void MessageQueue::sendMessage(const Message& message){
	msgsnd(msgid, &message, sizeof(message), 0);
}

MessageQueue::~MessageQueue(){

	if (msgctl(msgid, IPC_RMID, NULL) == -1) {
		ERROR("Failed to delete message queue (" << this->msgid <<") [Already Deleted]");
	}else{
		LOG("Message queue ("<< this->msgid <<") deleted");
	}
}

int MessageQueue::getLatestMessageID(){
	return this->message.id;
}
string MessageQueue::getLatestMessageString(){
	string rtnMessage = this->message.data;
	return rtnMessage;
}

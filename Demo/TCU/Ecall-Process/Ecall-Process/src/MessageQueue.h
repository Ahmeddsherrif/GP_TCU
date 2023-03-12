/*
 * MessageQueue.h
 *
 *  Created on: Mar 10, 2023
 *      Author: ahmed
 */

#ifndef MESSAGE_QUEUE_H_
#define MESSAGE_QUEUE_H_

#define DIR_NAME				"/home/ahmed/Desktop/MSGQ/"



#define ECALL_PROCESS_MSGQ	    "ECALL_PROCESS_MSGQ"
#define ECALL_PROCESS_MSGQ_KEY 	'A'


struct Message{
		int id;
		char data[100];
};

class MessageQueue {
	public:
		MessageQueue(const char *messageQueueName, char messageQueueKey);
		void waitForMessage();
		void sendMessage(const Message& message);
		int getLatestMessageID();
		string getLatestMessageString();
		~MessageQueue();

	private:
		int msgid;
		Message message;
};

#endif /* MESSAGE_QUEUE_H_ */

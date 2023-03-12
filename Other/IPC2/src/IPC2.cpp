#include <iostream>
#include <cstring>
#include <string>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>


#define MSG_QUEUE_NAME "ECALL_PROCESS_QUEUE"
#define MSG_QUEUE_KEY   0

#define MAX_MSG_SIZE 1024
#define MSG_TYPE 1

using namespace std;

// Function to initialize the message queue
int init() {
    key_t key;
    int msgid;

    // Generate a unique key for the message queue
    key = ftok(MSG_QUEUE_NAME, MSG_QUEUE_KEY);

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

int main() {
    int msgid = init();
    if (msgid == -1) {
        return 1;
    }

	while (1) {
		string message = receive(msgid);
		cout << "Received message: " << message << endl;

		if(message == "KILL"){
			break;
		}
	}


    // Delete the message queue
    if (msgctl(msgid, IPC_RMID, NULL) == -1) {
        cerr << "Failed to delete message queue" << endl;
        return 1;
    }
    cout << "Message queue deleted" << endl;

    return 0;
}

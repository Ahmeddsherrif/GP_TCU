#include <iostream>
#include <cstring>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string>

using namespace std;

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

// Function to send a message
int send(int msgid, const char *text) {
    // Send the message
    if (msgsnd(msgid, text, strlen(text) + 1, 0) == -1) {
        cerr << "Failed to send message" << endl;
        return -1;
    }

    return 0;
}

int main() {
    int msgid = init();
    string event;

    while (true) {
        cout << "Enter an Event: ";
        getline(cin, event);
        send(msgid, event.c_str());

        if (event == "KILL") {
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

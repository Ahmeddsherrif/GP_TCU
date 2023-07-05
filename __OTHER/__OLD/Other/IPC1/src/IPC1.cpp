#include <iostream>
#include <cstring>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string>

using namespace std;

#include "MessageQueue.h"


int main() {

	MessageQueue msgQ;

    while (true) {
        cout << "Enter an Event: ";
        getline(cin, event);
        send(msgid, event.c_str());

        if (event == "KILL") {
            break;
        }
    }


    return 0;
}

#include <iostream>
#include <string>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <sys/uio.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <fstream>
#include <sys/stat.h>

using namespace std;



void init(struct sockaddr_in &sendSockAddr, int &serverSd, int port) {
	// setup a socket and connection tools
	serverSd = socket(AF_INET, SOCK_STREAM, 0);
	memset(&sendSockAddr, 0, sizeof(sendSockAddr));
	sendSockAddr.sin_family = AF_INET;
	sendSockAddr.sin_addr.s_addr = INADDR_ANY;
	sendSockAddr.sin_port = htons(port);

	// bind the socket to the port number
	bind(serverSd, (struct sockaddr*) &sendSockAddr, sizeof(sendSockAddr));

	// wait for a connection
	listen(serverSd, 1);
}

void send_msg(int client, char *msg) {
	send(client, msg, strlen(msg), 0);
}


string event;

void handle_client(int client) {
	cout << "Enter an Event: ";
	getline(cin, event);
	send_msg(client, (char*) event.c_str());
}

int main(int argc, char *argv[]) {
	int serverSd = 0, clientSd = 0;
	struct sockaddr_in sendSockAddr;
	socklen_t sockAddrSize = sizeof(sendSockAddr);
	init(sendSockAddr, serverSd, 8888);

	cout << "Waiting for a client to connect..." << endl;

	// wait for a connection
	clientSd = accept(serverSd, (struct sockaddr*) &sendSockAddr,
			&sockAddrSize);

	// handle the client
	while (true) {

		handle_client(clientSd);

		if(event == "KILL"){
			break;
		}
	}

	// close the socket
	close(serverSd);

	return 0;
}


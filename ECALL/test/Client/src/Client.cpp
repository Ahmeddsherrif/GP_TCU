#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;

#define SERVER_ADDRESS "127.0.0.1"
#define SERVER_PORT 8888

#define MSG_BUFFER_SIZE 1024

int init_client(struct sockaddr_in &server_addr) {
    // create a socket for the client
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        cerr << "Error creating socket: " << strerror(errno) << endl;
        exit(EXIT_FAILURE);
    }

    // set the server address and port number
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);

    if (inet_pton(AF_INET, SERVER_ADDRESS, &server_addr.sin_addr) <= 0) {
        cerr << "Invalid server address" << endl;
        exit(EXIT_FAILURE);
    }

    // connect to the server
    if (connect(client_socket, (struct sockaddr*) &server_addr,
            sizeof(server_addr)) < 0) {
        cerr << "Error connecting to server: " << strerror(errno) << endl;
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    return client_socket;
}

void receive_msg(int server, char *buffer) {
	memset(buffer, 0, MSG_BUFFER_SIZE);
	int bytes_received = recv(server, buffer, MSG_BUFFER_SIZE, 0);
	if (bytes_received < 0) {
		cerr << "Error receiving message from server" << endl;
		exit(EXIT_FAILURE);
	}
}

int main() {
	int server_socket;
	struct sockaddr_in server_addr;
	char buffer[MSG_BUFFER_SIZE];

	string response;

	// initialize the client socket and connect to the server
	server_socket = init_client(server_addr);

	while (true) {
		// receive a message from the server
		receive_msg(server_socket, buffer);
		response = buffer;
		// print the message
		cout << "Server: " << response << endl;

		if(response == "KILL"){
			break;
		}
	}

	// close the socket
	close(server_socket);

	return 0;
}

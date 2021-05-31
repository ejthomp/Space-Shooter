#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <mutex>
#include <thread>
#include <iostream>
#include <chrono>


#define PORT 8001




using namespace std;

mutex playerCountMtx;
mutex player1PosMtx;
mutex player2PosMtx;


void Request (int client); 

float player1_xpos = 0;
float player1_ypos = 0;
float player1_zpos = 0;
float player2_xpos = 0;
float player2_ypos = 0;
float player2_zpos = 0;

int playercount = 0;


int main (int argc, char * argv []){
	int server_fd;
	struct sockaddr_in address;
	int opt = 1;
	int addrlen = sizeof(address);

	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0){
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);

	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) {
		perror("socket failed");
		exit(EXIT_FAILURE);
	}
	
	if (listen(server_fd, 3) < 0){
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

	while (true){
		int client = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
		thread t(Request, client);
		t.detach();
	}
}



void Request (int client) {
	char buffer [128] = {};
	bool accepted = false;
	string username = "";
	playerCountMtx.lock();
	int player = ++playercount;
	playerCountMtx.unlock();
	cout << player;
	fflush(stdout);
	while (true) {
		if (!accepted){
			recv(client, buffer, 128, 0);
			username = string(buffer);
			accepted = true;
		}
		else {
			recv(client, buffer, 1, MSG_WAITALL);
			buffer[1] = '\0';
			int selection = buffer[0] - '0';
			if (selection == 0)
			{
				if (player == 1)
				{
					recv(client, buffer, 1, MSG_WAITALL);
					buffer[1] = '\0';
					if (buffer[0] != 'x') {
						cout << "Error (x)" << endl;
						continue;
					}
					recv(client, buffer, 11, MSG_WAITALL);
					buffer[11] = '\0';

					player1PosMtx.lock();
					player1_xpos = atof(buffer);
					player1PosMtx.unlock();

					recv(client, buffer, 1, MSG_WAITALL);
					buffer[1] = '\0';
					if (buffer[0] != 'y') {
						cout << "Error (y)" << endl;
						continue;
					}
					recv(client, buffer, 11, MSG_WAITALL);
					buffer[11] = '\0';

					player1PosMtx.lock();
					player1_ypos = atof(buffer);
					player1PosMtx.unlock();

					recv(client, buffer, 1, MSG_WAITALL);
					buffer[1] = '\0';
					if (buffer[0] != 'z') {
						cout << "Error (z)" << endl;
						continue;
					}
					recv(client, buffer, 11, MSG_WAITALL);
					buffer[11] = '\0';

					player1PosMtx.lock();
					player1_zpos = atof(buffer);
					player1PosMtx.unlock();

					player2PosMtx.lock();
					string message = to_string(player2_xpos) + " " + to_string(player2_ypos) + " " + to_string(player2_zpos);
					player2PosMtx.unlock();

					send(client, message.c_str(), strlen(message.c_str()), 0);
				}
				else {
					recv(client, buffer, 1, MSG_WAITALL);
					buffer[1] = '\0';
					if (buffer[0] != 'x') {
						cout << "Error (x)" << endl;
						continue;
					}
					recv(client, buffer, 11, MSG_WAITALL);
					buffer[11] = '\0';

					player2PosMtx.lock();
					player2_xpos = atof(buffer);
					player2PosMtx.unlock();

					recv(client, buffer, 1, MSG_WAITALL);
					buffer[1] = '\0';
					if (buffer[0] != 'y') {
						cout << "Error (y)" << endl;
						continue;
					}
					recv(client, buffer, 11, MSG_WAITALL);
					buffer[11] = '\0';

					player2PosMtx.lock();
					player2_ypos = atof(buffer);
					player2PosMtx.unlock();

					recv(client, buffer, 1, MSG_WAITALL);
					buffer[1] = '\0';
					if (buffer[0] != 'z') {
						cout << "Error (z)" << endl;
						continue;
					}
					recv(client, buffer, 11, MSG_WAITALL);
					buffer[11] = '\0';

					player2PosMtx.lock();
					player2_zpos = atof(buffer);
					player2PosMtx.unlock();

					player1PosMtx.lock();
					string message = to_string(player1_xpos) + " " + to_string(player1_ypos) + " " + to_string(player1_zpos);
					player1PosMtx.unlock();
					send(client, message.c_str(), strlen(message.c_str()), 0);
				}
			}
		}
	}
}

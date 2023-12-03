#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib, "ws2_32")
#include <iostream>
#include <string>
#include <io.h>
#include <WinSock2.h>
#include "socket.h"
#include "board.h"

using namespace std;

void error_handling(const char* message);
void open_socket();
void set_my_adr();
void set_opp_adr();
void bind_and_listen_socket();
void connect_socket();
void accept_socket();

struct sockaddr_in;

int my_sock, opp_sock;
struct sockaddr_in my_adr;
struct sockaddr_in opp_adr;
int opp_adr_sz;

WSADATA wsaData;

void socket_start_routine() {

	if (WSAStartup(0x0202, &wsaData) != 0)
		error_handling("WSAStartup() error");

	open_socket();
}

void request_connect() {

	set_opp_adr();
	connect_socket();
}

void response_connect() {

	set_my_adr();
	bind_and_listen_socket();
	accept_socket();
}

void send_msg(string flag, string payload) {

	char msg[BUF_SIZE] = "\0";
	strcat(msg, flag.c_str());
	strcat(msg, "/");
	strcat(msg, payload.c_str());
	strcat(msg, "\0");

	send(opp_sock, msg, strlen(msg), 0);
}

void send_msg(string flag, point payload) {

	char msg[BUF_SIZE] = "\0";
	strcat(msg, flag.c_str());
	strcat(msg, "/");
	strcat(msg, (const char*)payload.x);
	strcat(msg, ",");
	strcat(msg, (const char*)payload.y);
	strcat(msg, "\0");

	send(opp_sock, msg, strlen(msg), 0);
}

string recv_msg() {

	char msg[BUF_SIZE];
	if (recv(opp_sock, msg, BUF_SIZE - 1, 0) == -1)
		error_handling("recv() error");

	char* msg_type = strtok(msg, "/");
	if (msg_type == "flag")
		return msg;
	else if (msg_type == "point")
		return "point";
}

void open_socket() {

	my_sock = socket(PF_INET, SOCK_STREAM, 0);
	if (my_sock == -1)
		error_handling("socket() error");
}

void set_my_adr() {

	memset(&my_adr, 0, sizeof(my_adr));
	my_adr.sin_family = PF_INET;
	my_adr.sin_addr.s_addr = htonl(INADDR_ANY);
	my_adr.sin_port = htons(100);
}

void set_opp_adr() {

	memset(&opp_adr, 0, sizeof(opp_adr));
	opp_adr.sin_family = PF_INET;
	opp_adr.sin_addr.s_addr = htonl(INADDR_ANY);
	opp_adr.sin_port = htons(101);
}

void bind_and_listen_socket() {

	if (bind(my_sock, (struct sockaddr*)&my_adr, sizeof(my_adr)) == -1)
		error_handling("bind() error");
	if (listen(my_sock, 5) == -1)
		error_handling("listen() error");
	opp_adr_sz = sizeof(opp_adr);
}

void connect_socket() {

	if (connect(opp_sock, (sockaddr*)&opp_adr, sizeof(opp_adr)) == -1)
		error_handling("connect() error");
}

void accept_socket() {

	opp_sock = accept(my_sock, (struct sockaddr*)&opp_adr, &opp_adr_sz);
	if (opp_sock == -1)
		error_handling("accept() error");
	else
		cout << "Client connected at: " << inet_ntoa(opp_adr.sin_addr) << ":" << ntohs(opp_adr.sin_port) << "\n";
}

void error_handling(const char* message) {
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
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
void open_my_socket();
void open_opp_socket();
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

// 소켓 초기화
void socket_start_routine() {

	if (WSAStartup(0x0202, &wsaData) != 0)
		error_handling("WSAStartup() error");
}

// 매칭에 참가하는 유저
// 소켓에 통신 요청을 보냄
void request_connect() {

	open_opp_socket();
	set_opp_adr();
	connect_socket();
}

// 매칭을 생성하는 유저
// 소켓에 요청된 연결을 수락함
void response_connect() {

	open_my_socket();
	set_my_adr();
	bind_and_listen_socket();
	accept_socket();
}

// 인자로 받은 메세지 타입과 페이로드를 포맷화해서 전송
// string 타입의 인자를 받음
void send_msg(string msg_type, string payload) {

	char msg[BUF_SIZE] = "\0";
	strcat(msg, msg_type.c_str());
	strcat(msg, "/");
	strcat(msg, payload.c_str());
	strcat(msg, "\0");

	send(opp_sock, msg, strlen(msg), 0);
}

// 인자로 받은 메세지 타입과 페이로드를 포맷화해서 전송
// point 타입의 인자를 받음
void send_msg(string msg_type, point payload) {

	char msg[BUF_SIZE] = "\0";
	sprintf(msg, "%s/%d,%d", msg_type.c_str(), payload.x, payload.y);

	send(opp_sock, msg, strlen(msg), 0);
}

// 메세지를 수신해서 파싱한 정보를 리턴
string* recv_msg() {

	char msg[BUF_SIZE];
	string* parse = new string[3];
	memset(msg, 0, BUF_SIZE);
	if (recv(opp_sock, msg, BUF_SIZE - 1, 0) == -1)
		error_handling("recv() error");
	
	// 메세지 타입을 판별
	char* msg_type = strtok(msg, "/");
	parse[0] = _strdup(msg_type);

	if (strcmp(msg_type, "flag") == 0) {	// 메세지 타입이 flag인 경우
		parse[1] = _strdup(strtok(NULL, " "));
	}
	else if (strcmp(msg_type, "point") == 0) {	// 메세지 타입이 point인 경우
		parse[1] = _strdup(strtok(NULL, ","));
		parse[2] = _strdup(strtok(NULL, " "));
	}
	else {	// 그 외의 경우는 error를 부여해서 예외처리
		parse[0] = "error";
	}

	return parse;
}

void open_my_socket() {

	my_sock = socket(PF_INET, SOCK_STREAM, 0);
	if (my_sock == -1)
		error_handling("socket() error");
}

void open_opp_socket() {

	opp_sock = socket(PF_INET, SOCK_STREAM, 0);
	if (opp_sock == -1)
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
	opp_adr.sin_addr.s_addr = inet_addr(HOST_IP);
	opp_adr.sin_port = htons(100);
}

void bind_and_listen_socket() {

	opp_adr_sz = sizeof(opp_adr);
	if (bind(my_sock, (struct sockaddr*)&my_adr, sizeof(my_adr)) == -1)
		error_handling("bind() error");
	if (listen(my_sock, 5) == -1)
		error_handling("listen() error");
}

void connect_socket() {

	if (connect(opp_sock, (sockaddr*)&opp_adr, sizeof(opp_adr)) == -1)
		error_handling("connect() error");
}

void accept_socket() {

	opp_sock = accept(my_sock, (struct sockaddr*)&opp_adr, &opp_adr_sz);
	if (opp_sock == -1) error_handling("accept() error");
}

void error_handling(const char* message) {
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
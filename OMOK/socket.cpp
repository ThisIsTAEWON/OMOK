#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib, "ws2_32");
#include <iostream>
#include <io.h>
#include <WinSock2.h>

#define BUF_SIZE 1024

void error_handling(const char* message);

struct sockaddr_in;

int my_sock, opp_sock;
struct sockaddr_in my_adr;
struct sockaddr_in opp_adr;
int opp_adr_sz;

char message[BUF_SIZE];
char buffer[BUF_SIZE];

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
	accept_socket();
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
		cout << "Client connected at: " << inet_ntoa(opp_adr.sin_addr) << ":" << ntohs(opp_adr.sin_port)) << "\n";
}

void error_handling(const char* message) {
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
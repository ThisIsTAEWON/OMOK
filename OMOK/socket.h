#include "board.h"
#pragma once
using namespace std;

#define HOST_IP "119.198.167.223"
#define BUF_SIZE 1024

void socket_start_routine();
void request_connect();
void response_connect();
void send_msg(string msg_type, string payload);
void send_msg(string msg_type, point payload);
string* recv_msg();
void error_handling(const char* message);
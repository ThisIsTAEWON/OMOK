#pragma once

#define BLACK "��"
#define WHITE "��"
#define CURSOR "��"

#define WIDTH 20
#define HEIGHT 12

typedef struct{
	int x, y;
} point;

void init_board(int width, int height, std::string init_turn);
void print_board(int width, int height);
void handle_board();
std::string init_turn();
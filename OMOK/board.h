#pragma once

#define BLACK "��"
#define WHITE "��"
#define CURSOR "��"

typedef struct{
	int x, y;
} point;

void init_board(int width, int height);
void print_board(int width, int height);
void handle_board();
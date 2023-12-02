#pragma once

#define BLACK "¡Ü"
#define WHITE "¡Û"
#define CURSOR "¡Ý"

typedef struct{
	int x, y;
} point;

void init_board(int width, int height);
void print_board(int width, int height);
void handle_board();
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <Windows.h>
#include <conio.h> // kbhit(), chget()
#include "ui.h"
#include "board.h"
#include "socket.h"
using namespace std;

void handle_menu();
void print_rules();

void start_routine() {

    // 콘솔 제목 변경
    SetConsoleTitle((LPCWSTR)"OMOK");
    handle_menu();
}

void print_logo() {

    char buf[SIZE];
    FILE* fp = fopen("logo.txt", "r");
    while (fgets(buf, SIZE, fp) != NULL)
        cout << buf;
    cout << "\n\n";
    fclose(fp);
}

void open_match() {

    socket_start_routine();
    system("cls");
    cout << "Opening match...\n";
    response_connect();

    string turn = init_turn();
    init_board(WIDTH, HEIGHT, turn);
    send_msg("flag", turn);
    
    system("cls");
    handle_board();
}

void join_match() {

    socket_start_routine();
    system("cls");
    cout << "Joining match...\n";
    request_connect();

    string init_turn(recv_msg());
    init_board(WIDTH, HEIGHT, init_turn);

    system("cls");
    handle_board();
}

void handle_menu() {

    string menu[4] = { "Open Match", "Join Match", "Rules", "Exit" };
    for(int i=0;i<4;i++) cout << "\n		" << menu[i];
    int cursor = 0;

    system("cls");
    print_logo();
    for (int i = 0; i < 4; i++) {
        if (i == cursor) cout << "                      -" << menu[i] << "-\n";
        else cout << "                       " << menu[i] << "\n";
    }
    cout << "\n		Press a to enter.";

    while (1) {        
        if (_kbhit()) {
            int command = _getch();
            if (command == 224) {
                command = _getch();
                if (command == 72 && cursor > 0)
                    cursor--;
                else if (command == 80 && cursor < 3)
                    cursor++;
            }
            else if (command == 'a') {
                if (cursor == 0)
                    open_match();
                else if (cursor == 1)
                    join_match();
                else if (cursor == 2)
                    print_rules();
                else if (cursor == 3)
                    exit(0);
            }
            system("cls");
            print_logo();
            for (int i = 0; i < 4; i++) {
                if (i == cursor) cout << "                      -" << menu[i] << "-\n";
                else cout << "                       " << menu[i] << "\n";
            }
            cout << "\n		Press a to enter.";
        }
    }
}

void win() {

    system("cls");
    char buf[SIZE];
    char t;
    FILE* fp = fopen("you_win.txt", "r");
    while (fgets(buf, SIZE, fp) != NULL)
        cout << buf;
    cout << "\n		Enter to restart.";
    cin.get(t);
    start_routine();
}

void print_rules() {

    char buf[SIZE];
    system("cls");
    FILE* fp = fopen("rule_book.txt", "r");
    while (fgets(buf, SIZE, fp) != NULL)
        cout << buf;
    cout << "\n		Press any key to quit.";
    char t;
    t = _getch();
}
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
    SetConsoleTitle(TEXT("OMOK"));
    handle_menu();
}

// 로고 출력
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

    // 호스트 측에서 기물 색 결정
    // 상대에게 flag 타입의 메세지로 기물 색 정보 송신
    string turn = init_turn();
    set_my_turn(turn);
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

    // 호스트가 보낸 기물 색 정보를 담은 메세지 수신
    string* msg = recv_msg();
    if (strcmp(msg[0].c_str(), "flag") != 0) error_handling("join_match() error");
    
    // 수신한 정보 기반으로 기물 색 결정
    string my_turn;
    if (msg[1] == BLACK) my_turn = WHITE;
    else if (msg[1] == WHITE) my_turn = BLACK;

    set_my_turn(my_turn);
    init_board(WIDTH, HEIGHT, msg[1]); // msg[1]는 payload

    system("cls");
    handle_board();
}

// 메뉴화면 출력
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
    cout << "\n		    Press a to enter.";

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
            cout << "\n		    Press a to enter.";
        }
    }
}

// 승리시 화면 출력
void win() {

    system("cls");
    char buf[SIZE];
    char t;
    FILE* fp = fopen("you_win.txt", "r");
    while (fgets(buf, SIZE, fp) != NULL)
        cout << buf;
    cout << "\n		Press any key to restart.";
    t = _getch();
    start_routine();
}

// 패배시 화면 출력
void lose() {

    system("cls");
    char buf[SIZE];
    char t;
    FILE* fp = fopen("you_lose.txt", "r");
    while (fgets(buf, SIZE, fp) != NULL)
        cout << buf;
    cout << "\n		Press any key to restart.";
    t = _getch();
    start_routine();
}

// 룰 출력
void print_rules() {

    char buf[SIZE];
    char t;
    system("cls");
    FILE* fp = fopen("rule.txt", "r");
    while (fgets(buf, SIZE, fp) != NULL)
        cout << buf;
    cout << "\n		Press any key to quit.";
    t = _getch();
}
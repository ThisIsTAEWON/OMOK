#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <Windows.h>
#include <conio.h> // kbhit(), chget()
#include "board.h"
#include "ui.h"
#include "socket.h"
using namespace std;


string board[100][100];
string my_turn;
string turn;

void handle_my_turn(point* p, string tmp);
void handle_opp_turn(point* p);

bool on_board(point p);
bool is_placeable(point p);
void move(point* p, int dir);
void init_point(point* p);
void five_in_a_row(point p);
bool is_double_three(point p);
bool is_serial_horizontal(point p, int length);
bool is_serial_vertical(point p, int length);
bool is_serial_increase(point p, int length);
bool is_serial_decrease(point p, int length);
int* set_offset(int dir);

// 보드를 초기화 하는 함수
void init_board(int width, int height, string init_turn) {

    turn = init_turn;

    board[0][0] = "┌";
    for (int x = 1; x < width - 1; x++)
        board[0][x] = "┬";
    board[0][width - 1] = "┐";

    for (int y = 1; y < height - 1; y++) {
        board[y][0] = "├";
        for (int x = 1; x < width - 1; x++)
            board[y][x] = "┼";
        board[y][width - 1] = "┤";
    }

    board[height - 1][0] = "└";
    for (int x = 1; x < width - 1; x++)
        board[height - 1][x] = "┴";
    board[height - 1][width - 1] = "┘";
}

// 돌의 색을 랜덤하게 반환하는 함수
string init_turn() {

    srand((unsigned int)time(NULL));
    int rand_num = rand() % 2;

    if (rand_num == 0) return BLACK;
    else return WHITE;
}

void set_my_turn(string turn) {

    my_turn = turn;
}

// 보드를 출력
void print_board(int width, int height) {

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            cout << board[i][j];
        }
        cout << '\n';
    }
    cout << "            turn: " << turn;
}

// 선, 후공 여부에 따라 턴 순서 지정
void handle_board() {

    point* p = new point;
    // 보드의 중앙에서 커서가 나타난다
    p->x = WIDTH / 2;
    p->y = HEIGHT / 2;
    string tmp = board[p->y][p->x];

    if (my_turn == BLACK) { // 흑돌일 경우 선공
        board[p->y][p->x] = CURSOR;
        print_board(WIDTH, HEIGHT);
        while (1) {
            handle_my_turn(p, tmp);
            handle_opp_turn(p);
        }
    }
    else {  // 백돌일 경우 후공
        print_board(WIDTH, HEIGHT);
        while (1) {
            handle_opp_turn(p);
            handle_my_turn(p, tmp);
        }
    }
}

// 사용자로부터 키 입력을 받음
// 입력이 방향키일 경우 -> 해당 방향으로 커서를 이동
// 입력이 a일 경우 -> 1. 기물의 충돌, 위치의 유효성 검사
//                    2. 해당 위치에 기물 배치
//                    3. 위치 정보를 상대 유저에게 전송
//                    4. 턴 종료  
void handle_my_turn(point* p, string tmp) {

    while (1) {
        if (_kbhit()) {
            board[p->y][p->x] = tmp;
            int dir = _getch();
            if (dir == 'a') {   // a를 입력받으면 기물 배치
                board[p->y][p->x] = turn;
                send_msg("point", *p);
                five_in_a_row(*p);
                //if (is_double_three(p)) continue;

                // 턴 전환
                if (turn == BLACK) turn = WHITE;
                else if (turn == WHITE) turn = BLACK;

                system("cls");
                print_board(WIDTH, HEIGHT);
                break;
            }
            else if (dir == 224) {  // 방향키를 입력받으면 커서를 이동
                dir = _getch();
                move(p, dir);
                tmp = board[p->y][p->x];
                board[p->y][p->x] = CURSOR;

                system("cls");
                //cout << "time: " << t << "\n";
                print_board(WIDTH, HEIGHT);
            }
        }
    }
}

// 상대의 메세지를 수신
// 메세지의 타입이 flag인 경우 -> 차후 구현
// 메세지의 타입이 point인 경우 -> 기물의 위치 정보를 로컬에 반영함
void handle_opp_turn(point* p) {
    
    string* msg = recv_msg();
    if (msg[0] == "error")
        error_handling("opp_turn() error");
    else if (msg[0] == "flag")
        cout << "flag!\n"; // timeout 등 추가 기능
    else if (msg[0] == "point") {
        p->x = stoi(msg[1]);
        p->y = stoi(msg[2]);
        board[p->y][p->x] = turn;
        five_in_a_row(*p);

        // 턴 전환
        if (turn == BLACK) turn = WHITE;
        else if (turn == WHITE) turn = BLACK;

        init_point(p);
        board[p->y][p->x] = CURSOR;
        system("cls");
        //printf("time: %d\n", t);
        print_board(WIDTH, HEIGHT);
    }
}

// 커서를 이동하는 함수
// 이동할 수 없다면 이동하지 않음
// 이동할 위치에 기물이 있다면, 기물이 없을 때까지 이동
void move(point* p, int dir) {

    point tmp = *p;
    int* offset = set_offset(dir);
    p->x += offset[0];
    p->y += offset[1];
    if (!on_board(*p)) {
        p->x = tmp.x;
        p->y = tmp.y;
        return;
    }

    while (!is_placeable(*p)) {
        offset = set_offset(dir);
        p->x += offset[0];
        p->y += offset[1];
        if (!on_board(*p)) {
            p->x = tmp.x;
            p->y = tmp.y;
            return;
        }
    }
}

void init_point(point* p) {

    point tmp;
    for (int y = p->y; y < HEIGHT; y++) {
        for (int x = p->x + 1; x < WIDTH; x++) {
            tmp.x = x;
            tmp.y = y;
            if (is_placeable(tmp)) {
                p->x = tmp.x;
                p->y = tmp.y;
                return;
            }
        }
    }

    for (int y = p->y; y >= 0; y--) {
        for (int x = p->x - 1; x >= 0; x--) {
            tmp.x = x;
            tmp.y = y;
            if (is_placeable(tmp)) {
                p->x = tmp.x;
                p->y = tmp.y;
                return;
            }
        }
    }
}

// 보드 안의 좌표인지 판별
bool on_board(point p) {

    if (p.x >= 0 && p.x < WIDTH && p.y >= 0 && p.y < HEIGHT)
        return true;
    return false;
}

// 좌표에 기물이 없는지 판별
bool is_placeable(point p) {

    if (board[p.y][p.x] != BLACK && board[p.y][p.x] != WHITE)
        return true;
    return false;
}

// 4방향으로 오목인지 판별
// 자신의 턴일 때 5목이면 승리
// 상대의 턴일 때 5목이면 패배
void five_in_a_row(point p) {

    if (is_serial_horizontal(p, 5)
        || is_serial_vertical(p, 5)
        || is_serial_increase(p, 5)
        || is_serial_decrease(p, 5)) {
        for (int i = 0; i < 3; i++) {
            Sleep(500);
            system("cls");
            Sleep(500);
            print_board(WIDTH, HEIGHT);
        }
        if (turn == my_turn)
            win();
        else
            lose();
    }
}

//bool is_double_three(point p) {
//
//    int three_count = 0;
//
//    if (is_serial_horizontal(p, 3)) three_count++;
//    if (is_serial_vertical(p, 3)) three_count++;
//    if (is_serial_increase(p, 3)) three_count++;
//    if (is_serial_decrease(p, 3)) three_count++;
//
//    if (three_count >= 2) return true;
//    return false;
//}


// 가로로 length만큼 연속인지 판별
bool is_serial_horizontal(point p, int length) {

    int piece_count = 0;

    for (int i = p.x - (length - 1); i <= p.x + (length - 1); i++) {
        if (i >= 0 && i < WIDTH && board[p.y][i] == turn) piece_count++;
        else piece_count = 0;

        if (piece_count == length) return true;
    }

    return false;
}

// 세로로 length만큼 연속인지 판별
bool is_serial_vertical (point p, int length) {

    int piece_count = 0;

    for (int i = p.y - (length - 1); i <= p.y + (length - 1); i++) {
        if (i >= 0 && i < HEIGHT && board[i][p.x] == turn) piece_count++;
        else piece_count = 0;

        if (piece_count == length) return true;
    }

    return false;
}

// 우상향으로 length만큼 연속인지 판별
bool is_serial_increase (point p, int length) {

    int piece_count = 0;

    point tmp;
    tmp.y = p.y + (length - 1);
    tmp.x = p.x - (length - 1);

    while (tmp.y >= p.y - (length - 1) && tmp.x <= p.x + (length - 1)) {
        if (on_board(tmp) && board[tmp.y][tmp.x] == turn) piece_count++;
        else piece_count = 0;

        if (piece_count == length) return true;

        tmp.y--;
        tmp.x++;
    }

    return false;
}

// 우하향으로 length만큼 연속인지 판별
bool is_serial_decrease(point p, int length) {

    int piece_count = 0;

    point tmp;
    tmp.y = p.y - (length - 1);
    tmp.x = p.x - (length - 1);

    while (tmp.y <= p.y + (length - 1) && tmp.x <= p.x + (length - 1)) {
        if (on_board(tmp) && board[tmp.y][tmp.x] == turn) piece_count++;
        else piece_count = 0;

        if (piece_count == length) return true;

        tmp.y++;
        tmp.x++;
    }

    return false;
}

int* set_offset(int dir) {

    int offset[2];
    if (dir == 72) {
        offset[0] = 0;
        offset[1] = -1;
    }
    else if (dir == 77) {
        offset[0] = 1;
        offset[1] = 0;
    }
    else if (dir == 80) {
        offset[0] = 0;
        offset[1] = 1;
    }
    else if (dir == 75) {
        offset[0] = -1;
        offset[1] = 0;
    }
    else {
        offset[0] = 0;
        offset[1] = 0;
    }

    return offset;
}
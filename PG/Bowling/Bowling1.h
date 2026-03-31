#pragma once
#include <stdio.h>
#include <string.h>

#define FRAMES 10  // 총 10프레임

/* ===================== 구조체 ===================== */

typedef struct {
    int rolls[3];      // 각 프레임의 투구 점수 (3번째 투구는 10프레임 스트라이크/스페어용)
    int roll_count;    // 해당 프레임 투구 수
} Frame;

typedef struct {
    char name[10];         // 플레이어 이름 (최대 9글자 + null)
    Frame frames[FRAMES];  // 10프레임
    int cumulative[FRAMES]; // 프레임별 누적 점수
} Player;

/* ===================== 함수 선언 ===================== */

void show_menu();                   // 메뉴 출력
int input_roll(int max);            // 투구 점수 입력
void play_game(Player *p);          // 게임 진행
void calculate_score(Player *p);    // 점수 계산
void print_board(Player *p);        // 점수판 출력

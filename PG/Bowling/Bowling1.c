#include <stdio.h>
#include "Bowling1.h"

/* 메뉴 출력 함수 */
void show_menu() {
    printf("\n=== 볼링 게임 시스템 ===\n");
    printf("1. 게임 플레이\n");
    printf("2. 내 기록 보기\n");
    printf("3. 최고 점수 보기\n");
    printf("4. 월별 통계 보기\n");
    printf("0. 종료\n");
    printf("선택: ");
}

/* 점수 입력 함수 */
int input_roll(int max) {
    int score;
    while (1) {
        scanf("%d", &score);
        if (score >= 0 && score <= max)
            return score;
        printf("유효한 점수를 입력하세요 (0-%d): ", max);
    }
}

/* 게임 플레이 함수 */
void play_game(Player *p) {
    printf("\nPlayer name: ");
    scanf("%s", p->name);

    for (int i = 0; i < FRAMES; i++) {
        p->frames[i].roll_count = 0;
        p->cumulative[i] = 0;
        for (int j = 0; j < 3; j++)
            p->frames[i].rolls[j] = 0;
    }

    for (int i = 0; i < FRAMES; i++) {
        Frame *f = &p->frames[i];

        printf("\n%d frame 1 cast : ", i + 1);
        f->rolls[0] = input_roll(10);
        f->roll_count++;

        if (f->rolls[0] == 10 && i != 9) continue;

        printf("%d frame 2 cast : ", i + 1);
        f->rolls[1] = input_roll(10 - f->rolls[0]);
        f->roll_count++;

        if (i == 9 && (f->rolls[0] + f->rolls[1] >= 10)) {
            printf("%d frame 3 cast : ", i + 1);
            f->rolls[2] = input_roll(10);
            f->roll_count++;
        }
    }

    calculate_score(p);
    print_board(p);

    printf("\n게임이 종료되었습니다!\n최종 점수: %d\n", p->cumulative[9]);
}

/* 점수 계산 함수 */
void calculate_score(Player *p) {
    int total = 0;

    for (int i = 0; i < FRAMES; i++) {
        Frame *f = &p->frames[i];

        if (f->rolls[0] == 10) {  // 스트라이크
            total += 10;
            if (i + 1 < FRAMES)
                total += p->frames[i + 1].rolls[0];

            if (i + 1 < FRAMES && p->frames[i + 1].rolls[0] == 10 && i + 2 < FRAMES)
                total += p->frames[i + 2].rolls[0];
            else if (i + 1 < FRAMES)
                total += p->frames[i + 1].rolls[1];
        } else if (f->rolls[0] + f->rolls[1] == 10) {  // 스페어
            total += 10;
            if (i + 1 < FRAMES)
                total += p->frames[i + 1].rolls[0];
        } else {  // 일반 점수
            total += f->rolls[0] + f->rolls[1];
        }

        p->cumulative[i] = total;
    }
}

/* 점수판 출력 함수 */
void print_board(Player *p) {
    printf("\nPlayer name: %s\n", p->name);

    printf("-------------------------------------------\n");
    printf("| 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10  |\n");
    printf("-------------------------------------------\n");

    printf("|");
    for (int i = 0; i < FRAMES; i++) {
        Frame f = p->frames[i];

        if (f.rolls[0] == 10) {
            printf("X| ");
        } else {
            printf("%d|", f.rolls[0]);

            if (f.rolls[0] + f.rolls[1] == 10)
                printf("/| ");
            else
                printf("%d| ", f.rolls[1]);
        }
    }
    printf("\n-------------------------------------------\n");

    printf("|");
    for (int i = 0; i < FRAMES; i++) {
        if (p->cumulative[i] == 0)
            printf("   |");
        else
            printf("%3d|", p->cumulative[i]);
    }
    printf("\n-------------------------------------------\n");
}
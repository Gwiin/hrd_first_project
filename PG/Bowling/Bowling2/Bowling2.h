#pragma once
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct{
    int score[12][3];
    int frameScore[11];
    char name[10];
} Score;

void initScore(Score *player, const char name[]);
void playGame(Score *player, int i);
void playGame10Frame(Score *player);
void printBoard(Score *player);
void setScore(int frame, int bow, Score *player);
void setFrameScore(int frame, Score *player);
void calScore(int *frameScore, const int (*score)[3]);
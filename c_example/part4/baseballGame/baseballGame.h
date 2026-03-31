#pragma once
// #ifndef BASEBALL_H
// #define BASEBALL_H

#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<stdbool.h>
#include "baseballGame.h"
#define SIZE 3

void generate_number(int* question);
void input_number(int* answer);
bool check_result(const int* question, const int* answer, int* strike, int* ball);

// #endif BASEBALL_H
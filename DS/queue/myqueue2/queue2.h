#pragma once
#include <stdio.h>
#include <stdlib.h>

#define ARRAYSIZE 100

typedef struct {
    int queue[ARRAYSIZE];
    int *front;
    int *rear;
} Qu;

void initQueue(Qu *pQu);
void push(Qu *pQu, int data);
int pop(Qu *pQu);
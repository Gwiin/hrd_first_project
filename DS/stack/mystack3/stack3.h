#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

typedef struct
{
    int *pArr;
    int *tos;
    int size;
} Stack;
void initStack(Stack *ps, int size);
void cleanupStack(Stack *ps);

void push(Stack *ps, int data);
int pop(Stack *ps);
int size(Stack *ps);
#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

typedef struct
{
    void *pArr;
    int eleSize;
    int *tos;
    int size;
} Stack;

void initStack(Stack *ps, int size, int eleSize);
void cleanupStack(Stack *ps);

void push(Stack *ps, const void *pData);
void pop(Stack *ps, void *pData);
int size(Stack *ps);
#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

typedef struct
{
    void *pArr;
    int eleSize;
    void *front;
    void *rear;
    int size;
} Qu;

void initQueue(Qu *pQu, int size, int elesize);
void cleanupQueue(Qu *pQu);
void push(Qu *pQu, const void *pData);
void pop(Qu *pQu, void *pData);
int size(Qu *pQu);
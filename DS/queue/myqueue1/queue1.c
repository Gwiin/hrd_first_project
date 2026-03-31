#include "queue1.h"


void initQueue(Qu *pQu)
{
    pQu->front = pQu->queue;
    pQu->rear = pQu->queue;
}
void push(Qu *pQu, int data)
{
    if(pQu->rear == pQu->queue + ARRAYSIZE){
        pQu->rear = pQu->queue;
    }
    if(pQu->rear+1 == pQu->front){
        fprintf(stderr, "stack is full\n");
        exit(1);
    }
    *(pQu->rear++) = data;
}
int pop(Qu *pQu)
{
    if(pQu->front == pQu->queue + ARRAYSIZE){
        pQu->front = pQu->queue;
    }
    if(pQu->front == pQu->rear){
        fprintf(stderr, "there is no data\n");
        exit(1);
    }
    return *(pQu->front++);
}
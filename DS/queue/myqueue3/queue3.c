#include "queue3.h"

void initQueue(Qu *pQu, int size, int eleSize)
{
    pQu->pArr = malloc(eleSize*size);
    assert(pQu->pArr != NULL);
    pQu->eleSize = eleSize;
    pQu->size = size;
    pQu->front = pQu->pArr;
    pQu->rear = pQu->pArr;
}
void cleanupQueue(Qu *pQu)
{
    free(pQu->pArr);
}
void push(Qu *pQu, const void *pData)
{
    assert(pQu->rear != (pQu->pArr + pQu->size));
    memcpy((unsigned char *)pQu->rear, pData, pQu->eleSize);
    pQu->rear = (unsigned char *)pQu->rear + pQu->eleSize;
}
void pop(Qu *pQu, void *pData)
{
    assert(pQu->front != pQu->rear);
    memcpy(pData, (unsigned char *)pQu->front, pQu->eleSize);
    pQu->front = (unsigned char *)pQu->front + pQu->eleSize;
}
int size(Qu *pQu)
{
    return pQu->size;
}
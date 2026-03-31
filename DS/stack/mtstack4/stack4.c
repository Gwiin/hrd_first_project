#include "stack4.h"

void cleanupStack(Stack *ps)
{
    free(ps->pArr);
}

int size(Stack *ps)
{
    return ps->size;
}

void initStack(Stack *ps, int size, int eleSize)
{
    ps->pArr = (int *)malloc(eleSize * size);
    assert(ps->pArr);

    ps->eleSize = eleSize;
    ps->size = size;
    ps->tos = ps->pArr;
}

void push(Stack *ps, const void *pData)
{
    assert(ps->tos != (ps->pArr + ps->size));
    memcpy((unsigned char *)ps->tos, pData, ps->eleSize);
    ps->tos = (unsigned char *)ps->tos + ps->eleSize;
}

void pop(Stack *ps, void *pData)
{
    assert(ps->tos != ps->pArr);
    ps->tos = (unsigned char *)ps->tos - ps->eleSize;
    memcpy(pData, (unsigned char *)ps->tos, ps->eleSize);
}
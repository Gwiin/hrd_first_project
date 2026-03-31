#include "stack3.h"

void cleanupStack(Stack *ps)
{
    free(ps->pArr);
}

int size(Stack *ps)
{
    return ps->size;
}

void initStack(Stack *ps, int size)
{
    ps->pArr = (int *)malloc(sizeof(int) * size);
    assert(ps->pArr);
    ps->size = size;
    ps->tos = ps->pArr;
}
void push(Stack *ps, int data)
{
    assert(ps->tos != (ps->pArr + ps->size));
    *(ps->tos++) = data;
}

int pop(Stack *ps)
{
    assert(ps->tos != ps->pArr);
    return *(--ps->tos);
}
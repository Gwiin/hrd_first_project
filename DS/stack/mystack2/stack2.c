#include "stack2.h"

void initStack(Stack *ps)
{
    ps->tos = ps->array;
}
void push(Stack *ps, int data)
{
    // *ps->tos = data;
    // ++ps->tos;
    *(ps->tos++) = data;
}

int pop(Stack *ps)
{
    // --ps->tos;
    // return *ps->tos;
    return *(--ps->tos);
}
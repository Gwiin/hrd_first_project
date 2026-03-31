#include <stdio.h>

int add(int a, int b);

int substract(int a, int b);

int main(void)
{
    int(*fp)(int, int);
    fp = add;
    int re = (*fp)(4, 3);
    re = fp(4,3);
    printf("결과 : %d\n", re);

    fp = &substract;
    re = (*fp)(4, 3);
    printf("결과 : %d\n", re);
    return 0;
}

int add (int a, int b)
{
    return a + b;
}
int substract(int a, int b)
{
    return a - b;
}
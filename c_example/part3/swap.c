#include <stdio.h>
void swap(int *a, int *b);
void info(int a, int b);

int main()
{
    int a, b, tmp;
    a= 100;
    b= 200;

    printf("a: %d, b: %d\n", a, b);
    //tmp = a;
    //a = b;
    //b = tmp;
    swap(&a, &b);
    printf("a: %d, b: %d\n", a, b);

    return 0;
}

void swap(int *a, int *b)
{
    int tmp;
    tmp = *a;
    *a = *b;
    *b = tmp;
}

void info(int a, int b)
{
    printf("a: %d, b: %d\n", a, b);
}
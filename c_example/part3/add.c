#include <stdio.h>

int add(int a, int b);

int main(void){
    int first = 43, second = 72;
    printf("%d + %d = %d\n", first, second, add(first, second));
    return 0;
}

int add(int a, int b)
{
    return a + b;
}
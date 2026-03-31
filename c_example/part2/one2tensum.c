#include <stdio.h>

int main()
{
    int sum = 0;

    for (int i = 0; i < 10; ++i)
    {
       sum = sum + i + 1;
    }
    printf("1 부터 10 까지의 합은 : %d\n", sum);
    return 0;
}
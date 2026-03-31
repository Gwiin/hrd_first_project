#include <stdio.h>

int main(void)
{
    int i, j;
    int n = 3; // 위쪽 피라미드 줄 수

    // 위쪽 피라미드
    for(i = 1; i <= n; i++)
    {
        for(j = 1; j <= n-i; j++) // 공백
            printf(" ");
        for(j = 1; j <= 2*i-1; j++) // 별
            printf("*");
        printf("\n");
    }

    // 아래쪽 피라미드
    for(i = n-1; i >= 1; i--)
    {
        for(j = 1; j <= n-i; j++) // 공백
            printf(" ");
        for(j = 1; j <= 2*i-1; j++) // 별
            printf("*");
        printf("\n");
    }

    return 0;
}
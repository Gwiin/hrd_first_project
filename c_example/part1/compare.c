#include <stdbool.h>
#include <stdio.h>

int main(void)
{
    int a, b;
    bool greater = false;
     
    printf("두 정수를 입력하세요 : ");
    scanf("%d %d", &a, &b);
    greater = a > b;
    printf("greater 의 값은 % d 이다", greater);
    if(greater == true)
    {
        printf("a가 더 크다.");
    }
    else
    {
        printf("a가 더 작다.");
    }

    return 0;
}
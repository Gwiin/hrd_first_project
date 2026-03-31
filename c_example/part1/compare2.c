#include <stdio.h>

int main(void)
{
    int num;
    printf("숫자를 입력하세요 1~9 : ");
    scanf("%d", &num);

    // printf("1 <= %d <= 9 : %d\n", num, (1 <= num) && (num <= 9)
    printf("1 <= %d <= 9 : %s\n", num, (1 <= num) && (num <= 9) ? "true": "false");
    return 0;
}
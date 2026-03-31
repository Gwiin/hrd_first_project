// a ~ b 까지의 합을 출력하세요. for 문을 사용해서

#include <stdio.h>

int main(void)
{
    int a, b;
    int sum = 0;
    printf("a, b 입력하세요: ");
    scanf("%d %d", &a, &b);

    for (int i = a; i <= b; i++)
    {
        sum += i;
    }
    printf("%d ~ %d 까지의 합은 : %d\n", a, b, sum);
    return 0;
}
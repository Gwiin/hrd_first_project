// Fahr 온도값을 넣어서  celsius 온도값으로 변환하도록 코드를 작성하세요

#include <stdio.h>

int main()
{
    int fahr;
    printf("온도 를 넣으세요: ");
    scanf("%d", &fahr);
    double celsius;
    celsius = 5.0 / 9.0 * (double)(fahr - 32);
    printf("C(celsius) 온도 : %.2f\n", celsius);
    printf("F(fahr) 온도 : %d\n", fahr);
    return 0;
}
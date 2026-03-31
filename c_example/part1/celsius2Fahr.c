#include <stdio.h>

int main()
{
    int celsius;
    printf("C 온도 를 넣으세요: \n");
    scanf("%d", &celsius);
    double fahr;
    fahr = 9.0 / 5.0 * (double)celsius + 32.0;
    printf("C(celsius) 온도 : %d\n", celsius);
    printf("F(fahr) 온도 : %.2f\n", fahr);
    return 0;
}
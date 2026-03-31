//4.0과 1.2를 덧셈, 뺄셈, 곱셈 나눗셈한 값을 소수점 이하 첫째자리까지 출력되도록!

#include <stdio.h>

int main(void)
{
    double a = 4.0, b = 1.2;



    printf("%f + %f = %.1f\n", a, b, a + b);
    printf("%f + %f = %.1f\n", a, b, a - b);
    printf("%f * %f = %.1f\n", a, b, a * b);
    printf("%f / %f = %.1f\n", a, b, a / b);

    return 0;
}
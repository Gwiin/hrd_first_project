#include <stdio.h>

int main (void)
{
    double length;
    double height;
    double area;

    printf("삼각형의 길이 와 높이를 넣으세요 : ");
    scanf("%lf %lf", &length, &height);
    area = length * height / 2;
    printf("삼각형의 넓이는 : %.2f\n", area);
    return 0;
}
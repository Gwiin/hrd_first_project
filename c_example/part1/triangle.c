#include <stdio.h>

int main (void)
{
    int length;
    int height;
    int area;

    printf("삼각형의 길이 와 높이를 넣으세요 : ");
    scanf("%d %d", &length, &height);
    area = length * height / 2;
    printf("삼각형의 넓이는 : %d\n", area);
      
    return 0;
}
// 키가 187.5 이상 몸무게 80.0 미만이면 ok, 그 이외에는 cancel
// 키는 179.5 몸무게 75.0 double 변수 초기화

#include <stdio.h>

int main(void)
{
    int height = 179.5;
    int weight = 75.0;

    if((height >= 187.5) && (weight < 80.0))
    {
        printf("ok\n");
    } else
    {
        printf("cancel\n");
    }
    return 0;
}
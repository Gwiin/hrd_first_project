#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(void)
{
    int dice;
    srand(time(NULL));
    dice = rand() % 6 + 1;
    printf("주사위의 숫자는 %d입니다.\n", dice);
    return 0;
}
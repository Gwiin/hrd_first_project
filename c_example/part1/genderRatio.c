#include <stdio.h>

int main (void)
{

    int man, woman;
    double sum, manRate, womanRate;

    printf("남자의 수는 : ");
    scanf("%d", &man);
    printf("여자의 수는 : ");
    scanf("%d", &woman);

    sum = man + woman;
    womanRate = woman / sum * 100;
    manRate = man / sum * 100;

    printf("남자의 수는 %d명이고 여자의 수는 %d명이다. \n", man, woman);
    printf("총 수는 %.f명\n남자의 비율은 %.2f\n여자의비율은 %.2f\n", sum, manRate, womanRate);
    
    return 0;
}
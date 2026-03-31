#include <stdalign.h>

int main (void)
{
    int num;
    printf("숫자를 넣으세요: ");
    scanf("%d", &num);

    int isodd;

    //isodd = (num % 2 ==1);
    isodd = num % 2;
    
    printf("숫자는: %d \t is isodd: %d\n", num, isodd);
    if (isodd)
    {
        printf("숫자는: %d \t 홀수입니다.\n", num);
    }
    else
    {
        printf("숫자는: %d \t 짝수입니다.\n", num);
    }
    return 0;

}
#include <stdio.h>

int main()
{
    char ch1 = 'A'; //선언과 동시에 대입
    char ch2 = 64;
    char ch3; // 선언만
    printf("ch1: %c Number: %d\n", ch1, ch1);
    printf("ch2: %c Number: %d\n", ch2, ch2);
    scanf("%c", &ch3);
    printf("ch3: %c Number: %d\n", ch3, ch3);
    return 0;
    
}
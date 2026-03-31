// 아스키 코드 값을 출력하는 프로그램이 완성하도록 빈칸에 알맞은 코드를 적으시오

#include <stdio.h>

int main(void)
{
    char ch;

    printf("문자 입력 : ");
    scanf("%c", &ch);
    printf("%c문자의 아스키 코드 값은 %d입니다.",ch, ch);
    return 0;
}
//대소문자 확인

#include <stdio.h>
#include <stdbool.h>

int main(void)
{
    char ch;
    printf("char 를 입력하세요.");
    scanf("%c", &ch);

    bool isBig;
    isBig = ('A' <= ch && ch <='Z');
    printf("%c 는 %s\n", ch, isBig ? "대문자 입니다" : "대문자가 아닙니다");
    return 0;
}
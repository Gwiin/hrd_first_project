//입력받은 char 가 알파벳인지 아닌지 출력

#include <stdio.h>

int main(void)
{
    char ch;
    printf("char를 입력하세요.:");
    scanf("%c", &ch);
    if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'))
    {
        printf("%c는 알파벳입니다.\n", ch);
    }
    else
    {
        printf("%c는 알파벳이 아닙니다.\n", ch);
    }
    return 0;
}
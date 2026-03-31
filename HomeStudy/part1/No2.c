#include <stdio.h>

int main(void)
{
    printf("Be happy\n");
    printf("12345678901234567890\n"); //열 번호 출력
    printf("My\tfriend\n");           //my를 출력하고 탭 위치 이동(\t) friend 출력
    printf("Goot\bd\tchance\n");      //t를 d 로 바꾸고 탭 위치 이동 goot 출력 chance 출력 줄바꿈
    printf("cow\rW\n");                //cow를 wow롤 바꾸고 줄바꿈 출력

    return 0;
}
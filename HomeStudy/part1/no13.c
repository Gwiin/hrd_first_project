//국어 영어 수학 점수 저장 변수 kor eng mat를 선언 각각 70 80 90 초기화
//총점 저장변수 tot 선언, 세과목 합, 세과목 점수 '총점' 출력


#include <stdio.h>

int main(void)
{
    int kor = 70;
    int eng = 80;
    int mat = 90;
    int tot;

    tot = kor + eng + mat;

    printf("국어 : %d\n", kor);
    printf("영어 : %d\n", eng);
    printf("수학 : %d\n", mat);
    printf("총점 : %d", tot);
    return 0;
}
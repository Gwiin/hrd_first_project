// 전체 학점과 평점을 계산해 신청 학점이 10학점이상이고, 
// 평점 평균이 4.0을 넘는 경우 1을 출력하고, 그렇지 않으면 0을 출력 하는 프로그램 작성
// 국어-3학점-3.8 kor, kscore
// 영어-5학점-4.4 eng, escore
// 수학-4학점-3.9 mat, mscore
// 전체 학점 credit, 평점 평균grade

#include <stdio.h>

int main(void)
{
    int kor = 3, eng = 5, mat = 4;
    int credit;
    int res;
    double kscore = 3.8, escore = 4.4, mscore = 3.9;
    double grade;

    credit = kor + eng + mat;
    grade = (kor + eng + mat) / 3.0;
    res = (credit >= 10) && (grade > 4.0);
    printf("%d\n", res);

    return 0;
}



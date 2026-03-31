//  int로 점수를 입력을 받아서 90점 이상이면 A, 80점 이상이면 B, 
//  70점 이상이면 C, 60점 이상이면 D, 그 외는 F를 출력하는 프로그램을 작성하시오.

#include <stdio.h>

int main(void)
{
    int num;
    char grade = 'F';
    
    int score;
    printf("점수를 입력하세요 : \n");
    scanf("%d", &score);

    if(score >=90)
    {
        grade = 'A';
    } else if(score >= 80) {
        grade = 'B';
    } else if(score >= 70) {
        grade = 'C';
    } else if(score >= 60) {
        grade = 'D';
    }
    printf("%d 는 %c 등급.\n", num, grade);

    return 0;
}
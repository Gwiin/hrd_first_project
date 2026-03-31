#include <stdio.h>
#include <stdlib.h>  //atoi 함수

int main(int argc, char *argv[])
{
    // int score;
    // printf("점수를 입력하세요 : \n");
    // scanf("%d", &score);
    int score = atoi(argv[1]);

    if(score >= 60)
    {
        printf("점수: %d --- 합격\n", score);
    } else {
        printf("점수: %d --- 탈락\n", score);
    }

    return 0;

}
#include "baseballGame.h"


void generate_number(int* question) //숫자 생성
{
    srand(time(NULL));
    for(int i =0 ; i<SIZE; i++){
        question[i] = rand() % 10;
        for(int j = 0; j < i; j++){
            if(question[i] == question[j]){
                i--;
                break;
            }
        }
    }
}


void input_number(int* answer)// 숫자 입력
{
    printf("숫자3개 입력: ");
    for(int i = 0; i<SIZE;i++){
        scanf("%d", &answer[i]);
    }

    //입력값 중복 체크
    for(int i=0 ; i<SIZE; i++){
        for(int j=0; j<i;j++){
            if(answer[i] == answer[j]){
                printf("중복된 숫자. 다시 입력\n");
                input_number(answer);
                return;
            }
        }
    }
}

bool check_result(const int* question, const int* answer, int* strike, int* ball) //결과 체크
{
    *strike = 0;
    *ball = 0;

    for(int i=0;i<SIZE;i++){
        for(int j=0;j<SIZE;j++){
            if(question[i] == answer[j]){
                if(i==j){
                    (*strike)++;
                }else{
                    (*ball)++;
                }
            }
        }
    }

    
    return (*strike == SIZE);

}
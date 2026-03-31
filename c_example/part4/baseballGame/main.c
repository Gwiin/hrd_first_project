#include "baseballGame.h"





int main(){
    int count = 0;
    int strike, ball;
    int question[SIZE];
    int answer[SIZE];

    generate_number(question);

    while(true){
        input_number(answer);
        count++;

        if(check_result(question, answer, &strike, &ball)){
            break;
        }
        printf("%d Strike, %d Ball\n", strike, ball);
        
    }
    printf("축하합니다! %d회만에 맞췄습니다.\n", count);
    return 0;
}
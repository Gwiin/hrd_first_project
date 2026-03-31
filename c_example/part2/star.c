// 하나 씩 늘어나는 별표 화면에 찍기\
// * 

#include <stdio.h>

int main(void)
{   
    for (int i = 1; i <=5; ++i){
        for (int j = 1; j <= i; ++j){
            printf("*");
        }
        printf("\n");
    }

    return 0;
}
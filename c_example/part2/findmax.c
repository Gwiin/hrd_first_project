// 최대값 찾기 - 순회

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#define MAXINDEX 50


int main(void)
{
    int nums[MAXINDEX];
    srand(time(NULL));
    for (int i = 0; i < MAXINDEX; ++i){
        nums[i] = rand() % 100 ;
        printf("%d ", nums[i]);
    }
    int max;
    max = nums[0];
    for (int i = 0; i< MAXINDEX; ++i){
        if (max < nums[i]){
            max = nums[i];
        }else{
                max = max;
        }
        }
    printf("\n최대값 : %d", max);
    printf("\n");
    return 0;
}
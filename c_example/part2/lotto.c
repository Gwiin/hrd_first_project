#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(void)
{
    int lotto[6];
    int i, j, temp;

    srand(time(NULL));

    for (int i = 0; i < 6;i++){
        lotto[i] = (rand() % 45) + 1;
        //  중복검사
        for (int j = 0; j < i;j++){
            if (lotto[i] == lotto[j]){
                i--;
                break;
            }
        }
    }   
    for (int i = 0; i< 6;++i){
        printf("%d ", lotto[i]);
    }
    printf("\n");
    return 0;
}
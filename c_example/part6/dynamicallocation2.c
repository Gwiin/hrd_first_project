#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


int main(void){
    int size;
    printf("넣을 데이터의 크기를 알려주세요 : ");
    scanf("%d", &size);
    int *pArr;
    pArr = (int *)malloc(sizeof(int) * size);

    srand(time(NULL));
    for (int i = 0; i < size;++i)
    {
        *(pArr + i) = rand() % 101;
    }
    for (int i = 0; i < size; ++i){
        printf("%d, %p, ", *(pArr + i), pArr+i);
    }
    free(pArr);
    return 0;

   
}
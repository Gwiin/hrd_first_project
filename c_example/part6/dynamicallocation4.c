#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    int *pInt;
    int size = 5;
    int count = 0;
    int num;
    int i;

    pInt = (int *)calloc(size, sizeof(int));
    while(1)
    {
        printf("양수만 입력하세요 끝내려면 음수나 0을 입력 => ");
        scanf("%d", &num);
        if(num <=0)
            break;
        if (count == size)
        {   
            size +=5;
            pInt = (int *)realloc(pInt, size * sizeof(int));
        }
        pInt[count++] = num;
    }
    for (i = 0; i<count;++i){
        printf("%5d", pInt[i]);
    }
    free(pInt);
    return 0;
}
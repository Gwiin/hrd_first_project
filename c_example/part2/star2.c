//피라미드 형태 출력
//     *
//    ***
//   *****
//  *******
// *********

#include <stdio.h>

int main(void)
{   int height;
    printf("피라미드의 높이를 넣으세요");
    scanf("%d", &height);
    for (int i = 1; i <= height; ++i)
    {
        for(int j = 1; j <= 6-i; ++j)
        { 
            printf(" ");
        }
        for(int j = 1; j <= 2*i-1; ++j)
        {
            printf("*");
        }     
        printf("\n");       
    }         
    return 0;
}
#include <stdio.h>

int main(void)
{
    int num;
    printf("숫자를 넣으세요");
    scanf("%d", &num);

    //if(num<0){
    //    num = -num;
    // }
    
    printf("절댓값은 : %d\n", num < 0 ? -num : num);
    return 0;

}
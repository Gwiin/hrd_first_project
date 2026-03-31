#include <stdio.h>

int main(void) 
{
    int num;
    printf("숫자를 넣으세요 : \n");
    scanf("%d", &num);

    if(num>0){
        printf("%d 는 양수 입니다.\n", num);
    } else if(num == 0){
        printf("%d 는 0 입니다.\n", num);
    } else {
        printf("%d 는 음수 입니다.\n", num);
    }
    return 0;
}
#include <stdio.h>

int main(void)
{
    int num;
    char grade = 'F';
    printf("점수를 넣으세요 : \n");
    scanf("%d", &num);

    switch(num/10){
        case 10:
            grade = 'A';
            break;
        case 9:
            grade = 'A';
            break;
        case 8:
            grade = 'B';
            break;
        case 7:
            grade = 'C';
            break;
        case 6:
            grade = 'D';
            break;
        default:
            grade = 'F';
            break;
}
    printf("%d 는 %c 등급.\n", num, grade);
    return 0;
}
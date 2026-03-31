#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    char *pLiteral = "리터럴 상수이다.";
    int *pInt;
    double *pDouble;
    char pLocalVariable = 'c';
    char pArry[50] = "지역 변수 (배열)";

    pInt = (int *)malloc(sizeof(int));
    if (pInt == NULL)
    {
        printf("메모리가 부족합니다.\n");
        exit(1);
    }
    pDouble = (double *)malloc(sizeof(double));

    *pInt = 10;
    *pDouble = 3.4;

    printf("정수형으로 사용 : %d\n", *pInt);
    printf("실수형으로 사용 : %.1lf\n", *pDouble);
    printf("%s : 0x%p", pLiteral, pLiteral);
    printf("%d 힘메모리 : 0x%p\n", *pInt, pInt);
    printf("%c 지역변수 : 0x%p\n", pLocalVariable, &pLocalVariable);
    printf("%s : 0x%p", pArry, pArry);

    free(pInt);
    free(pDouble);

    return 0;
}
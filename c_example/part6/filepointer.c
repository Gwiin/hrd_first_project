#include <stdio.h>

int main(void)
{
    FILE *a;
    a = fopen("test2.dat", "w");
    printf("%d\n", a->_fileno);
    if (a == NULL)
    {
        printf("파일을 열수 없습니다.\n");
        return -1;
    }
    // fprintf(a, "이것은 파일포인터로 저장되는 예시 데이터 입니다.");
    fputs("이것은 파일포인터로 저장되는 예시 데이터 입니다.\n", a);
    fclose(a);
    return 0;
}
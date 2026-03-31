// little endian, big endian

#include <stdio.h>

int main(void)
{
    int a = 0x12345678;
    char *p;
    p = (char *)&a;
    printf("*p : 0x%x\n", *(p+1));
    return 0;
    // 0x78이 나오면 Big endian 이다.
}
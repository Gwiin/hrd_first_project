// 가슴둘레가 90 보다 작거나 같으면 S
// 90보다 크고 100보다 작거나 같으면 M
// 100보다 크며 L

#include <stdio.h>

int main(void)
{
    int chest = 95;
    char size;

    if(chest <= 90)
    {
        size = 'S';
    }else if(chest <=100)
    {
        size = 'M';
    }else
    {
        size = 'L';
    }
    printf("가슴둘레 : %d -> 사이즈%c\n", chest, size);
    return 0;
}
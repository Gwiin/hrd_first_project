#include <stdio.h>

int main()
{
    int i = 'A';
    while(i<='Z')
    {
        printf("%c ", i);
        ++i;
    }
    printf("\n");
    return 0;
}
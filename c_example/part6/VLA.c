#include <stdio.h>
#include <stdlib.h>

int main (void)
{
    int size;
    scanf("%d", &size);

    int num[size]; // VLA   지원한다.

    for (int i = 0; i < size; ++i){
        num[1] = i + 1;
    }
    for (int i = 0; i < size; ++i){
        printf("%d, ", num[i]);
    }
    printf("\n");
    return 0;
}
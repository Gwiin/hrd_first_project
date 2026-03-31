#include <stdio.h>
#include <string.h>

int main()
{
    char fruit[20] = "strawberry";
    printf("딸기 : %s\n", fruit);
    printf("딸기쨈: %s %s\n", fruit, "jam");
    strcpy(fruit, "banana");
    printf("바나나 : %s\n", fruit);
    // strcpy fruit -> apple
    fruit[0] = 'a';
    fruit[1] = 'p';
    fruit[2] = 'p';
    fruit[3] = 'l';
    fruit[4] = 'e';
    fruit[5] = NULL;
    printf("사과 : %s\n", fruit);
    return 0;
}
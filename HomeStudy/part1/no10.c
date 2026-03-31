#include <stdio.h>
#include <string.h>

int main(void)
{
    char fruit[20] = "Strawberry";

    printf("%s\n", fruit);
    strcpy(fruit, "banana");
    printf("%s\n", fruit);

    return 0;
}
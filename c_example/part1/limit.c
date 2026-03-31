#include <stdio.h>
#include <limits.h>

#define END_HELLO "end of file"

int main (void)
{
    printf("chr_bit : %d\n", CHAR_BIT);
    printf("char_max : %d\n", CHAR_MAX);
    printf("INT_MIN : %d \t INT_MAX : %d\n", INT_MIN, INT_MAX);
    printf("LONG_MIN : %ld \t LONG_MAX : %ld\n", LONG_MIN, LONG_MAX);
    printf(END_HELLO);
    return 0;
}
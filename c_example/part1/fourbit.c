#include <stdio.h>

int main(void)
{

    // unsigned char attr;
    __uint8_t  attr;
    attr = attr ^ attr;
    attr = attr | 0b00000001;
    printf("attr: 0x%02x\n", attr);
    attr = attr | (0b00000010 + 0b00000100);
    printf("attr: 0x%02x\n", attr);
    attr = attr & (~0b00000001);
    printf("attr: 0x%02x\n", attr);
    
    return 0;
}
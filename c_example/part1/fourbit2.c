#include <stdint.h>
#include <stdio.h>
#define BOLD 0x01
#define ITALIC (0x01 << 1)
#define SHADOW (0x01 << 2)
#define UNDERLINE (0x01 << 3)






int main(void)
{

    // unsigned char attr;
    __uint8_t  attr;
    attr = attr ^ attr;
    attr = attr | BOLD;
    printf("attr: 0x%02x\n", attr);
    attr = attr | (ITALIC + SHADOW);
    printf("attr: 0x%02x\n", attr);
    attr = attr & (~BOLD);
    printf("attr: 0x%02x\n", attr);
    if (attr & BOLD)
    {    
        printf("BOLD 옵션은 활성화 되었습니다");
    }else
    {
        printf("BOLD 옵션은 비활성화 되었습니다.");
    }
    if ((attr & ITALIC) >>1)
    {    
        printf("ITALIC 옵션은 활성화 되었습니다.\n");
    }
    else
    {
        printf("ITALIC 옵션은 비활성화 되었습니다.\n");
    }
    
    return 0;
}
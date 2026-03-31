#include "serial.h"
#include <stdio.h>
#include "serial.h"


int main(void)
{
    for (int i = 0; i < 10;++i){
        int num = getSerialNumber();
        printf("HYNDAI_MOTOR_%010d\n", num);
    }
    return 0;
}

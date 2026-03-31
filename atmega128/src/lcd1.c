#include <avr/io.h>
#include "lcd.h"

int main(void)
{
    lcdInit();
    lcdClear();
    lcdGotoXY(0,0);
    lcdDataWrite('H');
    lcdDataWrite('e');
    lcdDataWrite('l');
    lcdDataWrite('l');
    lcdDataWrite('o');
    lcdGotoXY(2,1);
    lcdPrintData("MCU World !",12);
    while(1)
    ;
    return 0;
}
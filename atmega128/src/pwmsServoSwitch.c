// 버튼을 누르면 0도, 45도, 90도, 135도 로 이동하게 작성하고
// lcd 에도 각도가 표시되게 만드세요.
#include <avr/interrupt.h>
#include <avr/io.h>
#include "lcd.h"
#include <stdlib.h>
#include <util/delay.h>
#include <stdio.h>

int main(void)
{
    // 서보 PWM 선 PE3
    DDRE = _BV(PE3);
    DDRC = 0xFF;

    TCCR3A = _BV(COM3A1) | _BV(WGM31);
    TCCR3B = _BV(WGM33) | _BV(WGM32) | _BV(CS31);
    // 분주비 8, fast pwm , timer3 oc 3a

    ICR3 = 40000; // 20ms 50hz .. 2000000-> 1초 ?? -> 0.02초
    OCR3A = 3000; // 40000 : 20 ? : 2 2000 ~ 4000

    lcdInit();
    lcdClear();
    lcdPrintData("Servo Ready!", 12);

    uint8_t switch_flag = 0;
    char buf[17];

    while (1)
    {
        switch_flag = (PINE >> 4) & 0x0F;
        if (switch_flag == 0x01){
            OCR3A = 2000;
            lcdGotoXY(0, 1);
            snprintf(buf, sizeof(buf), "Angle: %3d deg  ", 0);
            lcdPrintData(buf, 16);
        }
        else if (switch_flag == 0x02)
        {
            OCR3A = 2500;
            lcdGotoXY(0, 1);
            snprintf(buf, sizeof(buf), "Angle: %3d deg  ", 45);
            lcdPrintData(buf, 16);
        }
        else if (switch_flag == 0x04)
        {
            OCR3A = 3000;
            lcdGotoXY(0, 1);
            snprintf(buf, sizeof(buf), "Angle: %3d deg  ", 90);
            lcdPrintData(buf, 16);
        }
        else if (switch_flag == 0x08)
        {
            OCR3A = 3500;
            lcdGotoXY(0, 1);
            snprintf(buf, sizeof(buf), "Angle: %3d deg  ", 135);
            lcdPrintData(buf, 16);
        }
        _delay_ms(50);
    }
    return 0;
}

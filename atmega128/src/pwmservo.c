#include <lcd.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdlib.h>
#include <util/delay.h>

int main(void)
{
    // 서보 PWM 선 PE5
    DDRE = _BV(PE3);

    TCCR3A = _BV(COM3A1) | _BV(WGM31);
    TCCR3B = _BV(WGM33) | _BV(WGM32) | _BV(CS31);
    // 분주비 8, fast pwm , timer3 oc 3a

    ICR3 = 40000; // 20ms 50hz .. 2000000-> 1초 ?? -> 0.02초
    OCR3A = 3000; // 40000 : 20 ? : 2 2000 ~ 4000

    lcdInit();
    lcdClear();
    char buffer[16];

    while (1)
    {
        for (int pulse = 2000; pulse <= 4000;pulse +=40){
            OCR3A = pulse;
            itoa(pulse, buffer, 10);
            lcdGotoXY(0, 0);
            lcdPrint(buffer);
            _delay_ms(20);
        }
        for (int pulse = 4000; pulse <= 2000; pulse -= 40)
        {
            OCR3A = pulse;
            itoa(pulse, buffer, 10);
            lcdGotoXY(0, 0);
            lcdPrint(buffer);
            _delay_ms(20);
        }
    }
    return 0;
}
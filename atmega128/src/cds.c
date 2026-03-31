#include "lcd.h"
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdio.h>
#include <string.h>
#include <util/delay.h>

volatile uint16_t ADC_result = 0;

int main(void)
{
    DDRB = 0x10;
    TCCR0 = 0x62;
    TCNT0 = 0x00;
    ADMUX = 0x40; // ADC0 선택, single mode, 0번 채널, 3.3V 외부 기준 전압 (AREF)
    ADCSRA = 0xAF; // 10101111 ADC 허가, free running Mode, Interrupt 허가, 128 분주
    ADCSRA |= 0x40; // ADSC AD 개시(Start)
    sei();

    lcdInit();
    lcdGotoXY(0, 0);
    lcdPrintData("Light Sensor", 12);

    while (1)
    {
        OCR0 = (ADC_result - 40) * 255 / 700;
        if (ADC_result < 40)
        {
            OCR0 = 0;
        }
        else if(ADC_result > 700)
        {
            OCR0 = 255;
        }
        else{
            OCR0 = (ADC_result - 40) * 255 / 660;
        }

        {
            char buf[16];
            sprintf(buf, "L:%u   ", ADC_result);
            lcdGotoXY(0, 1);
            lcdPrintData(buf, strlen(buf));
        }
        _delay_ms(200);
    }
    return 0;
}

ISR(ADC_vect)
{
    cli();
    ADC_result = ADC;
    sei();
}
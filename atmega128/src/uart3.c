#include "lcd.h"
#include "uart0.h"
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>

volatile uint8_t int_data = '0';
uint8_t cursor = 0;

FILE OUTPUT = FDEV_SETUP_STREAM(uart0Transmit, NULL, _FDEV_SETUP_WRITE);
FILE INPUT = FDEV_SETUP_STREAM(NULL, uart0Receive, _FDEV_SETUP_READ);

int main(void)
{
    uart0Init();
    lcdInit();

    stdin = &INPUT;
    stdout = &OUTPUT;

    DDRE = 0x02;

    EICRB = 0xFF;
    EIMSK = 0xF0;
    EIFR = 0xF0;

    sei();
    char cData;
    // char buffer[32];
    uint8_t count = 0;

    printf("Hi, I'm Atmega128");
    lcdGotoXY(0, 0);

    while(1)
    {
        if(int_data != '0')
        {
            printf("\n\r Input Switch : %c", int_data);
            int_data = '0';
        }
        while(UCSR0A & (1 << RXC0))
        {
            count++;
            cData = fgetc(stdin);
            lcdDataWrite(cData);
            lcdDataWrite('0' + count);
            cursor++;
            cursor++;
            if(cursor == 16)
                lcdGotoXY(0, 1);
            else if(cursor >=32)
            {
                cursor = 0;
                lcdGotoXY(0, 0);
            }
        }
    }
    return 0;
}

ISR(INT4_vect)
{
    cli();
    int_data = '1';
    sei();
}

ISR(INT5_vect)
{
    cli();
    int_data = '2';
    sei();
}

ISR(INT6_vect)
{
    cli();
    int_data = '3';
    sei();
}

ISR(INT7_vect)
{
    cli();
    int_data = '4';
    sei();
}
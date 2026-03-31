#include "uart0.h"
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdio.h>



FILE OUTPUT = FDEV_SETUP_STREAM(uart0Transmit, NULL, _FDEV_SETUP_WRITE);
FILE INPUT = FDEV_SETUP_STREAM(NULL, uart0Receive, _FDEV_SETUP_READ);

int main(void)
{
    uart0Init();
    stdin = &INPUT;
    stdout = &OUTPUT;
    uint8_t numbers[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7C, 0x07, 0x7F, 0x67, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71, 0x80};
    uint8_t rx_data;
    DDRA = 0xFF;

    printf("hi, I'm Atmega128");
    fflush(stdout);

    while (1)
    {
        scanf("%c", &rx_data);
        if ((rx_data >= 0x30) && (rx_data <= 0x39))
        {
            PORTA = numbers[rx_data - 0x30];
        }
    }
    return 0;
}
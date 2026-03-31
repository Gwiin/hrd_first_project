#include <avr/interrupt.h>
#include <avr/io.h>

volatile uint8_t txFlag = 0;
volatile int8_t txData = 0;

uint8_t getch(void)
{
    uint8_t data;
    while ((UCSR0A & 0x80) == 0) // 문자 버퍼에 있으면 루프 탈출
        ;
    data = UDR0;
    UCSR0A |= 0x80;
    return data;
}

int main(void){
    uint8_t numbers[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7C, 0x07, 0x7F, 0x67, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71, 0x80};
    uint8_t rx_data;
    DDRA = 0xFF;

    UCSR0A = 0x00;
    UCSR0B = 0x18; // 0x00011000 rx tx enable
    UCSR0C = 0x16; // 0b00010110 no Parity, 1 stop bit

    UBRR0H = 0x00;
    UBRR0L = 0x07; // 115200 bps

    while(1)
    {
        rx_data = getch();
        if((rx_data >= 0x30) && (rx_data <= 0x39))
        {
            PORTA = numbers[rx_data - 0x30];
        }
    }
    return 0;
}
#include <avr/io.h>
#include <util/delay.h>

int main()
{
    *(volatile uint8_t *)(0x14) = 0x03;  //0b00000011
    DDRC = 0x03; // 0011 0,1 울 출력 모드로 한다.
    while (1)
    {
        PORTC |= (1 << PC0) | (1 << PC1) | (1 << PC2) | (1 << PC3);
        PORTC |= _BV(PC0) | _BV(PC1) | _BV(PC2) | _BV(PC3);
        // 0b 0000 0000 -> 0000 0100
        // PORTC = 0x04;
        // PORTC = PORTC | _BV(PC2);
        PORTC |= _BV(PC2);
        PORTC = 0x0F; // 00001111 0, 1, 2, 3 번을 1(ON)->5V 출력시킴.
        _delay_ms(500);

        PORTC = 0x00; // 00000000 0, 1, 2, 3 번을 0(off)-> 0V 출력시킴.
        _delay_ms(500);
    }
    return 0;
}
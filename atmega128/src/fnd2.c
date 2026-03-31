#include <avr/io.h>
#include <avr/interrupt.h>
// delay() 함수를 쓰지 않음!!
// _delay_ms();

volatile uint8_t milliseconds = 0;

ISR(TIMER0_OVF_vect)
{
    ++milliseconds;
}

void timer0_init(void)
{
    TCCR0 = _BV(CS02) | _BV(CS01) | _BV(CS00);
    TIMSK |= _BV(TOIE0);
    sei();
}   

int main()
{
    timer0_init();
    uint8_t numbers[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7C, 0x07, 0x7F, 0x67};
    int count = 0;
    DDRA = 0xFF;
    while(1)
    {
        if(milliseconds >=32)
        {
            count++;
            milliseconds = 0;
            count = count % 10;
            PORTA = numbers[count];
        }
    }
    return 0;
}
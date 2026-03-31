#include <avr/delay.h>
#include <avr/io.h>

int main(void)
{
    DDRD = _BV(PD4) | _BV(PD5);
    DDRB = _BV(PB5);

    TCCR1A = _BV(COM1A1) | _BV(WGM11);
    TCCR1B = _BV(CS11) | _BV(WGM12) | _BV(WGM13);
    
    // fast pwm, 분주비 8, count 는 2MHz
    ICR1 = 800;   //2MHz : 1초 = 800Hz : x초
    OCR1A = 560;  //800 카운트 하는데 560까지 -> 70 %  

    while (1)
    {
        PORTD &= ~_BV(PD5);
        PORTD |= _BV(PD4);
        PORTB |= _BV(PB5);
        _delay_ms(500);
        PORTD &= ~_BV(PD4);
        PORTD |= _BV(PD5);
        PORTB |= _BV(PB5);
        _delay_ms(500);
    }
    return 0;
}

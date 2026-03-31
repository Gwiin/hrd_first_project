#include <avr/io.h>
#include <avr/interrupt.h>

uint8_t led_data = 0x01;
volatile uint8_t timer2_cnt = 0;
ISR(TIMER2_OVF_vect);

int main(void)
{
    DDRC = 0x0F;                     // LED 출력 설정
    TCCR2 = _BV(CS22) | _BV(CS20);   // 1024 분주비
    TIMSK = _BV(TOIE2);
    sei();
    uint8_t direction = 0;

    while(1)
    {
        if (timer2_cnt == 30)        // 0.3 초마다 작동
        {
            if(led_data > 0x04)
                direction = 0;
            if(led_data == 1)
            {
                direction = 1;
                led_data = 1;
            }
            if (direction)
                led_data <<= 1;
            else
                led_data >>= 1;
            timer2_cnt = 0;
        }
        PORTC = led_data;
    }
    return 0;
}

ISR(TIMER2_OVF_vect)
{
    cli();
    TCNT2 = 112;
    timer2_cnt++;
    sei();
}
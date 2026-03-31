#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

volatile uint8_t time_stop = 0;

int main(void){
    uint8_t led_data = 0x01;
    DDRC = 0x0F;
    DDRE = 0x00;
    // EICRB = 0x03; // 인터럽트4 상승 엣지
    EICRB = _BV(ISC41) | _BV(ISC40);
    // EIMSK = 0x10; // 인터럽트 4 허용
    EIMSK = _BV(INT4); // 인터럽트 4 허용
    EIFR = 0x10; // 인터럽트 플레그 클리어
    sei(); // 전역 SREG 인터럽트 설정
    while(1){
        PORTC = led_data;
        if(time_stop == 0)
        {
            if(led_data ==0x08)
                led_data = 0x01;
            else
                led_data <<= 1;
        }
        _delay_ms(100);
    }
}

ISR(INT4_vect)
{
    cli(); // SREG I 비트 해제
    if(time_stop == 0)
    {
        time_stop = 1;
    }
    else{
        time_stop = 0;
    }
    sei(); // 다시 설정
}
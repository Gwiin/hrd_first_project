#include <avr/io.h>
#include <avr/interrupt.h>

uint8_t FND_DATA_TBL[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7C, 0x07, 0x7F, 0x67, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71, 0x80};
volatile uint8_t time_s = 0x01;
volatile uint8_t timer1_cnt;

int main(void)
{
    DDRA = 0xFF; // fnd 출력 설정
    DDRE = 0x00; // 스위치 입력 설정

    TCCR1A = 0x00;
    TCCR1B = 0x05; //분주비 1024 // 16M/1024/65535 -> 0.238 Hz 4.2초

    OCR1A = 14400; // 65535  = 2^16 -> 14400 에서 trigger 준다. 1.085 Hz 0.92 초
    OCR1B = 28800; // 65535  = 2^16 -> 28800 에서 trigger 준다. 0.542 Hz 1.8초
    TIMSK = _BV(OCIE1A) | _BV(OCIE1B) | _BV(TOIE1);
    sei();

    PORTA = FND_DATA_TBL[0];
    while(1)
    {
        PORTA = FND_DATA_TBL[time_s];
        if (time_s > 10)
            time_s = 0;
    }
    return 0;
}

ISR(TIMER1_COMPA_vect)
{
    cli();
    time_s++;
    sei();
}

ISR(TIMER1_COMPB_vect)
{
    cli();
    time_s--;
    sei();
}

ISR(TIMER1_OVF_vect)
{
    cli();
    time_s++;
    sei();
}
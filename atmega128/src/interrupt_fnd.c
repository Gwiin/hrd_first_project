#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

volatile uint8_t cnt = 0;


int main(void)
{
    // fnd init()
    uint8_t FND_DATA_TBL[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7C, 0x07, 0x7F, 0x67, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71, 0x80};
    DDRA = 0xFF;

    // 인터럽트 설정
    EICRB = _BV(ISC41) | _BV(ISC40); // 인터럽트4 상승 엣지
    EIMSK |= _BV(INT4); // 인터럽트 4 허용
    EICRB |= _BV(ISC51) | _BV(ISC50); // 인터럽트5 상승 엣지
    EIMSK |= _BV(INT5);               // 인터럽트 4 허용
    sei();             // 전역 SREG 인터럽트 설정

    while (1)
    {
        PORTA = FND_DATA_TBL[cnt];
        _delay_ms(100);
    }
}

ISR(INT4_vect)
{
    cli(); // SREG I 비트 해제
    cnt = (cnt + 1) % 10;
    sei(); // 다시 설정
}

ISR(INT5_vect)
{
    cli(); // SREG I 비트 해제
    cnt--;
    if (cnt < 0)
        cnt = 9;
    sei(); // 다시 설정
}
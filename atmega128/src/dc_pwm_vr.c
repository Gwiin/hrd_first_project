#include <avr/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

volatile uint16_t ADC_result = 0;

int main(void)
{
    DDRD = _BV(PD4) | _BV(PD5);
    DDRB = _BV(PB5);

    TCCR1A = _BV(COM1A1) | _BV(WGM11);
    TCCR1B = _BV(CS11) | _BV(WGM12) | _BV(WGM13);
    
    // fast pwm, 분주비 8, count 는 2MHz
    ICR1 = 800;   //2MHz : 1초 = 800Hz : x초
    OCR1A = 0;  //800 카운트 하는데 560까지 -> 70 %  
   
    //가변 자항
    ADMUX = 0x40; // ADC0 선택, single mode, 0번 채널, 3.3V 외부 기준 전압 (AREF)
    ADCSRA = 0xAF; // 10101111 ADC 허가, free running Mode, Interrupt 허가, 128 분주
    ADCSRA |= 0x40; // ADSC AD 개시(Start)
    sei();

    PORTD &= ~_BV(PD5);
    PORTD |= _BV(PD4);

    while (1)
    {
    OCR1A = (uint32_t)ADC_result * 800 / 1024;
    _delay_ms(50);    
    }
    return 0;
}
ISR(ADC_vect)
{
    cli();
    ADC_result = ADC;
    sei();
}
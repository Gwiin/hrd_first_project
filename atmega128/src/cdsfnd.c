// cds 값을 fnd 출력으로 연결하기.
// 최대값이 10 최소값이 0 되도록 설정
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>

volatile uint16_t ADC_result = 0;

// FND 0~9 데이터 테이블 (공통 캐소드 기준)
uint8_t FND_DATA_TBL[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};

ISR(ADC_vect)
{
    ADC_result = ADC; // ADC 값 읽기 (0~1023)
}

int main(void)
{
    // 1. 포트 설정
    DDRA = 0xFF; // FND 데이터 출력 (Port A)
    DDRB = 0x10; // PWM 출력 등 기존 설정 유지

    // 2. ADC 설정 (기존 코드 유지)
    ADMUX = 0x40;   // ADC1 채널 선택, AREF 5V(또는 3.3V) 기준
    ADCSRA = 0xAF;  // ADC 허가, Free running, 인터럽트 허가, 128 분주
    ADCSRA |= 0x40; // ADC 변환 시작

    sei(); // 전역 인터럽트 허가

    uint8_t display_val = 0;

    while (1)
    {
        // 3. CDS 값을 0~10 범위로 매핑
        // 1024 / 10 = 약 102. 이 값으로 나누어 범위를 조절합니다.
        display_val = ADC_result / 100;

        if (display_val > 10)
            display_val = 10;

        // 4. FND 출력
        if (display_val < 10)
        {
            PORTA = FND_DATA_TBL[display_val];
        }
        else
        {
            // '10'일 때: 여기서는 편의상 'A' 또는 별도 패턴 출력 (FND 1개 기준)
            // 만약 FND가 1개라면 10을 표현하기 어려우므로 'A'나 '-'를 띄웁니다.
            PORTA = 0x77; // 'A' 표시 (또는 원하는 패턴)
        }

        _delay_ms(100); // 표시 업데이트 간격
    }
    return 0;
}
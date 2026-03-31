#include <avr/io.h>
#include <util/delay.h>

int main()
{
    //unsigned char led_data = 0x00; // 'ibyte = 8bit, 4bit = 2^4 = 16 (00 ~ ff)
    //unsigned char led_data = 0b00000000;
    uint8_t led_data = 0x00;
    DDRC = 0x0F;
    //DDRC = 0b00001111;
    while(1)
    {
        PORTC = led_data;
        led_data++;
        if(led_data > 0x0F) 
           led_data = 0;
        _delay_ms(500);
    }
    return 0;
}
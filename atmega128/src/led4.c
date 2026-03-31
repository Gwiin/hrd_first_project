#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
    uint8_t current_pattern_value;
    uint8_t next_pattern_value = 0x01; // 0b 0000 0001

    DDRC = 0x0F;
    while(1)
    {
        current_pattern_value = next_pattern_value;

        PORTC = current_pattern_value;

        next_pattern_value = current_pattern_value << 1;

        if(next_pattern_value == 0x10)
           next_pattern_value = 0x01;
        _delay_ms(400);
    }
    return 0;
}
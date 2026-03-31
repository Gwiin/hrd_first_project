#include <avr/io.h>
#include <util/delay.h>

int main()
{
    DDRC = 0x0F;
    while(1)
    {
        PORTC = 0x0F;
        _delay_ms(500);
        
        PORTC = 0x00;
        _delay_ms(500);        
    }
    return 0;
}
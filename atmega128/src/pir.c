#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
    DDRE = 0x00;
    DDRC = 0x0F;

    while(1)
    {
        if(PINE){
            PORTC = 0x0F;
        } else{
            PORTC = 0x00;
        }
    }
    return 0;
}
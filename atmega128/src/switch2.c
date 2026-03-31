#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
    uint8_t switch_flag = 0, switch_flag_pre = 0x01;

    DDRC = 0x0F;
    DDRE = 0x00;
    PORTC = 0x00;

    while(1)
    {
        switch_flag = PINE >> 4;
        if (switch_flag == 0x01 || switch_flag == 0x02 || switch_flag == 0x04 || switch_flag == 0x08)
        {
            switch_flag_pre = switch_flag;

        }
        PORTC ^= 0x0F;

        if(switch_flag_pre == 0x01)
        _delay_ms(250);
        else if(switch_flag_pre == 0x02)
        _delay_ms(500);
        else if(switch_flag_pre == 0x04)
        _delay_ms(750);
        else if(switch_flag_pre == 0x08)
        _delay_ms(1000);
    }
    return 0;
}
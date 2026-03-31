#include <avr/io.h>
#include <util/delay.h>

int main(void){
    uint8_t FND_DATA_TBL[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7C, 0x07, 0x7F, 0x67, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71, 0x80};
    uint8_t cnt = 0;
    DDRA = 0xFF;

    while (1)
    {
        PORTA = FND_DATA_TBL[cnt];
        cnt++;
        if (cnt == 16)
            cnt = 0;
        _delay_ms(400);
    }
    return 0;
}
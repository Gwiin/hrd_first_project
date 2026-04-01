#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "hardware/sync.h"

//--------------------------------------
// 핀 설정
//--------------------------------------
#define DHT_PIN 15

// CDS (LDR) 입력 ADC 핀 (GPIO27 = ADC1)
#define CDS_GPIO 27
#define CDS_ADC_CHANNEL 1

//--------------------------------------
// DHT 읽기용 함수
//--------------------------------------
bool wait_for_level(bool level, uint32_t timeout_us)
{
    uint32_t start = time_us_32();
    while (gpio_get(DHT_PIN) != level)
    {
        if (time_us_32() - start > timeout_us)
            return false;
    }
    return true;
}

bool read_dht(float *temperature, float *humidity)
{
    uint8_t data[5] = {0};

    uint32_t irq_state = save_and_disable_interrupts();

    gpio_set_dir(DHT_PIN, GPIO_OUT);
    gpio_put(DHT_PIN, 0);
    sleep_ms(20);
    gpio_put(DHT_PIN, 1);
    sleep_us(30);
    gpio_set_dir(DHT_PIN, GPIO_IN);

    if (!wait_for_level(0, 100))
        goto fail;
    if (!wait_for_level(1, 100))
        goto fail;
    if (!wait_for_level(0, 100))
        goto fail;

    for (int i = 0; i < 40; i++)
    {
        if (!wait_for_level(1, 70))
            goto fail;

        sleep_us(30);

        if (gpio_get(DHT_PIN))
            data[i / 8] |= (1 << (7 - (i % 8)));

        if (!wait_for_level(0, 70))
            goto fail;
    }

    restore_interrupts(irq_state);

    if ((data[0] + data[1] + data[2] + data[3]) != data[4])
    {
        printf("[DHT] checksum fail (%u+%u+%u+%u!=%u)\n",
               data[0], data[1], data[2], data[3], data[4]);
        return false;
    }

    // DHT11 기준
    *humidity = (float)data[0] + (float)data[1] / 10.0f;
    *temperature = (float)data[2] + (float)data[3] / 10.0f;

    printf("[DHT] raw=%u,%u,%u,%u,%u -> %.1f%% %.1fC\n",
           data[0], data[1], data[2], data[3], data[4],
           *humidity, *temperature);

    return true;

fail:
    restore_interrupts(irq_state);
    printf("[DHT] signal timeout fail\n");
    return false;
}

//--------------------------------------
// 메인
//--------------------------------------
int main()
{
    stdio_init_all();
    sleep_ms(2000);

    // DHT 초기화
    gpio_init(DHT_PIN);
    gpio_pull_up(DHT_PIN);

    // ADC 초기화 (CDS)
    adc_init();
    adc_gpio_init(CDS_GPIO);
    adc_select_input(CDS_ADC_CHANNEL);

    printf("Sensor Booting...\n");
    printf("Please wait...\n");
    sleep_ms(1500);

    while (true)
    {
        // CDS 읽기
        uint16_t raw = adc_read();

        // DHT 읽기
        float temp = 0.0f;
        float humi = 0.0f;
        bool dht_ok = read_dht(&temp, &humi);

        // printf 출력
        printf("CDS raw=%u\n", raw);
        if (dht_ok)
        {
            printf("Temp: %.1fC, Humi: %.1f%%\n", temp, humi);
        }
        else
        {
            printf("DHT read fail\n");
        }

        sleep_ms(2000);
    }

    return 0;
}
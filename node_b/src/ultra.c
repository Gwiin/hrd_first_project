#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#define TRIG_PIN 3
#define ECHO_PIN 2

#define I2C_PORT i2c0
#define SDA_PIN 0
#define SCL_PIN 1
#define ECHO_TIMEOUT_US 30000

#define LCD_PINMAP_YWROBOT 0
#define LCD_PINMAP_ALT 1
#define LCD_PINMAP LCD_PINMAP_YWROBOT

static uint8_t lcd_addr = 0x27;

bool lcd_probe(uint8_t addr);
bool lcd_detect_address(void);
void i2c_scan(void);
uint8_t lcd_make_byte(uint8_t nibble, int mode, int en);

// LCD 기능
uint8_t lcd_make_byte(uint8_t nibble, int mode, int en) {
#if LCD_PINMAP == LCD_PINMAP_YWROBOT
    const uint8_t rs_mask = 0x01;
    const uint8_t en_mask = 0x04;
    const uint8_t bl_mask = 0x08;

    return (nibble & 0xF0) | (mode ? rs_mask : 0) | bl_mask | (en ? en_mask : 0);
#else
    const uint8_t rs_mask = 0x80;
    const uint8_t en_mask = 0x20;
    const uint8_t bl_mask = 0x10;
    uint8_t shifted_nibble = (nibble >> 4) & 0x0F;

    return shifted_nibble | (mode ? rs_mask : 0) | bl_mask | (en ? en_mask : 0);
#endif
}

void lcd_send(uint8_t val, int mode) {
    uint8_t hi = val & 0xF0;
    uint8_t lo = (val << 4) & 0xF0;
    uint8_t data[4];

    data[0] = lcd_make_byte(hi, mode, 1);
    data[1] = lcd_make_byte(hi, mode, 0);
    data[2] = lcd_make_byte(lo, mode, 1);
    data[3] = lcd_make_byte(lo, mode, 0);

    i2c_write_blocking(I2C_PORT, lcd_addr, data, 4, false);
}

void lcd_cmd(uint8_t cmd){ lcd_send(cmd, 0);}
void lcd_char(uint8_t c){ lcd_send(c, 1);}

bool lcd_probe(uint8_t addr) {
    uint8_t dummy = 0;
    int result = i2c_write_blocking(I2C_PORT, addr, &dummy, 1, true);
    return result >= 0;
}

bool lcd_detect_address(void) {
    if (lcd_probe(0x27)) {
        lcd_addr = 0x27;
        return true;
    }
    if (lcd_probe(0x3F)) {
        lcd_addr = 0x3F;
        return true;
    }
    return false;
}

void i2c_scan(void) {
    uint8_t dummy = 0;
    int addr;

    printf("I2C scan start\n");
    for (addr = 0x08; addr <= 0x77; ++addr) {
        int result = i2c_write_blocking(I2C_PORT, (uint8_t)addr, &dummy, 1, true);
        if (result >= 0) {
            printf("I2C device found at 0x%02X\n", addr);
        }
    }
    printf("I2C scan end\n");
}

void lcd_init() {
    sleep_ms(50);
    lcd_cmd(0x33);
    lcd_cmd(0x32);
    lcd_cmd(0x28);
    lcd_cmd(0x0C);
    lcd_cmd(0x06);
    lcd_cmd(0x01);
    sleep_ms(5);
}

void lcd_set_cursor(int col, int row) {
    lcd_cmd((row==0?0x80:0xC0) + col);
}

void lcd_print(const char *s){
    while(*s) lcd_char(*s++);
}

// 초음파 거리 측정
bool measure_distance(float *distance_cm) {
    absolute_time_t wait_start;
    absolute_time_t start;
    absolute_time_t end;
    int64_t duration;

    gpio_put(TRIG_PIN, 0);
    sleep_us(2);

    gpio_put(TRIG_PIN, 1);
    sleep_us(10);
    gpio_put(TRIG_PIN, 0);

    wait_start = get_absolute_time();
    while (!gpio_get(ECHO_PIN)) {
        if (absolute_time_diff_us(wait_start, get_absolute_time()) > ECHO_TIMEOUT_US) {
            return false;
        }
    }
    start = get_absolute_time();

    while (gpio_get(ECHO_PIN)) {
        if (absolute_time_diff_us(start, get_absolute_time()) > ECHO_TIMEOUT_US) {
            return false;
        }
    }
    end = get_absolute_time();

    duration = absolute_time_diff_us(start, end);
    *distance_cm = (duration * 0.0343f) / 2.0f;
    return true;
}

int main(){
    bool lcd_ok;

    stdio_init_all();
    sleep_ms(2000);

    gpio_init(TRIG_PIN);
    gpio_set_dir(TRIG_PIN, GPIO_OUT);

    gpio_init(ECHO_PIN);
    gpio_set_dir(ECHO_PIN, GPIO_IN);

    i2c_init(I2C_PORT, 100*1000);
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);

    printf("Ultra + LCD test start\n");
    printf("TRIG=%d ECHO=%d SDA=%d SCL=%d\n",
           TRIG_PIN, ECHO_PIN, SDA_PIN, SCL_PIN);
    printf("LCD pinmap profile: %s\n",
#if LCD_PINMAP == LCD_PINMAP_YWROBOT
           "YWROBOT"
#else
           "ALT"
#endif
    );
    i2c_scan();

    lcd_ok = lcd_detect_address();
    printf("LCD detect: %s\n", lcd_ok ? "FOUND" : "NOT FOUND");

    if (!lcd_ok) {
        while (1) {
            printf("LCD not responding at 0x27 or 0x3F\n");
            sleep_ms(1000);
        }
    }

    printf("Using LCD address: 0x%02X\n", lcd_addr);
    lcd_init();
    lcd_cmd(0x01);
    sleep_ms(5);
    lcd_set_cursor(0,0);
    lcd_print("Ultrasonic");
    lcd_set_cursor(0,1);
    lcd_print("Starting...");
    sleep_ms(1000);

    char buf[16];
    while(1){
        float dist = 0.0f;
        bool ok = measure_distance(&dist);

        lcd_cmd(0x01);
        sleep_ms(2);

        lcd_set_cursor(0,0);
        lcd_print("Distance:");

        lcd_set_cursor(0,1);
        if (ok) {
            snprintf(buf, sizeof(buf), "%.1f cm", dist);
            printf("Distance: %.1f cm\n", dist);
        } else {
            snprintf(buf, sizeof(buf), "sensor timeout ");
            printf("Distance measurement timeout\n");
        }
        lcd_print(buf);

        sleep_ms(500);
    }
}

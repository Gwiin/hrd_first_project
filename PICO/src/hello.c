#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#define I2C_PORT i2c0
#define SDA_PIN 0
#define SCL_PIN 1

static uint8_t lcd_addr = 0x27;

bool lcd_probe(uint8_t addr);
bool lcd_detect_address(void);
void i2c_scan(void);

// LCD 제어 함수
void lcd_send(uint8_t val, int mode) {
    uint8_t hi = val & 0xF0;
    uint8_t lo = (val << 4) & 0xF0;
    uint8_t data[4];

    data[0] = hi | mode | 0x08 | 0x04; // EN=1
    data[1] = hi | mode | 0x08;        // EN=0
    data[2] = lo | mode | 0x08 | 0x04; // EN=1
    data[3] = lo | mode | 0x08;        // EN=0

    i2c_write_blocking(I2C_PORT, lcd_addr, data, 4, false);
}

void lcd_cmd(uint8_t cmd) { lcd_send(cmd, 0); }
void lcd_char(uint8_t c) { lcd_send(c, 1); }

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

// LCD 초기화
void lcd_init() {
    sleep_ms(50);
    lcd_cmd(0x33);
    lcd_cmd(0x32);
    lcd_cmd(0x28); // 4-bit mode, 2라인, 5x8 폰트
    lcd_cmd(0x0C); // Display ON, 커서 OFF
    lcd_cmd(0x06); // Entry mode
    lcd_cmd(0x01); // Clear display
    sleep_ms(5);
}

// 커서 위치 설정
void lcd_set_cursor(int col, int row) {
    lcd_cmd((row == 0 ? 0x80 : 0xC0) + col);
}

// 문자열 출력
void lcd_print(const char *s) {
    while (*s) lcd_char(*s++);
}

int main() {
    bool lcd_ok;

    stdio_init_all();
    sleep_ms(2000);

    // I2C 초기화
    i2c_init(I2C_PORT, 100 * 1000);
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);

    printf("Hello LCD test start\n");
    printf("SDA=%d SCL=%d\n", SDA_PIN, SCL_PIN);
    i2c_scan();

    lcd_ok = lcd_detect_address();
    printf("LCD detect: %s\n", lcd_ok ? "FOUND" : "NOT FOUND");

    if (!lcd_ok) {
        while (1) {
            printf("No LCD detected at 0x27 or 0x3F\n");
            sleep_ms(1000);
        }
    }

    printf("Using LCD address: 0x%02X\n", lcd_addr);

    // LCD 초기화
    lcd_init();
    sleep_ms(10);

    // 메시지 출력
    lcd_cmd(0x01);
    sleep_ms(5);
    lcd_set_cursor(0, 0);
    lcd_print("Hello World!");
    lcd_set_cursor(0, 1);
    lcd_print("Pico 2 W");
    printf("LCD message written\n");

    while (1) {
        tight_loop_contents(); // 루프 유지
    }
}

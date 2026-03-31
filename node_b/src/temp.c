#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include <stdio.h>

#define I2C_PORT i2c0
#define SDA_PIN 0
#define SCL_PIN 1

#define DHT_PIN 4       // DHT22 DATA 핀
#define DHT_MAX_TIMINGS 85
#define DHT_TIMEOUT_US 1000

#define LCD_PINMAP_YWROBOT 0
#define LCD_PINMAP_ALT 1
#define LCD_PINMAP LCD_PINMAP_YWROBOT

static uint8_t lcd_addr = 0x27;

bool lcd_probe(uint8_t addr);
bool lcd_detect_address(void);
void i2c_scan(void);
uint8_t lcd_make_byte(uint8_t nibble, int mode, int en);
bool wait_for_pin_state(uint gpio, bool state, uint32_t timeout_us);
const char *dht22_read_status(float *temperature, float *humidity);

// -------------------- LCD 함수 --------------------
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
    lcd_cmd((row == 0 ? 0x80 : 0xC0) + col);
}

void lcd_print(const char *s) {
    while (*s) lcd_char(*s++);
}

void lcd_print_line(uint8_t row, const char *text) {
    char line[17];
    int i;

    for (i = 0; i < 16; ++i) {
        if (text[i] == '\0') {
            break;
        }
        line[i] = text[i];
    }
    for (; i < 16; ++i) {
        line[i] = ' ';
    }
    line[16] = '\0';

    lcd_set_cursor(0, row);
    lcd_print(line);
}

// -------------------- DHT22 함수 --------------------
bool wait_for_pin_state(uint gpio, bool state, uint32_t timeout_us) {
    absolute_time_t start = get_absolute_time();

    while (gpio_get(gpio) != state) {
        if (absolute_time_diff_us(start, get_absolute_time()) > timeout_us) {
            return false;
        }
    }
    return true;
}

const char *dht22_read_status(float *temperature, float *humidity) {
    uint8_t data[5] = {0};
    int i;
    int j;

    gpio_set_dir(DHT_PIN, GPIO_OUT);
    gpio_put(DHT_PIN, 0);
    sleep_ms(2);
    gpio_put(DHT_PIN, 1);
    sleep_us(40);
    gpio_set_dir(DHT_PIN, GPIO_IN);
    gpio_pull_up(DHT_PIN);

    if (!wait_for_pin_state(DHT_PIN, false, DHT_TIMEOUT_US)) {
        return "start timeout";
    }
    if (!wait_for_pin_state(DHT_PIN, true, DHT_TIMEOUT_US)) {
        return "resp low fail";
    }
    if (!wait_for_pin_state(DHT_PIN, false, DHT_TIMEOUT_US)) {
        return "resp high fail";
    }

    for (i = 0; i < 5; i++) {
        uint8_t byte = 0;
        for (j = 0; j < 8; j++) {
            absolute_time_t pulse_start;
            int64_t high_time;

            if (!wait_for_pin_state(DHT_PIN, true, DHT_TIMEOUT_US)) {
                return "bit start fail";
            }

            pulse_start = get_absolute_time();
            if (!wait_for_pin_state(DHT_PIN, false, DHT_TIMEOUT_US)) {
                return "bit end fail";
            }

            high_time = absolute_time_diff_us(pulse_start, get_absolute_time());
            if (high_time > 40) {
                byte |= (1 << (7 - j));
            }
        }
        data[i] = byte;
    }

    if (data[4] != ((data[0] + data[1] + data[2] + data[3]) & 0xFF)) {
        return "checksum err";
    }

    *humidity = ((data[0] << 8) | data[1]) * 0.1f;
    *temperature = (((data[2] & 0x7F) << 8) | data[3]) * 0.1f;
    if (data[2] & 0x80) {
        *temperature = -*temperature;
    }
    return NULL;
}

int main() {
    bool lcd_ok;

    stdio_init_all();
    sleep_ms(2000);

    // I2C LCD 초기화
    i2c_init(I2C_PORT, 100*1000);
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);
    gpio_init(DHT_PIN);
    gpio_pull_up(DHT_PIN);

    printf("Temp + LCD test start\n");
    printf("DHT=%d SDA=%d SCL=%d\n", DHT_PIN, SDA_PIN, SCL_PIN);
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
            printf("No LCD detected at 0x27 or 0x3F\n");
            sleep_ms(1000);
        }
    }

    printf("Using LCD address: 0x%02X\n", lcd_addr);
    lcd_init();
    lcd_print_line(0, "DHT22 Starting");
    lcd_print_line(1, "Please wait...");
    sleep_ms(1000);

    char buf[16];
    float temp=0, hum=0;

    while (1) {
        const char *dht_error = dht22_read_status(&temp, &hum);

        if (!dht_error) {
            printf("Temp: %.1f C, Hum: %.1f %%\n", temp, hum);
            snprintf(buf, sizeof(buf), "Temp:%5.1f C", temp);
            lcd_print_line(0, buf);

            snprintf(buf, sizeof(buf), "Hum :%5.1f%%", hum);
            lcd_print_line(1, buf);
        } else {
            printf("DHT22 read error: %s\n", dht_error);
            lcd_print_line(0, "DHT22 error");
            lcd_print_line(1, dht_error);
        }
        sleep_ms(2000);
    }
}

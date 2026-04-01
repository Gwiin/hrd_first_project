#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "pico/unique_id.h"
#include "hardware/i2c.h"
#include "hardware/pwm.h"
#include "lwip/apps/mqtt.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>

#ifndef WIFI_SSID
#error Need to define WIFI_SSID
#endif

#ifndef WIFI_PASSWORD
#error Need to define WIFI_PASSWORD
#endif

#ifndef MQTT_SERVER_IP
#error Need to define MQTT_SERVER_IP
#endif

#ifndef MQTT_SERVER_PORT
#define MQTT_SERVER_PORT 1883
#endif

#define I2C_PORT i2c0
#define SDA_PIN 0
#define SCL_PIN 1

#define DHT_PIN 15
#define DHT_TIMEOUT_US 5000
#define SERVO_PIN 14
#define LED_PIN 16
#define HUMIDITY_THRESHOLD 70.0f
#define SERVO_CLOSED_US 500
#define SERVO_OPEN_US 2400

#define MQTT_TOPIC_LEN 64
#define MQTT_SUBSCRIBE_QOS 1
#define MQTT_PUBLISH_QOS 1
#define MQTT_PUBLISH_RETAIN 0
#define MQTT_KEEP_ALIVE_S 60

#define MQTT_TEMPERATURE_TOPIC "/tempservo_led/temperature"
#define MQTT_HUMIDITY_TOPIC "/tempservo_led/humidity"
#define MQTT_SENSOR_TOPIC "/tempservo_led/sensor"
#define MQTT_SENSOR_JSON_TOPIC "/tempservo_led/sensor/json"
#define MQTT_SERVO_CONTROL_TOPIC "/tempservo_led/servo"
#define MQTT_LED_CONTROL_TOPIC "/tempservo_led/led"
#define MQTT_SERVO_STATE_TOPIC "/tempservo_led/servo/state"
#define MQTT_LED_STATE_TOPIC "/tempservo_led/led/state"
#define MQTT_SERVO_STATE_JSON_TOPIC "/tempservo_led/servo/state/json"
#define MQTT_LED_STATE_JSON_TOPIC "/tempservo_led/led/state/json"
#define MQTT_STATUS_TOPIC "/tempservo_led/status"
#define MQTT_STATUS_JSON_TOPIC "/tempservo_led/status/json"
#define MQTT_WILL_TOPIC "/tempservo_led/online"

#define LCD_PINMAP_YWROBOT 0
#define LCD_PINMAP_ALT 1
#define LCD_PINMAP LCD_PINMAP_YWROBOT

typedef struct {
    mqtt_client_t *client;
    struct mqtt_connect_client_info_t info;
    ip_addr_t broker_ip;
    char topic[MQTT_TOPIC_LEN];
    char payload[MQTT_OUTPUT_RINGBUF_SIZE];
    bool mqtt_connected;
    bool led_auto_mode;
    bool servo_auto_mode;
    bool led_on;
    bool servo_open;
} tempservo_mqtt_state_t;

static uint8_t lcd_addr = 0x27;

static uint8_t lcd_make_byte(uint8_t nibble, int mode, int en);
static bool lcd_probe(uint8_t addr);
static bool lcd_detect_address(void);
static void i2c_scan(void);
static void lcd_init(void);
static void lcd_set_cursor(int col, int row);
static void lcd_print(const char *s);
static void lcd_print_line(uint8_t row, const char *text);
static bool wait_for_pin_state(uint gpio, bool state, uint32_t timeout_us);
static const char *dht22_read_status(float *temperature, float *humidity);
static void servo_init(void);
static void servo_set_pulse_us(uint pulse_us);
static void servo_set_open(bool open);
static void led_init(void);
static void led_set_state(tempservo_mqtt_state_t *state, bool on, bool publish_state);
static void publish_sensor_values(tempservo_mqtt_state_t *state, float temperature, float humidity);
static void publish_actuator_state(tempservo_mqtt_state_t *state);
static void publish_status_message(tempservo_mqtt_state_t *state, const char *status);
static bool payload_has_value(const char *payload, const char *key, const char *value);
static bool payload_requests_bool(const char *payload, const char *key, bool *result);

static uint8_t lcd_make_byte(uint8_t nibble, int mode, int en) {
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

static void lcd_send(uint8_t val, int mode) {
    uint8_t hi = val & 0xF0;
    uint8_t lo = (val << 4) & 0xF0;
    uint8_t data[4];

    data[0] = lcd_make_byte(hi, mode, 1);
    data[1] = lcd_make_byte(hi, mode, 0);
    data[2] = lcd_make_byte(lo, mode, 1);
    data[3] = lcd_make_byte(lo, mode, 0);

    i2c_write_blocking(I2C_PORT, lcd_addr, data, 4, false);
}

static void lcd_cmd(uint8_t cmd) { lcd_send(cmd, 0); }
static void lcd_char(uint8_t c) { lcd_send(c, 1); }

static bool lcd_probe(uint8_t addr) {
    uint8_t dummy = 0;
    int result = i2c_write_blocking(I2C_PORT, addr, &dummy, 1, true);
    return result >= 0;
}

static bool lcd_detect_address(void) {
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

static void i2c_scan(void) {
    uint8_t dummy = 0;

    printf("I2C scan start\n");
    for (int addr = 0x08; addr <= 0x77; ++addr) {
        int result = i2c_write_blocking(I2C_PORT, (uint8_t)addr, &dummy, 1, true);
        if (result >= 0) {
            printf("I2C device found at 0x%02X\n", addr);
        }
    }
    printf("I2C scan end\n");
}

static void lcd_init(void) {
    sleep_ms(50);
    lcd_cmd(0x33);
    lcd_cmd(0x32);
    lcd_cmd(0x28);
    lcd_cmd(0x0C);
    lcd_cmd(0x06);
    lcd_cmd(0x01);
    sleep_ms(5);
}

static void lcd_set_cursor(int col, int row) {
    lcd_cmd((row == 0 ? 0x80 : 0xC0) + col);
}

static void lcd_print(const char *s) {
    while (*s) {
        lcd_char(*s++);
    }
}

static void lcd_print_line(uint8_t row, const char *text) {
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

static bool wait_for_pin_state(uint gpio, bool state, uint32_t timeout_us) {
    absolute_time_t start = get_absolute_time();

    while (gpio_get(gpio) != state) {
        if (absolute_time_diff_us(start, get_absolute_time()) > timeout_us) {
            return false;
        }
    }
    return true;
}

static const char *dht22_read_status(float *temperature, float *humidity) {
    uint8_t data[5] = {0};

    gpio_set_dir(DHT_PIN, GPIO_IN);
    gpio_pull_up(DHT_PIN);
    sleep_ms(1);

    if (!gpio_get(DHT_PIN)) {
        return "idle low";
    }

    gpio_set_dir(DHT_PIN, GPIO_OUT);
    gpio_put(DHT_PIN, 0);
    sleep_ms(2);
    gpio_put(DHT_PIN, 1);
    sleep_us(40);
    gpio_set_dir(DHT_PIN, GPIO_IN);
    gpio_pull_up(DHT_PIN);
    sleep_us(10);

    if (!wait_for_pin_state(DHT_PIN, false, DHT_TIMEOUT_US)) {
        return "no sensor response";
    }
    if (!wait_for_pin_state(DHT_PIN, true, DHT_TIMEOUT_US)) {
        return "resp low fail";
    }
    if (!wait_for_pin_state(DHT_PIN, false, DHT_TIMEOUT_US)) {
        return "resp high fail";
    }

    for (int i = 0; i < 5; i++) {
        uint8_t byte = 0;

        for (int j = 0; j < 8; j++) {
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

static void servo_init(void) {
    gpio_set_function(SERVO_PIN, GPIO_FUNC_PWM);

    uint slice_num = pwm_gpio_to_slice_num(SERVO_PIN);
    pwm_config config = pwm_get_default_config();

    pwm_config_set_clkdiv(&config, 125.0f);
    pwm_config_set_wrap(&config, 20000 - 1);
    pwm_init(slice_num, &config, true);

    servo_set_pulse_us(SERVO_CLOSED_US);
}

static void servo_set_pulse_us(uint pulse_us) {
    uint slice_num = pwm_gpio_to_slice_num(SERVO_PIN);
    uint channel = pwm_gpio_to_channel(SERVO_PIN);

    if (pulse_us < SERVO_CLOSED_US) {
        pulse_us = SERVO_CLOSED_US;
    }
    if (pulse_us > SERVO_OPEN_US) {
        pulse_us = SERVO_OPEN_US;
    }

    pwm_set_chan_level(slice_num, channel, pulse_us);
}

static void servo_set_open(bool open) {
    servo_set_pulse_us(open ? SERVO_OPEN_US : SERVO_CLOSED_US);
}

static void led_init(void) {
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, 0);
}

static void mqtt_publish_text(tempservo_mqtt_state_t *state, const char *topic, const char *message, bool retain) {
    if (!state->mqtt_connected) {
        return;
    }

    mqtt_publish(state->client, topic, message, strlen(message), MQTT_PUBLISH_QOS, retain ? 1 : MQTT_PUBLISH_RETAIN, NULL, state);
}

static bool payload_has_value(const char *payload, const char *key, const char *value) {
    char pattern[48];

    snprintf(pattern, sizeof(pattern), "\"%s\":\"%s\"", key, value);
    if (strstr(payload, pattern)) {
        return true;
    }

    snprintf(pattern, sizeof(pattern), "\"%s\": \"%s\"", key, value);
    return strstr(payload, pattern) != NULL;
}

static bool payload_requests_bool(const char *payload, const char *key, bool *result) {
    char pattern[32];

    snprintf(pattern, sizeof(pattern), "\"%s\":true", key);
    if (strstr(payload, pattern) || strstr(payload, "\"on\":true") || strstr(payload, "\"open\":true")) {
        *result = true;
        return true;
    }

    snprintf(pattern, sizeof(pattern), "\"%s\": true", key);
    if (strstr(payload, pattern) || strstr(payload, "\"on\": true") || strstr(payload, "\"open\": true")) {
        *result = true;
        return true;
    }

    snprintf(pattern, sizeof(pattern), "\"%s\":false", key);
    if (strstr(payload, pattern) || strstr(payload, "\"off\":false") || strstr(payload, "\"close\":false") ||
        strstr(payload, "\"closed\":false")) {
        *result = false;
        return true;
    }

    snprintf(pattern, sizeof(pattern), "\"%s\": false", key);
    if (strstr(payload, pattern) || strstr(payload, "\"off\": false") || strstr(payload, "\"close\": false") ||
        strstr(payload, "\"closed\": false")) {
        *result = false;
        return true;
    }

    return false;
}

static void led_set_state(tempservo_mqtt_state_t *state, bool on, bool publish_state) {
    state->led_on = on;
    gpio_put(LED_PIN, on ? 1 : 0);
    printf("LED: %s\n", on ? "ON" : "OFF");

    if (publish_state) {
        mqtt_publish_text(state, MQTT_LED_STATE_TOPIC, on ? "ON" : "OFF", false);
        mqtt_publish_text(state, MQTT_LED_STATE_JSON_TOPIC, on ? "{\"led\":\"ON\"}" : "{\"led\":\"OFF\"}", false);
    }
}

static void servo_apply_state(tempservo_mqtt_state_t *state, bool open, bool publish_state) {
    state->servo_open = open;
    servo_set_open(open);
    printf("Servo: %s\n", open ? "OPEN" : "CLOSED");

    if (publish_state) {
        mqtt_publish_text(state, MQTT_SERVO_STATE_TOPIC, open ? "OPEN" : "CLOSED", false);
        mqtt_publish_text(state, MQTT_SERVO_STATE_JSON_TOPIC, open ? "{\"servo\":\"OPEN\"}" : "{\"servo\":\"CLOSED\"}", false);
    }
}

static void publish_sensor_values(tempservo_mqtt_state_t *state, float temperature, float humidity) {
    char temp_text[16];
    char hum_text[16];
    char sensor_text[48];
    char sensor_json[96];

    snprintf(temp_text, sizeof(temp_text), "%.1f", temperature);
    snprintf(hum_text, sizeof(hum_text), "%.1f", humidity);
    snprintf(sensor_text, sizeof(sensor_text), "temp=%.1f,hum=%.1f", temperature, humidity);
    snprintf(sensor_json, sizeof(sensor_json), "{\"temperature\":%.1f,\"humidity\":%.1f}", temperature, humidity);

    mqtt_publish_text(state, MQTT_TEMPERATURE_TOPIC, temp_text, false);
    mqtt_publish_text(state, MQTT_HUMIDITY_TOPIC, hum_text, false);
    mqtt_publish_text(state, MQTT_SENSOR_TOPIC, sensor_text, false);
    mqtt_publish_text(state, MQTT_SENSOR_JSON_TOPIC, sensor_json, false);
}

static void publish_actuator_state(tempservo_mqtt_state_t *state) {
    char status_text[64];
    char status_json[128];

    mqtt_publish_text(state, MQTT_SERVO_STATE_TOPIC, state->servo_open ? "OPEN" : "CLOSED", false);
    mqtt_publish_text(state, MQTT_LED_STATE_TOPIC, state->led_on ? "ON" : "OFF", false);
    mqtt_publish_text(state, MQTT_SERVO_STATE_JSON_TOPIC, state->servo_open ? "{\"servo\":\"OPEN\"}" : "{\"servo\":\"CLOSED\"}", false);
    mqtt_publish_text(state, MQTT_LED_STATE_JSON_TOPIC, state->led_on ? "{\"led\":\"ON\"}" : "{\"led\":\"OFF\"}", false);

    snprintf(status_text, sizeof(status_text), "servo=%s(%s),led=%s(%s)",
             state->servo_open ? "OPEN" : "CLOSED",
             state->servo_auto_mode ? "AUTO" : "MANUAL",
             state->led_on ? "ON" : "OFF",
             state->led_auto_mode ? "AUTO" : "MANUAL");
    snprintf(status_json, sizeof(status_json),
             "{\"servo\":\"%s\",\"servo_mode\":\"%s\",\"led\":\"%s\",\"led_mode\":\"%s\"}",
             state->servo_open ? "OPEN" : "CLOSED",
             state->servo_auto_mode ? "AUTO" : "MANUAL",
             state->led_on ? "ON" : "OFF",
             state->led_auto_mode ? "AUTO" : "MANUAL");
    mqtt_publish_text(state, MQTT_STATUS_TOPIC, status_text, false);
    mqtt_publish_text(state, MQTT_STATUS_JSON_TOPIC, status_json, false);
}

static void publish_status_message(tempservo_mqtt_state_t *state, const char *status) {
    char status_json[128];

    mqtt_publish_text(state, MQTT_STATUS_TOPIC, status, false);
    snprintf(status_json, sizeof(status_json), "{\"message\":\"%s\"}", status);
    mqtt_publish_text(state, MQTT_STATUS_JSON_TOPIC, status_json, false);
}

static void subscribe_request_cb(void *arg, err_t err) {
    tempservo_mqtt_state_t *state = (tempservo_mqtt_state_t *)arg;

    if (err != ERR_OK) {
        printf("subscribe failed: %d\n", err);
        return;
    }

    publish_status_message(state, "mqtt subscribed");
    publish_actuator_state(state);
    mqtt_publish_text(state, MQTT_WILL_TOPIC, "1", true);
}

static void incoming_publish_cb(void *arg, const char *topic, __unused u32_t tot_len) {
    tempservo_mqtt_state_t *state = (tempservo_mqtt_state_t *)arg;

    strncpy(state->topic, topic, sizeof(state->topic) - 1);
    state->topic[sizeof(state->topic) - 1] = '\0';
}

static void incoming_data_cb(void *arg, const u8_t *data, u16_t len, __unused u8_t flags) {
    tempservo_mqtt_state_t *state = (tempservo_mqtt_state_t *)arg;
    size_t copy_len = len < sizeof(state->payload) - 1 ? len : sizeof(state->payload) - 1;

    memcpy(state->payload, data, copy_len);
    state->payload[copy_len] = '\0';

    printf("Topic: %s, Message: %s\n", state->topic, state->payload);

    if (strcmp(state->topic, MQTT_SERVO_CONTROL_TOPIC) == 0) {
        bool bool_value;

        if (strcasecmp(state->payload, "auto") == 0 || payload_has_value(state->payload, "mode", "auto")) {
            state->servo_auto_mode = true;
            publish_status_message(state, "servo auto mode");
        } else if (strcasecmp(state->payload, "open") == 0 || strcmp(state->payload, "1") == 0 ||
                   payload_has_value(state->payload, "servo", "open") || payload_has_value(state->payload, "state", "open") ||
                   payload_requests_bool(state->payload, "servo", &bool_value)) {
            state->servo_auto_mode = false;
            servo_apply_state(state,
                              (strcasecmp(state->payload, "open") == 0 || strcmp(state->payload, "1") == 0 ||
                               payload_has_value(state->payload, "servo", "open") || payload_has_value(state->payload, "state", "open"))
                                  ? true
                                  : bool_value,
                              true);
        } else if (strcasecmp(state->payload, "close") == 0 || strcasecmp(state->payload, "closed") == 0 ||
                   strcmp(state->payload, "0") == 0 || payload_has_value(state->payload, "servo", "close") ||
                   payload_has_value(state->payload, "servo", "closed") || payload_has_value(state->payload, "state", "close") ||
                   payload_has_value(state->payload, "state", "closed")) {
            state->servo_auto_mode = false;
            servo_apply_state(state, false, true);
        } else {
            publish_status_message(state, "servo cmd: auto/open/close");
        }
        publish_actuator_state(state);
        return;
    }

    if (strcmp(state->topic, MQTT_LED_CONTROL_TOPIC) == 0) {
        bool bool_value;

        if (strcasecmp(state->payload, "auto") == 0 || payload_has_value(state->payload, "mode", "auto")) {
            state->led_auto_mode = true;
            publish_status_message(state, "led auto mode");
        } else if (strcasecmp(state->payload, "on") == 0 || strcmp(state->payload, "1") == 0 ||
                   payload_has_value(state->payload, "led", "on") || payload_has_value(state->payload, "state", "on") ||
                   payload_requests_bool(state->payload, "led", &bool_value)) {
            state->led_auto_mode = false;
            led_set_state(state,
                          (strcasecmp(state->payload, "on") == 0 || strcmp(state->payload, "1") == 0 ||
                           payload_has_value(state->payload, "led", "on") || payload_has_value(state->payload, "state", "on"))
                              ? true
                              : bool_value,
                          true);
        } else if (strcasecmp(state->payload, "off") == 0 || strcmp(state->payload, "0") == 0) {
            state->led_auto_mode = false;
            led_set_state(state, false, true);
        } else if (payload_has_value(state->payload, "led", "off") || payload_has_value(state->payload, "state", "off")) {
            state->led_auto_mode = false;
            led_set_state(state, false, true);
        } else {
            publish_status_message(state, "led cmd: auto/on/off");
        }
        publish_actuator_state(state);
    }
}

static void mqtt_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status) {
    tempservo_mqtt_state_t *state = (tempservo_mqtt_state_t *)arg;

    if (status != MQTT_CONNECT_ACCEPTED) {
        printf("MQTT connect failed: %d\n", status);
        return;
    }

    state->mqtt_connected = true;
    printf("MQTT connected\n");

    mqtt_set_inpub_callback(client, incoming_publish_cb, incoming_data_cb, state);
    mqtt_subscribe(client, MQTT_SERVO_CONTROL_TOPIC, MQTT_SUBSCRIBE_QOS, subscribe_request_cb, state);
    mqtt_subscribe(client, MQTT_LED_CONTROL_TOPIC, MQTT_SUBSCRIBE_QOS, subscribe_request_cb, state);
}

static void start_mqtt_client(tempservo_mqtt_state_t *state) {
    state->client = mqtt_client_new();
    if (!state->client) {
        panic("mqtt_client_new failed");
    }

    cyw43_arch_lwip_begin();
    if (mqtt_client_connect(state->client, &state->broker_ip, MQTT_SERVER_PORT, mqtt_connection_cb, state, &state->info) != ERR_OK) {
        cyw43_arch_lwip_end();
        panic("mqtt_client_connect failed");
    }
    cyw43_arch_lwip_end();
}

int main(void) {
    tempservo_mqtt_state_t state = {
        .led_auto_mode = true,
        .servo_auto_mode = true,
    };
    bool lcd_ok;
    char lcd_line[17];
    char client_id[20];
    char unique_id[9];
    float temp = 0.0f;
    float hum = 0.0f;
    absolute_time_t next_sensor_read = make_timeout_time_ms(0);

    stdio_init_all();
    sleep_ms(2000);

    i2c_init(I2C_PORT, 100 * 1000);
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);
    gpio_init(DHT_PIN);
    gpio_pull_up(DHT_PIN);
    servo_init();
    led_init();

    printf("DHT22 + LCD + SG90 + LED + MQTT start\n");
    printf("Pins DHT=%d SDA=%d SCL=%d SERVO=%d LED=%d\n", DHT_PIN, SDA_PIN, SCL_PIN, SERVO_PIN, LED_PIN);
    printf("Humidity threshold: %.1f %%\n", HUMIDITY_THRESHOLD);
    printf("Broker: %s:%d\n", MQTT_SERVER_IP, MQTT_SERVER_PORT);
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
    lcd_print_line(0, "WiFi connect...");
    lcd_print_line(1, "MQTT waiting...");

    if (cyw43_arch_init()) {
        panic("Failed to initialize CYW43");
    }

    cyw43_arch_enable_sta_mode();
    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 30000)) {
        panic("Failed to connect Wi-Fi");
    }
    printf("Wi-Fi connected\n");

    pico_get_unique_board_id_string(unique_id, sizeof(unique_id));
    for (size_t i = 0; i < sizeof(unique_id) - 1; ++i) {
        unique_id[i] = (char)tolower((unsigned char)unique_id[i]);
    }
    snprintf(client_id, sizeof(client_id), "tsl%s", unique_id);

    state.info.client_id = client_id;
    state.info.keep_alive = MQTT_KEEP_ALIVE_S;
    state.info.client_user = NULL;
    state.info.client_pass = NULL;
    state.info.will_topic = MQTT_WILL_TOPIC;
    state.info.will_msg = "0";
    state.info.will_qos = MQTT_PUBLISH_QOS;
    state.info.will_retain = true;

    if (!ipaddr_aton(MQTT_SERVER_IP, &state.broker_ip)) {
        panic("Invalid MQTT_SERVER_IP");
    }

    start_mqtt_client(&state);

    while (1) {
        cyw43_arch_poll();

        if (time_reached(next_sensor_read)) {
            const char *dht_error = dht22_read_status(&temp, &hum);

            if (!dht_error) {
                bool auto_servo_open = hum >= HUMIDITY_THRESHOLD;

                if (state.servo_auto_mode) {
                    servo_apply_state(&state, auto_servo_open, true);
                }
                if (state.led_auto_mode) {
                    led_set_state(&state, auto_servo_open, true);
                }

                printf("Temp: %.1f C, Hum: %.1f %%\n", temp, hum);
                publish_sensor_values(&state, temp, hum);
                publish_actuator_state(&state);

                snprintf(lcd_line, sizeof(lcd_line), "Temp:%5.1f C", temp);
                lcd_print_line(0, lcd_line);

                snprintf(lcd_line, sizeof(lcd_line), "Hum:%4.1f %s", hum, state.servo_open ? "OPEN" : "CLOSE");
                lcd_print_line(1, lcd_line);
            } else {
                printf("DHT22 read error: %s\n", dht_error);
                if (state.servo_auto_mode) {
                    servo_apply_state(&state, false, true);
                }
                if (state.led_auto_mode) {
                    led_set_state(&state, false, true);
                }
                lcd_print_line(0, "DHT22 error");
                lcd_print_line(1, dht_error);
                publish_status_message(&state, dht_error);
            }

            next_sensor_read = make_timeout_time_ms(2000);
        }

        cyw43_arch_wait_for_work_until(next_sensor_read);
    }
}

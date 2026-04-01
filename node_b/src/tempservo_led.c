#include "tempservo_led_contract.h"
#include "node_b_config.h"

#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "hardware/pio.h"
#include "hardware/pwm.h"
#include "lwip/apps/mqtt.h"
#include "lwip/dns.h"
#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"
#include "pico/unique_id.h"
#include "ws2812.pio.h"

#ifndef WIFI_SSID
#define WIFI_SSID NODE_B_DEFAULT_WIFI_SSID
#endif

#ifndef WIFI_PASSWORD
#define WIFI_PASSWORD NODE_B_DEFAULT_WIFI_PASSWORD
#endif

#ifndef MQTT_SERVER
#define MQTT_SERVER NODE_B_DEFAULT_MQTT_SERVER
#endif

#ifndef MQTT_USERNAME
#define MQTT_USERNAME NODE_B_DEFAULT_MQTT_USERNAME
#endif

#ifndef MQTT_PASSWORD
#define MQTT_PASSWORD NODE_B_DEFAULT_MQTT_PASSWORD
#endif

#ifndef MQTT_PORT
#define MQTT_PORT NODE_B_DEFAULT_MQTT_PORT
#endif

#ifndef MQTT_KEEP_ALIVE_S
#define MQTT_KEEP_ALIVE_S NODE_B_DEFAULT_MQTT_KEEP_ALIVE_S
#endif

#ifndef MQTT_QOS
#define MQTT_QOS NODE_B_DEFAULT_MQTT_QOS
#endif

#define NODE_B_TOPIC_BUFFER_SIZE 64
#define NODE_B_PAYLOAD_BUFFER_SIZE 128
#define NODE_B_LOG_BUFFER_SIZE 160
#define NODE_B_HEARTBEAT_INTERVAL_MS 2000

#define SERVO_PIN 14
#define LED_PIN 16
#define WS2812_LED_COUNT 8
#define WS2812_FREQ 800000
#define WS2812_RGBW false
#define WS2812_COLOR_WHITE 0x00ffffffu
#define PWM_WRAP 20000
#define SERVO_CLOSED_US 500
#define SERVO_OPEN_US 2400

typedef struct {
    mqtt_client_t *client;
    ip_addr_t server_addr;
    struct mqtt_connect_client_info_t client_info;
    char topic_buffer[NODE_B_TOPIC_BUFFER_SIZE];
    char payload_buffer[NODE_B_PAYLOAD_BUFFER_SIZE];
    char client_id[32];
    bool wifi_ready;
    bool mqtt_ready;
    bool lamp_on;
    bool window_open;
    uint64_t last_heartbeat_ms;
} node_b_state_t;

static node_b_state_t g_state;
static bool g_cyw43_inited;
static uint g_servo_slice;
static PIO g_led_pio;
static uint g_led_sm;
static uint g_led_offset;

static void node_b_log(const char *fmt, ...)
{
    char buffer[NODE_B_LOG_BUFFER_SIZE];
    va_list args;

    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    printf("%s\n", buffer);
}

static void servo_init(void)
{
    pwm_config config;

    gpio_set_function(SERVO_PIN, GPIO_FUNC_PWM);
    g_servo_slice = pwm_gpio_to_slice_num(SERVO_PIN);

    config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, 125.0f);
    pwm_config_set_wrap(&config, PWM_WRAP - 1);
    pwm_init(g_servo_slice, &config, true);
}

static void servo_set_open(bool open)
{
    pwm_set_gpio_level(SERVO_PIN, open ? SERVO_OPEN_US : SERVO_CLOSED_US);
}

static void ws2812_put_pixel(uint32_t rgb)
{
    if (g_led_pio == NULL) {
        return;
    }

    pio_sm_put_blocking(
        g_led_pio,
        g_led_sm,
        ((rgb >> 8) & 0x0000ff00u) |
        ((rgb << 16) & 0x00ff0000u) |
        ((rgb << 8) & 0xff000000u)
    );
}

static void ws2812_fill(uint32_t rgb)
{
    for (int i = 0; i < WS2812_LED_COUNT; ++i) {
        ws2812_put_pixel(rgb);
    }
}

static void led_init(void)
{
    if (!pio_claim_free_sm_and_add_program_for_gpio_range(&ws2812_program, &g_led_pio, &g_led_sm, &g_led_offset, LED_PIN, 1, true)) {
        panic("ws2812 init failed");
    }

    ws2812_program_init(g_led_pio, g_led_sm, g_led_offset, LED_PIN, WS2812_FREQ, WS2812_RGBW);
    ws2812_fill(0);
}

static void trim_copy(char *dest, size_t dest_size, const char *src)
{
    size_t start = 0;
    size_t end = 0;
    size_t length = 0;

    if (dest_size == 0) {
        return;
    }

    dest[0] = '\0';
    if (src == NULL) {
        return;
    }

    length = strlen(src);
    while (start < length && isspace((unsigned char) src[start])) {
        start++;
    }

    end = length;
    while (end > start && isspace((unsigned char) src[end - 1])) {
        end--;
    }

    if (end <= start) {
        return;
    }

    length = end - start;
    if (length >= dest_size) {
        length = dest_size - 1;
    }

    memcpy(dest, src + start, length);
    dest[length] = '\0';
}

static bool equals_ignore_case(const char *lhs, const char *rhs)
{
    unsigned char left = 0;
    unsigned char right = 0;

    if (lhs == NULL || rhs == NULL) {
        return false;
    }

    while (*lhs != '\0' && *rhs != '\0') {
        left = (unsigned char) tolower((unsigned char) *lhs);
        right = (unsigned char) tolower((unsigned char) *rhs);

        if (left != right) {
            return false;
        }

        lhs++;
        rhs++;
    }

    return *lhs == '\0' && *rhs == '\0';
}

static const char *lamp_name(bool lamp_on)
{
    return lamp_on ? "ON" : "OFF";
}

static const char *window_name(bool window_open)
{
    return window_open ? "OPEN" : "CLOSE";
}

static void mqtt_publish_request_cb(__unused void *arg, err_t err)
{
    if (err != ERR_OK) {
        node_b_log("[NODE_B] WARN mqtt publish failed: %d", err);
    }
}

static void node_b_publish(const char *topic, const char *payload)
{
    if (g_state.client == NULL || !g_state.mqtt_ready) {
        return;
    }

    cyw43_arch_lwip_begin();
    mqtt_publish(g_state.client, topic, payload, strlen(payload), MQTT_QOS, 0, mqtt_publish_request_cb, &g_state);
    cyw43_arch_lwip_end();
}

static void publish_status_snapshot(void)
{
    char payload[64];

    snprintf(payload, sizeof(payload), "lamp=%s,window=%s", lamp_name(g_state.lamp_on), window_name(g_state.window_open));
    node_b_publish(NODE_B_TOPIC_STATUS_NODE_B, payload);
}

static void publish_heartbeat(void)
{
    node_b_publish(NODE_B_TOPIC_HEARTBEAT_NODE_B, "alive");
}

static void set_lamp_state(bool lamp_on, const char *reason)
{
    if (g_state.lamp_on == lamp_on) {
        return;
    }

    g_state.lamp_on = lamp_on;
    ws2812_fill(lamp_on ? WS2812_COLOR_WHITE : 0);
    node_b_log("[NODE_B] LAMP %s (%s)", lamp_name(lamp_on), reason);
    publish_status_snapshot();
}

static void set_window_state(bool window_open, const char *reason)
{
    if (g_state.window_open == window_open) {
        return;
    }

    g_state.window_open = window_open;
    servo_set_open(window_open);
    node_b_log("[NODE_B] WINDOW %s (%s)", window_name(window_open), reason);
    publish_status_snapshot();
}

static void mqtt_subscribe_request_cb(void *arg, err_t err)
{
    (void) arg;

    if (err != ERR_OK) {
        node_b_log("[NODE_B] WARN mqtt subscribe failed: %d", err);
    }
}

static void mqtt_incoming_publish_cb(void *arg, const char *topic, __unused u32_t tot_len)
{
    node_b_state_t *state = (node_b_state_t *) arg;

    if (state == NULL) {
        return;
    }

    strncpy(state->topic_buffer, topic, sizeof(state->topic_buffer) - 1);
    state->topic_buffer[sizeof(state->topic_buffer) - 1] = '\0';
}

static void mqtt_incoming_data_cb(void *arg, const u8_t *data, u16_t len, __unused u8_t flags)
{
    node_b_state_t *state = (node_b_state_t *) arg;
    char normalized[32];

    if (state == NULL) {
        return;
    }

    if (len >= sizeof(state->payload_buffer)) {
        len = sizeof(state->payload_buffer) - 1;
    }

    memcpy(state->payload_buffer, data, len);
    state->payload_buffer[len] = '\0';

    trim_copy(normalized, sizeof(normalized), state->payload_buffer);
    node_b_log("[NODE_B] RX %s %s", state->topic_buffer, normalized);

    if (strcmp(state->topic_buffer, NODE_B_TOPIC_CMD_LIGHT) == 0) {
        if (equals_ignore_case(normalized, "ON")) {
            set_lamp_state(true, "mqtt");
        } else if (equals_ignore_case(normalized, "OFF")) {
            set_lamp_state(false, "mqtt");
        } else {
            node_b_log("[NODE_B] WARN invalid light payload: %s", normalized);
        }
        return;
    }

    if (strcmp(state->topic_buffer, NODE_B_TOPIC_CMD_WINDOW) == 0) {
        if (equals_ignore_case(normalized, "OPEN")) {
            set_window_state(true, "mqtt");
        } else if (equals_ignore_case(normalized, "CLOSE") || equals_ignore_case(normalized, "CLOSED")) {
            set_window_state(false, "mqtt");
        } else {
            node_b_log("[NODE_B] WARN invalid window payload: %s", normalized);
        }
    }
}

static void subscribe_required_topics(void)
{
    mqtt_sub_unsub(g_state.client, NODE_B_TOPIC_CMD_LIGHT, MQTT_QOS, mqtt_subscribe_request_cb, &g_state, true);
    mqtt_sub_unsub(g_state.client, NODE_B_TOPIC_CMD_WINDOW, MQTT_QOS, mqtt_subscribe_request_cb, &g_state, true);
}

static void mqtt_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status)
{
    node_b_state_t *state = (node_b_state_t *) arg;

    (void) client;

    if (state == NULL) {
        return;
    }

    if (status != MQTT_CONNECT_ACCEPTED) {
        state->mqtt_ready = false;
        node_b_log("[NODE_B] WARN MQTT connect failed: %d", status);
        return;
    }

    state->mqtt_ready = true;
    node_b_log("[NODE_B] MQTT connected");

    mqtt_set_inpub_callback(state->client, mqtt_incoming_publish_cb, mqtt_incoming_data_cb, state);
    subscribe_required_topics();
    publish_status_snapshot();
    publish_heartbeat();
}

static void start_mqtt_client(void)
{
    err_t err;

    if (g_state.client != NULL) {
        return;
    }

    g_state.client = mqtt_client_new();
    if (g_state.client == NULL) {
        panic("mqtt_client_new failed");
    }

    cyw43_arch_lwip_begin();
    err = mqtt_client_connect(g_state.client, &g_state.server_addr, MQTT_PORT, mqtt_connection_cb, &g_state, &g_state.client_info);
    cyw43_arch_lwip_end();

    if (err != ERR_OK) {
        panic("mqtt_client_connect failed");
    }
}

static void dns_found_cb(__unused const char *hostname, const ip_addr_t *ipaddr, void *arg)
{
    node_b_state_t *state = (node_b_state_t *) arg;

    if (state == NULL) {
        return;
    }

    if (ipaddr == NULL) {
        panic("mqtt dns lookup failed");
    }

    state->server_addr = *ipaddr;
    start_mqtt_client();
}

static void configure_client_info(void)
{
    char unique_id[2 * PICO_UNIQUE_BOARD_ID_SIZE_BYTES + 1];

    pico_get_unique_board_id_string(unique_id, sizeof(unique_id));
    snprintf(g_state.client_id, sizeof(g_state.client_id), "node-b-%s", unique_id);

    g_state.client_info.client_id = g_state.client_id;
    g_state.client_info.keep_alive = MQTT_KEEP_ALIVE_S;
    g_state.client_info.client_user = MQTT_USERNAME[0] != '\0' ? MQTT_USERNAME : NULL;
    g_state.client_info.client_pass = MQTT_PASSWORD[0] != '\0' ? MQTT_PASSWORD : NULL;
}

int main(void)
{
    err_t dns_err;

    memset(&g_state, 0, sizeof(g_state));

    stdio_init_all();
    sleep_ms(2000);

    led_init();
    servo_init();
    servo_set_open(false);

    node_b_log("[NODE_B] Boot start");
    node_b_log("[NODE_B] Broker %s:%d", MQTT_SERVER, MQTT_PORT);

    if (cyw43_arch_init()) {
        panic("cyw43_arch_init failed");
    }
    g_cyw43_inited = true;

    cyw43_arch_enable_sta_mode();
    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 30000)) {
        panic("Wi-Fi connect failed");
    }
    g_state.wifi_ready = true;
    node_b_log("[NODE_B] WIFI OK");

    configure_client_info();

    cyw43_arch_lwip_begin();
    dns_err = dns_gethostbyname(MQTT_SERVER, &g_state.server_addr, dns_found_cb, &g_state);
    cyw43_arch_lwip_end();

    if (dns_err == ERR_OK) {
        start_mqtt_client();
    } else if (dns_err != ERR_INPROGRESS) {
        panic("dns_gethostbyname failed");
    }

    while (true) {
        uint64_t now_ms = to_ms_since_boot(get_absolute_time());

        if (g_state.mqtt_ready && now_ms - g_state.last_heartbeat_ms >= NODE_B_HEARTBEAT_INTERVAL_MS) {
            g_state.last_heartbeat_ms = now_ms;
            publish_heartbeat();
        }

        cyw43_arch_poll();
        sleep_ms(20);
    }

    return 0;
}

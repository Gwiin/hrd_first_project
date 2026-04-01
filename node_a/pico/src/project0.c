#include "node_a_config.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "hardware/adc.h"
#include "hardware/gpio.h"
#include "hardware/sync.h"
#include "lwip/apps/mqtt.h"
#include "lwip/dns.h"
#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"
#include "pico/unique_id.h"

#ifndef WIFI_SSID
#define WIFI_SSID NODE_A_DEFAULT_WIFI_SSID
#endif

#ifndef WIFI_PASSWORD
#define WIFI_PASSWORD NODE_A_DEFAULT_WIFI_PASSWORD
#endif

#ifndef MQTT_SERVER
#define MQTT_SERVER NODE_A_DEFAULT_MQTT_SERVER
#endif

#ifndef MQTT_USERNAME
#define MQTT_USERNAME NODE_A_DEFAULT_MQTT_USERNAME
#endif

#ifndef MQTT_PASSWORD
#define MQTT_PASSWORD NODE_A_DEFAULT_MQTT_PASSWORD
#endif

#ifndef MQTT_PORT
#define MQTT_PORT NODE_A_DEFAULT_MQTT_PORT
#endif

#ifndef MQTT_KEEP_ALIVE_S
#define MQTT_KEEP_ALIVE_S NODE_A_DEFAULT_MQTT_KEEP_ALIVE_S
#endif

#ifndef MQTT_QOS
#define MQTT_QOS NODE_A_DEFAULT_MQTT_QOS
#endif

#define NODE_A_TOPIC_ENV "house/env"
#define NODE_A_TOPIC_HEARTBEAT "house/heartbeat/nodeA"

#define DHT_PIN 15
#define CDS_GPIO 27
#define CDS_ADC_CHANNEL 1

#define NODE_A_LOG_BUFFER_SIZE 160
#define NODE_A_HEARTBEAT_INTERVAL_MS 2000
#define NODE_A_SENSOR_INTERVAL_MS 2000
#define NODE_A_LIGHT_SCALE_MAX 400

typedef struct {
    mqtt_client_t *client;
    ip_addr_t server_addr;
    struct mqtt_connect_client_info_t client_info;
    char client_id[32];
    bool mqtt_ready;
    bool wifi_ready;
    uint64_t last_heartbeat_ms;
    uint64_t last_sensor_publish_ms;
} node_a_state_t;

static node_a_state_t g_state;

static uint16_t normalize_light(uint16_t raw)
{
    return (uint16_t) ((raw * NODE_A_LIGHT_SCALE_MAX) / 4095u);
}

static void node_a_log(const char *fmt, ...)
{
    char buffer[NODE_A_LOG_BUFFER_SIZE];
    va_list args;

    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    printf("%s\n", buffer);
}

static bool wait_for_level(bool level, uint32_t timeout_us)
{
    uint32_t start = time_us_32();

    while (gpio_get(DHT_PIN) != level) {
        if (time_us_32() - start > timeout_us) {
            return false;
        }
    }

    return true;
}

static bool read_dht(float *temperature, float *humidity)
{
    uint8_t data[5] = {0};
    uint32_t irq_state = save_and_disable_interrupts();

    gpio_set_dir(DHT_PIN, GPIO_OUT);
    gpio_put(DHT_PIN, 0);
    sleep_ms(20);
    gpio_put(DHT_PIN, 1);
    sleep_us(30);
    gpio_set_dir(DHT_PIN, GPIO_IN);

    if (!wait_for_level(0, 100) ||
        !wait_for_level(1, 100) ||
        !wait_for_level(0, 100)) {
        goto fail;
    }

    for (int i = 0; i < 40; ++i) {
        if (!wait_for_level(1, 70)) {
            goto fail;
        }

        sleep_us(30);

        if (gpio_get(DHT_PIN)) {
            data[i / 8] |= (1u << (7 - (i % 8)));
        }

        if (!wait_for_level(0, 70)) {
            goto fail;
        }
    }

    restore_interrupts(irq_state);

    if ((uint8_t) (data[0] + data[1] + data[2] + data[3]) != data[4]) {
        node_a_log("[NODE_A] WARN DHT checksum fail");
        return false;
    }

    *humidity = (float) data[0] + ((float) data[1] / 10.0f);
    *temperature = (float) data[2] + ((float) data[3] / 10.0f);
    return true;

fail:
    restore_interrupts(irq_state);
    node_a_log("[NODE_A] WARN DHT signal timeout");
    return false;
}

static void mqtt_publish_request_cb(__unused void *arg, err_t err)
{
    if (err != ERR_OK) {
        node_a_log("[NODE_A] WARN mqtt publish failed: %d", err);
    }
}

static void node_a_publish(const char *topic, const char *payload)
{
    if (g_state.client == NULL || !g_state.mqtt_ready) {
        return;
    }

    cyw43_arch_lwip_begin();
    mqtt_publish(g_state.client, topic, payload, strlen(payload), MQTT_QOS, 0, mqtt_publish_request_cb, &g_state);
    cyw43_arch_lwip_end();
}

static void publish_heartbeat(void)
{
    node_a_publish(NODE_A_TOPIC_HEARTBEAT, "alive");
}

static void publish_environment(uint16_t light_raw, float temperature, float humidity)
{
    char payload[96];
    uint16_t normalized_light = normalize_light(light_raw);

    snprintf(payload, sizeof(payload), "light=%u,temp=%.1f,humidity=%.1f", normalized_light, temperature, humidity);
    node_a_log("[NODE_A] PUB raw=%u normalized=%u", light_raw, normalized_light);
    node_a_log("[NODE_A] PUB %s", payload);
    node_a_publish(NODE_A_TOPIC_ENV, payload);
}

static void mqtt_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status)
{
    node_a_state_t *state = (node_a_state_t *) arg;

    (void) client;

    if (state == NULL) {
        return;
    }

    if (status != MQTT_CONNECT_ACCEPTED) {
        state->mqtt_ready = false;
        node_a_log("[NODE_A] WARN MQTT connect failed: %d", status);
        return;
    }

    state->mqtt_ready = true;
    node_a_log("[NODE_A] MQTT connected");
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
    node_a_state_t *state = (node_a_state_t *) arg;

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
    snprintf(g_state.client_id, sizeof(g_state.client_id), "node-a-%s", unique_id);

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

    gpio_init(DHT_PIN);
    gpio_pull_up(DHT_PIN);

    adc_init();
    adc_gpio_init(CDS_GPIO);
    adc_select_input(CDS_ADC_CHANNEL);

    node_a_log("[NODE_A] Boot start");
    node_a_log("[NODE_A] Broker %s:%d", MQTT_SERVER, MQTT_PORT);

    if (cyw43_arch_init()) {
        panic("cyw43_arch_init failed");
    }

    cyw43_arch_enable_sta_mode();
    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 30000)) {
        panic("Wi-Fi connect failed");
    }
    g_state.wifi_ready = true;
    node_a_log("[NODE_A] WIFI OK");

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

        if (g_state.mqtt_ready && now_ms - g_state.last_heartbeat_ms >= NODE_A_HEARTBEAT_INTERVAL_MS) {
            g_state.last_heartbeat_ms = now_ms;
            publish_heartbeat();
        }

        if (g_state.mqtt_ready && now_ms - g_state.last_sensor_publish_ms >= NODE_A_SENSOR_INTERVAL_MS) {
            uint16_t light_raw = adc_read();
            float temperature = 0.0f;
            float humidity = 0.0f;

            g_state.last_sensor_publish_ms = now_ms;
            if (read_dht(&temperature, &humidity)) {
                publish_environment(light_raw, temperature, humidity);
            } else {
                node_a_log("[NODE_A] WARN env publish skipped due to DHT read failure");
            }
        }

        cyw43_arch_poll();
        sleep_ms(20);
    }

    return 0;
}

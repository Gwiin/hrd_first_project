#include "node_c_network.h"
#include "node_c_config.h"

#include <stdio.h>
#include <string.h>

#include "pico/cyw43_arch.h"
#include "lwip/apps/mqtt.h"
#include "lwip/apps/mqtt_priv.h"
#include "lwip/dns.h"

#ifndef WIFI_SSID
#define WIFI_SSID NODE_C_DEFAULT_WIFI_SSID
#endif

#ifndef WIFI_PASSWORD
#define WIFI_PASSWORD NODE_C_DEFAULT_WIFI_PASSWORD
#endif

#ifndef MQTT_SERVER
#define MQTT_SERVER NODE_C_DEFAULT_MQTT_SERVER
#endif

#ifndef MQTT_USERNAME
#define MQTT_USERNAME NODE_C_DEFAULT_MQTT_USERNAME
#endif

#ifndef MQTT_PASSWORD
#define MQTT_PASSWORD NODE_C_DEFAULT_MQTT_PASSWORD
#endif

#ifndef MQTT_PORT
#define MQTT_PORT NODE_C_DEFAULT_MQTT_PORT
#endif

#ifndef MQTT_KEEP_ALIVE_S
#define MQTT_KEEP_ALIVE_S NODE_C_DEFAULT_MQTT_KEEP_ALIVE_S
#endif

#ifndef MQTT_QOS
#define MQTT_QOS NODE_C_DEFAULT_MQTT_QOS
#endif

#define NODE_C_TOPIC_BUFFER_SIZE 64
#define NODE_C_PAYLOAD_BUFFER_SIZE 192
#define NODE_C_HEARTBEAT_INTERVAL_MS 2000

typedef struct {
    mqtt_client_t *client;
    ip_addr_t server_addr;
    struct mqtt_connect_client_info_t client_info;
    node_c_controller_t *controller;
    node_c_network_status_t *status;
    char topic_buffer[NODE_C_TOPIC_BUFFER_SIZE];
    char payload_buffer[NODE_C_PAYLOAD_BUFFER_SIZE];
} node_c_network_context_t;

static node_c_network_context_t g_network;
static bool g_cyw43_inited;

static void network_log(node_c_controller_t *controller, const char *fmt, const char *arg)
{
    char buffer[160];

    if (controller == NULL || controller->io.log == NULL) {
        return;
    }

    snprintf(buffer, sizeof(buffer), fmt, arg);
    controller->io.log(buffer, controller->io.user_data);
}

static void mqtt_publish_request_cb(__unused void *arg, err_t err)
{
    (void) arg;
    (void) err;
}

static void mqtt_subscribe_request_cb(void *arg, err_t err)
{
    node_c_network_context_t *ctx = (node_c_network_context_t *) arg;

    if (err != ERR_OK && ctx != NULL && ctx->controller != NULL && ctx->controller->io.log != NULL) {
        ctx->controller->io.log("[NODE_C] WARN mqtt subscribe failed", ctx->controller->io.user_data);
    }
}

static void node_c_network_publish_topic(const char *topic, const char *payload, void *user_data)
{
    node_c_network_context_t *ctx = (node_c_network_context_t *) user_data;

    if (ctx == NULL || ctx->client == NULL || ctx->status == NULL || !ctx->status->mqtt_ready) {
        return;
    }

    cyw43_arch_lwip_begin();
    mqtt_publish(ctx->client, topic, payload, strlen(payload), MQTT_QOS, 0, mqtt_publish_request_cb, ctx);
    cyw43_arch_lwip_end();
}

static void mqtt_incoming_data_cb(void *arg, const u8_t *data, u16_t len, u8_t flags)
{
    node_c_network_context_t *ctx = (node_c_network_context_t *) arg;

    (void) flags;

    if (ctx == NULL || ctx->controller == NULL) {
        return;
    }

    if (len >= sizeof(ctx->payload_buffer)) {
        len = sizeof(ctx->payload_buffer) - 1;
    }

    memcpy(ctx->payload_buffer, data, len);
    ctx->payload_buffer[len] = '\0';

    if (strcmp(ctx->topic_buffer, NODE_C_TOPIC_ENV) == 0) {
        node_c_controller_apply_env_payload(ctx->controller, ctx->payload_buffer, to_ms_since_boot(get_absolute_time()));
    } else if (strcmp(ctx->topic_buffer, NODE_C_TOPIC_MODE) == 0) {
        node_c_controller_apply_mode_payload(ctx->controller, ctx->payload_buffer, to_ms_since_boot(get_absolute_time()));
    } else if (strcmp(ctx->topic_buffer, NODE_C_TOPIC_CMD_LIGHT) == 0) {
        node_c_controller_apply_light_command_payload(ctx->controller, ctx->payload_buffer, to_ms_since_boot(get_absolute_time()));
    } else if (strcmp(ctx->topic_buffer, NODE_C_TOPIC_CMD_WINDOW) == 0) {
        node_c_controller_apply_window_command_payload(ctx->controller, ctx->payload_buffer, to_ms_since_boot(get_absolute_time()));
    } else if (strcmp(ctx->topic_buffer, NODE_C_TOPIC_STATUS_NODE_B) == 0) {
        node_c_controller_apply_node_b_status(ctx->controller, ctx->payload_buffer, to_ms_since_boot(get_absolute_time()));
    }
}

static void mqtt_incoming_publish_cb(void *arg, const char *topic, u32_t tot_len)
{
    node_c_network_context_t *ctx = (node_c_network_context_t *) arg;

    (void) tot_len;

    if (ctx == NULL) {
        return;
    }

    strncpy(ctx->topic_buffer, topic, sizeof(ctx->topic_buffer) - 1);
    ctx->topic_buffer[sizeof(ctx->topic_buffer) - 1] = '\0';
}

static void subscribe_required_topics(node_c_network_context_t *ctx)
{
    mqtt_sub_unsub(ctx->client, NODE_C_TOPIC_ENV, MQTT_QOS, mqtt_subscribe_request_cb, ctx, true);
    mqtt_sub_unsub(ctx->client, NODE_C_TOPIC_MODE, MQTT_QOS, mqtt_subscribe_request_cb, ctx, true);
    mqtt_sub_unsub(ctx->client, NODE_C_TOPIC_CMD_LIGHT, MQTT_QOS, mqtt_subscribe_request_cb, ctx, true);
    mqtt_sub_unsub(ctx->client, NODE_C_TOPIC_CMD_WINDOW, MQTT_QOS, mqtt_subscribe_request_cb, ctx, true);
    mqtt_sub_unsub(ctx->client, NODE_C_TOPIC_STATUS_NODE_B, MQTT_QOS, mqtt_subscribe_request_cb, ctx, true);
    mqtt_sub_unsub(ctx->client, NODE_C_TOPIC_HEARTBEAT_NODE_A, MQTT_QOS, mqtt_subscribe_request_cb, ctx, true);
    mqtt_sub_unsub(ctx->client, NODE_C_TOPIC_HEARTBEAT_NODE_B, MQTT_QOS, mqtt_subscribe_request_cb, ctx, true);
}

static void mqtt_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status)
{
    node_c_network_context_t *ctx = (node_c_network_context_t *) arg;

    (void) client;

    if (ctx == NULL || ctx->status == NULL || ctx->controller == NULL) {
        return;
    }

    if (status == MQTT_CONNECT_ACCEPTED) {
        ctx->status->mqtt_ready = true;
        ctx->controller->io.log("[NODE_C] MQTT connected", ctx->controller->io.user_data);
        subscribe_required_topics(ctx);
        mqtt_set_inpub_callback(ctx->client, mqtt_incoming_publish_cb, mqtt_incoming_data_cb, ctx);
    } else {
        ctx->status->mqtt_ready = false;
        ctx->controller->io.log("[NODE_C] WARN MQTT connect failed", ctx->controller->io.user_data);
    }
}

static void start_mqtt_client(node_c_network_context_t *ctx)
{
    err_t err;

    if (ctx == NULL || ctx->client != NULL) {
        return;
    }

    ctx->client = mqtt_client_new();
    if (ctx->client == NULL) {
        ctx->controller->io.log("[NODE_C] WARN mqtt client alloc failed", ctx->controller->io.user_data);
        return;
    }

    cyw43_arch_lwip_begin();
    err = mqtt_client_connect(ctx->client, &ctx->server_addr, MQTT_PORT, mqtt_connection_cb, ctx, &ctx->client_info);
    cyw43_arch_lwip_end();

    if (err != ERR_OK) {
        ctx->controller->io.log("[NODE_C] WARN mqtt connect request failed", ctx->controller->io.user_data);
    }
}

static void dns_found_cb(const char *hostname, const ip_addr_t *ipaddr, void *arg)
{
    node_c_network_context_t *ctx = (node_c_network_context_t *) arg;

    (void) hostname;

    if (ctx == NULL || ctx->controller == NULL) {
        return;
    }

    if (ipaddr == NULL) {
        ctx->controller->io.log("[NODE_C] WARN mqtt server dns failed", ctx->controller->io.user_data);
        return;
    }

    ctx->server_addr = *ipaddr;
    start_mqtt_client(ctx);
}

bool node_c_network_is_enabled(void)
{
    return WIFI_SSID[0] != '\0' && MQTT_SERVER[0] != '\0';
}

bool node_c_network_init(node_c_controller_t *controller, node_c_network_status_t *status)
{
    err_t dns_err;

    if (controller == NULL || status == NULL) {
        return false;
    }

    memset(status, 0, sizeof(*status));
    memset(&g_network, 0, sizeof(g_network));

    status->enabled = node_c_network_is_enabled();
    if (!status->enabled) {
        controller->io.log("[NODE_C] Network disabled. Set WIFI_SSID/WIFI_PASSWORD/MQTT_SERVER to enable MQTT mode.", controller->io.user_data);
        return true;
    }

    if (cyw43_arch_init()) {
        controller->io.log("[NODE_C] WARN cyw43 init failed", controller->io.user_data);
        return false;
    }
    g_cyw43_inited = true;

    cyw43_arch_enable_sta_mode();

    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 30000)) {
        controller->io.log("[NODE_C] WARN Wi-Fi connection failed", controller->io.user_data);
        return false;
    }

    status->wifi_ready = true;
    controller->io.log("[NODE_C] WIFI OK", controller->io.user_data);

    g_network.controller = controller;
    g_network.status = status;
    g_network.client_info.client_id = "node-c";
    g_network.client_info.keep_alive = MQTT_KEEP_ALIVE_S;
    if (MQTT_USERNAME[0] != '\0') {
        g_network.client_info.client_user = MQTT_USERNAME;
    }
    if (MQTT_PASSWORD[0] != '\0') {
        g_network.client_info.client_pass = MQTT_PASSWORD;
    }

    controller->io.publish = node_c_network_publish_topic;
    controller->io.user_data = &g_network;

    cyw43_arch_lwip_begin();
    dns_err = dns_gethostbyname(MQTT_SERVER, &g_network.server_addr, dns_found_cb, &g_network);
    cyw43_arch_lwip_end();

    if (dns_err == ERR_OK) {
        start_mqtt_client(&g_network);
    } else if (dns_err != ERR_INPROGRESS) {
        network_log(controller, "[NODE_C] WARN dns start failed: %s", MQTT_SERVER);
    }

    return true;
}

void node_c_network_poll(node_c_controller_t *controller, node_c_network_status_t *status, uint64_t now_ms)
{
    if (controller == NULL || status == NULL || !status->enabled) {
        return;
    }

    if (status->mqtt_ready && now_ms - status->last_heartbeat_ms >= NODE_C_HEARTBEAT_INTERVAL_MS) {
        status->last_heartbeat_ms = now_ms;
        controller->io.publish(NODE_C_TOPIC_HEARTBEAT_NODE_C, "alive", controller->io.user_data);
    }
}

void node_c_network_deinit(node_c_network_status_t *status)
{
    if (status != NULL) {
        memset(status, 0, sizeof(*status));
    }

    if (g_network.client != NULL) {
        cyw43_arch_lwip_begin();
        mqtt_disconnect(g_network.client);
        cyw43_arch_lwip_end();
        g_network.client = NULL;
    }

    if (g_cyw43_inited) {
        cyw43_arch_deinit();
        g_cyw43_inited = false;
    }
}

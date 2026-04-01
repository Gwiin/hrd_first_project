#include "node_c_controller.h"

#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#define NODE_C_LOG_BUFFER_SIZE 192

static void publish_mode(node_c_controller_t *controller);
static void publish_status_snapshot(node_c_controller_t *controller);
static void evaluate_auto_rules(node_c_controller_t *controller);

static void node_c_logf(node_c_controller_t *controller, const char *fmt, ...)
{
    char buffer[NODE_C_LOG_BUFFER_SIZE];
    va_list args;

    if (controller == NULL || controller->io.log == NULL) {
        return;
    }

    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    controller->io.log(buffer, controller->io.user_data);
}

static void node_c_publish(node_c_controller_t *controller, const char *topic, const char *payload)
{
    if (controller != NULL && controller->io.publish != NULL) {
        controller->io.publish(topic, payload, controller->io.user_data);
    }
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

static void lowercase_in_place(char *text)
{
    size_t i = 0;

    while (text[i] != '\0') {
        text[i] = (char) tolower((unsigned char) text[i]);
        i++;
    }
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

static bool ensure_manual_mode(node_c_controller_t *controller, const char *source, const char *payload)
{
    if (controller->mode == NODE_C_MODE_MANUAL) {
        return true;
    }

    node_c_logf(controller, "[NODE_C] WARN manual command ignored in AUTO mode from %s: %s", source, payload);
    return false;
}

static const char *mode_name(node_c_mode_t mode)
{
    return mode == NODE_C_MODE_MANUAL ? "MANUAL" : "AUTO";
}

static const char *lamp_name(node_c_switch_state_t state)
{
    return state == NODE_C_SWITCH_ON ? "ON" : "OFF";
}

static const char *window_name(node_c_window_state_t state)
{
    return state == NODE_C_WINDOW_OPEN ? "OPEN" : "CLOSE";
}

static bool set_mode(node_c_controller_t *controller, node_c_mode_t next_mode, const char *reason, bool publish_topic)
{
    if (controller == NULL) {
        return false;
    }

    if (controller->mode == next_mode) {
        return true;
    }

    controller->mode = next_mode;
    if (reason != NULL) {
        node_c_logf(controller, "[NODE_C] MODE SOURCE %s", reason);
    }
    if (publish_topic) {
        publish_mode(controller);
    } else {
        node_c_logf(controller, "[NODE_C] MODE %s", mode_name(controller->mode));
    }
    if (next_mode == NODE_C_MODE_AUTO) {
        evaluate_auto_rules(controller);
    }
    publish_status_snapshot(controller);
    return true;
}

static void publish_mode(node_c_controller_t *controller)
{
    node_c_publish(controller, NODE_C_TOPIC_MODE, mode_name(controller->mode));
    node_c_logf(controller, "[NODE_C] MODE %s", mode_name(controller->mode));
}

static void publish_status_snapshot(node_c_controller_t *controller)
{
    char payload[160];

    snprintf(payload,
        sizeof(payload),
        "mode=%s,light=%d,temp=%.1f,humidity=%.1f,lamp=%s,window=%s",
        mode_name(controller->mode),
        controller->env.light,
        controller->env.temp,
        controller->env.humidity,
        lamp_name(controller->lamp_state),
        window_name(controller->window_state));

    node_c_publish(controller, NODE_C_TOPIC_STATUS_NODE_C, payload);
}

static void set_lamp_state(node_c_controller_t *controller, node_c_switch_state_t next_state, const char *reason)
{
    if (controller->lamp_state == next_state) {
        return;
    }

    controller->lamp_state = next_state;
    node_c_publish(controller, NODE_C_TOPIC_CMD_LIGHT, lamp_name(next_state));
    node_c_logf(controller, "[NODE_C] CMD LIGHT %s (%s)", lamp_name(next_state), reason);
}

static void set_window_state(node_c_controller_t *controller, node_c_window_state_t next_state, const char *reason)
{
    if (controller->window_state == next_state) {
        return;
    }

    controller->window_state = next_state;
    node_c_publish(controller, NODE_C_TOPIC_CMD_WINDOW, window_name(next_state));
    node_c_logf(controller, "[NODE_C] CMD WINDOW %s (%s)", window_name(next_state), reason);
}

static void evaluate_auto_rules(node_c_controller_t *controller)
{
    if (controller->mode != NODE_C_MODE_AUTO || !controller->env.valid) {
        return;
    }

    if (controller->env.light < controller->thresholds.light_on_threshold) {
        set_lamp_state(controller, NODE_C_SWITCH_ON, "light below threshold");
    } else if (controller->env.light > controller->thresholds.light_off_threshold) {
        set_lamp_state(controller, NODE_C_SWITCH_OFF, "light above threshold");
    }

    if (controller->env.temp > controller->thresholds.window_open_temp_threshold ||
        controller->env.humidity > controller->thresholds.window_open_humidity_threshold) {
        set_window_state(controller, NODE_C_WINDOW_OPEN, "temp or humidity high");
    } else if (controller->env.temp <= controller->thresholds.window_close_temp_threshold &&
               controller->env.humidity <= controller->thresholds.window_close_humidity_threshold) {
        set_window_state(controller, NODE_C_WINDOW_CLOSED, "temp and humidity normal");
    }
}

static void log_status(node_c_controller_t *controller)
{
    if (!controller->env.valid) {
        node_c_logf(controller,
            "[NODE_C] MODE %s | waiting env data | lamp=%s window=%s",
            mode_name(controller->mode),
            lamp_name(controller->lamp_state),
            window_name(controller->window_state));
        return;
    }

    node_c_logf(controller,
        "[NODE_C] MODE %s | light=%d temp=%.1f humidity=%.1f | lamp=%s window=%s",
        mode_name(controller->mode),
        controller->env.light,
        controller->env.temp,
        controller->env.humidity,
        lamp_name(controller->lamp_state),
        window_name(controller->window_state));
}

void node_c_controller_init(node_c_controller_t *controller, const node_c_io_t *io)
{
    if (controller == NULL) {
        return;
    }

    memset(controller, 0, sizeof(*controller));
    controller->thresholds.light_on_threshold = 280;
    controller->thresholds.light_off_threshold = 320;
    controller->thresholds.window_open_temp_threshold = 28.0f;
    controller->thresholds.window_open_humidity_threshold = 70.0f;
    controller->thresholds.window_close_temp_threshold = 27.0f;
    controller->thresholds.window_close_humidity_threshold = 65.0f;
    controller->thresholds.node_a_timeout_ms = 5000;
    controller->thresholds.node_b_timeout_ms = 5000;
    controller->thresholds.status_interval_ms = 1000;
    controller->mode = NODE_C_MODE_AUTO;
    controller->lamp_state = NODE_C_SWITCH_OFF;
    controller->window_state = NODE_C_WINDOW_CLOSED;

    if (io != NULL) {
        controller->io = *io;
    }
}

void node_c_controller_publish_boot_state(node_c_controller_t *controller)
{
    if (controller == NULL) {
        return;
    }

    node_c_logf(controller, "[NODE_C] BOOT OK");
    publish_mode(controller);
    publish_status_snapshot(controller);
}

bool node_c_controller_apply_env_payload(node_c_controller_t *controller, const char *payload, uint64_t now_ms)
{
    int light = 0;
    float temp = 0.0f;
    float humidity = 0.0f;

    if (controller == NULL || payload == NULL) {
        return false;
    }

    if (sscanf(payload, "light=%d,temp=%f,humidity=%f", &light, &temp, &humidity) != 3) {
        node_c_logf(controller, "[NODE_C] WARN invalid env payload: %s", payload);
        return false;
    }

    controller->env.light = light;
    controller->env.temp = temp;
    controller->env.humidity = humidity;
    controller->env.valid = true;
    controller->env.last_update_ms = now_ms;
    controller->node_a_timeout_reported = false;

    node_c_logf(controller, "[NODE_C] RX ENV light=%d temp=%.1f humidity=%.1f", light, temp, humidity);
    evaluate_auto_rules(controller);
    publish_status_snapshot(controller);
    return true;
}

bool node_c_controller_apply_node_b_status(node_c_controller_t *controller, const char *payload, uint64_t now_ms)
{
    char lamp[16];
    char window[16];

    if (controller == NULL || payload == NULL) {
        return false;
    }

    lamp[0] = '\0';
    window[0] = '\0';

    if (sscanf(payload, "lamp=%15[^,],window=%15s", lamp, window) < 2) {
        node_c_logf(controller, "[NODE_C] WARN invalid nodeB payload: %s", payload);
        return false;
    }

    if (equals_ignore_case(lamp, "ON")) {
        controller->lamp_state = NODE_C_SWITCH_ON;
    } else if (equals_ignore_case(lamp, "OFF")) {
        controller->lamp_state = NODE_C_SWITCH_OFF;
    }

    if (equals_ignore_case(window, "OPEN")) {
        controller->window_state = NODE_C_WINDOW_OPEN;
    } else if (equals_ignore_case(window, "CLOSE") || equals_ignore_case(window, "CLOSED")) {
        controller->window_state = NODE_C_WINDOW_CLOSED;
    }

    controller->last_node_b_update_ms = now_ms;
    controller->node_b_timeout_reported = false;
    node_c_logf(controller, "[NODE_C] RX NODE_B lamp=%s window=%s", lamp_name(controller->lamp_state), window_name(controller->window_state));
    publish_status_snapshot(controller);
    return true;
}

bool node_c_controller_apply_mode_payload(node_c_controller_t *controller, const char *payload, uint64_t now_ms)
{
    char normalized[32];

    (void) now_ms;

    if (controller == NULL || payload == NULL) {
        return false;
    }

    trim_copy(normalized, sizeof(normalized), payload);
    lowercase_in_place(normalized);

    if (normalized[0] == '\0') {
        return false;
    }

    if (strcmp(normalized, "auto") == 0 || strcmp(normalized, "mode auto") == 0 || strcmp(normalized, "mode_auto") == 0) {
        return set_mode(controller, NODE_C_MODE_AUTO, "mqtt", false);
    }

    if (strcmp(normalized, "manual") == 0 || strcmp(normalized, "mode manual") == 0 || strcmp(normalized, "mode_manual") == 0) {
        return set_mode(controller, NODE_C_MODE_MANUAL, "mqtt", false);
    }

    node_c_logf(controller, "[NODE_C] WARN invalid mode payload: %s", normalized);
    return false;
}

bool node_c_controller_apply_light_command_payload(node_c_controller_t *controller, const char *payload, uint64_t now_ms)
{
    char normalized[32];

    (void) now_ms;

    if (controller == NULL || payload == NULL) {
        return false;
    }

    trim_copy(normalized, sizeof(normalized), payload);
    lowercase_in_place(normalized);

    if (normalized[0] == '\0') {
        return false;
    }

    if (!ensure_manual_mode(controller, "mqtt light", normalized)) {
        return false;
    }

    if (strcmp(normalized, "on") == 0) {
        controller->lamp_state = NODE_C_SWITCH_ON;
        node_c_logf(controller, "[NODE_C] CMD LIGHT ON (mqtt)");
        publish_status_snapshot(controller);
        return true;
    }

    if (strcmp(normalized, "off") == 0) {
        controller->lamp_state = NODE_C_SWITCH_OFF;
        node_c_logf(controller, "[NODE_C] CMD LIGHT OFF (mqtt)");
        publish_status_snapshot(controller);
        return true;
    }

    node_c_logf(controller, "[NODE_C] WARN invalid light payload: %s", normalized);
    return false;
}

bool node_c_controller_apply_window_command_payload(node_c_controller_t *controller, const char *payload, uint64_t now_ms)
{
    char normalized[32];

    (void) now_ms;

    if (controller == NULL || payload == NULL) {
        return false;
    }

    trim_copy(normalized, sizeof(normalized), payload);
    lowercase_in_place(normalized);

    if (normalized[0] == '\0') {
        return false;
    }

    if (!ensure_manual_mode(controller, "mqtt window", normalized)) {
        return false;
    }

    if (strcmp(normalized, "open") == 0) {
        controller->window_state = NODE_C_WINDOW_OPEN;
        node_c_logf(controller, "[NODE_C] CMD WINDOW OPEN (mqtt)");
        publish_status_snapshot(controller);
        return true;
    }

    if (strcmp(normalized, "close") == 0 || strcmp(normalized, "closed") == 0) {
        controller->window_state = NODE_C_WINDOW_CLOSED;
        node_c_logf(controller, "[NODE_C] CMD WINDOW CLOSE (mqtt)");
        publish_status_snapshot(controller);
        return true;
    }

    node_c_logf(controller, "[NODE_C] WARN invalid window payload: %s", normalized);
    return false;
}

bool node_c_controller_handle_uart_command(node_c_controller_t *controller, const char *command, uint64_t now_ms)
{
    char normalized[64];

    (void) now_ms;

    if (controller == NULL || command == NULL) {
        return false;
    }

    trim_copy(normalized, sizeof(normalized), command);
    lowercase_in_place(normalized);

    if (normalized[0] == '\0') {
        return false;
    }

    if (strcmp(normalized, "mode auto") == 0 || strcmp(normalized, "mode_auto") == 0) {
        return set_mode(controller, NODE_C_MODE_AUTO, "uart", true);
    }

    if (strcmp(normalized, "mode manual") == 0 || strcmp(normalized, "mode_manual") == 0) {
        return set_mode(controller, NODE_C_MODE_MANUAL, "uart", true);
    }

    if (!ensure_manual_mode(controller, "uart", normalized)) {
        return false;
    }

    if (strcmp(normalized, "light on") == 0 || strcmp(normalized, "light_on") == 0) {
        set_lamp_state(controller, NODE_C_SWITCH_ON, "manual");
        publish_status_snapshot(controller);
        return true;
    }

    if (strcmp(normalized, "light off") == 0 || strcmp(normalized, "light_off") == 0) {
        set_lamp_state(controller, NODE_C_SWITCH_OFF, "manual");
        publish_status_snapshot(controller);
        return true;
    }

    if (strcmp(normalized, "window open") == 0 || strcmp(normalized, "window_open") == 0) {
        set_window_state(controller, NODE_C_WINDOW_OPEN, "manual");
        publish_status_snapshot(controller);
        return true;
    }

    if (strcmp(normalized, "window close") == 0 || strcmp(normalized, "window_close") == 0) {
        set_window_state(controller, NODE_C_WINDOW_CLOSED, "manual");
        publish_status_snapshot(controller);
        return true;
    }

    node_c_logf(controller, "[NODE_C] WARN unknown command: %s", normalized);
    return false;
}

void node_c_controller_periodic(node_c_controller_t *controller, uint64_t now_ms)
{
    if (controller == NULL) {
        return;
    }

    if (controller->env.valid &&
        !controller->node_a_timeout_reported &&
        now_ms - controller->env.last_update_ms > controller->thresholds.node_a_timeout_ms) {
        controller->node_a_timeout_reported = true;
        node_c_logf(controller, "[NODE_C] WARN node A timeout");
    }

    if (controller->last_node_b_update_ms != 0 &&
        !controller->node_b_timeout_reported &&
        now_ms - controller->last_node_b_update_ms > controller->thresholds.node_b_timeout_ms) {
        controller->node_b_timeout_reported = true;
        node_c_logf(controller, "[NODE_C] WARN node B no response");
    }

    if (now_ms - controller->last_status_log_ms >= controller->thresholds.status_interval_ms) {
        controller->last_status_log_ms = now_ms;
        log_status(controller);
        publish_status_snapshot(controller);
    }
}

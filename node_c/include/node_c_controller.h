#ifndef NODE_C_CONTROLLER_H
#define NODE_C_CONTROLLER_H

#include <stdbool.h>
#include <stdint.h>

#define NODE_C_TOPIC_ENV "house/env"
#define NODE_C_TOPIC_MODE "house/mode"
#define NODE_C_TOPIC_CMD_LIGHT "house/cmd/light"
#define NODE_C_TOPIC_CMD_WINDOW "house/cmd/window"
#define NODE_C_TOPIC_STATUS_NODE_B "house/status/nodeB"
#define NODE_C_TOPIC_STATUS_NODE_C "house/status/nodeC"
#define NODE_C_TOPIC_HEARTBEAT_NODE_A "house/heartbeat/nodeA"
#define NODE_C_TOPIC_HEARTBEAT_NODE_B "house/heartbeat/nodeB"
#define NODE_C_TOPIC_HEARTBEAT_NODE_C "house/heartbeat/nodeC"

typedef enum {
    NODE_C_MODE_AUTO = 0,
    NODE_C_MODE_MANUAL = 1
} node_c_mode_t;

typedef enum {
    NODE_C_SWITCH_OFF = 0,
    NODE_C_SWITCH_ON = 1
} node_c_switch_state_t;

typedef enum {
    NODE_C_WINDOW_CLOSED = 0,
    NODE_C_WINDOW_OPEN = 1
} node_c_window_state_t;

typedef struct {
    int light_on_threshold;
    int light_off_threshold;
    float window_open_temp_threshold;
    float window_open_humidity_threshold;
    float window_close_temp_threshold;
    float window_close_humidity_threshold;
    uint32_t node_a_timeout_ms;
    uint32_t node_b_timeout_ms;
    uint32_t status_interval_ms;
} node_c_thresholds_t;

typedef struct {
    int light;
    float temp;
    float humidity;
    bool valid;
    uint64_t last_update_ms;
} node_c_environment_t;

typedef void (*node_c_publish_fn)(const char *topic, const char *payload, void *user_data);
typedef void (*node_c_log_fn)(const char *message, void *user_data);

typedef struct {
    node_c_publish_fn publish;
    node_c_log_fn log;
    void *user_data;
} node_c_io_t;

typedef struct {
    node_c_thresholds_t thresholds;
    node_c_environment_t env;
    node_c_mode_t mode;
    node_c_switch_state_t lamp_state;
    node_c_window_state_t window_state;
    uint64_t last_node_b_update_ms;
    uint64_t last_status_log_ms;
    bool node_a_timeout_reported;
    bool node_b_timeout_reported;
    node_c_io_t io;
} node_c_controller_t;

void node_c_controller_init(node_c_controller_t *controller, const node_c_io_t *io);
bool node_c_controller_apply_env_payload(node_c_controller_t *controller, const char *payload, uint64_t now_ms);
bool node_c_controller_apply_node_b_status(node_c_controller_t *controller, const char *payload, uint64_t now_ms);
bool node_c_controller_handle_uart_command(node_c_controller_t *controller, const char *command, uint64_t now_ms);
void node_c_controller_periodic(node_c_controller_t *controller, uint64_t now_ms);
void node_c_controller_publish_boot_state(node_c_controller_t *controller);

#endif

#ifndef NODE_C_NETWORK_H
#define NODE_C_NETWORK_H

#include <stdbool.h>
#include <stdint.h>

#include "node_c_controller.h"

typedef struct {
    bool enabled;
    bool wifi_ready;
    bool mqtt_ready;
    uint64_t last_heartbeat_ms;
} node_c_network_status_t;

bool node_c_network_init(node_c_controller_t *controller, node_c_network_status_t *status);
void node_c_network_poll(node_c_controller_t *controller, node_c_network_status_t *status, uint64_t now_ms);
void node_c_network_deinit(node_c_network_status_t *status);
bool node_c_network_is_enabled(void);

#endif

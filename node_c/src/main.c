#include "node_c_controller.h"
#include "node_c_network.h"

#include <stdio.h>

#include "pico/stdlib.h"

#define NODE_C_COMMAND_BUFFER_SIZE 96
#define NODE_C_DEMO_INTERVAL_MS 3000

static uint64_t now_ms(void)
{
    return to_ms_since_boot(get_absolute_time());
}

static void uart_log(const char *message, void *user_data)
{
    (void) user_data;
    printf("%s\n", message);
}

static void mqtt_publish_stub(const char *topic, const char *payload, void *user_data)
{
    (void) user_data;
    printf("[NODE_C] MQTT PUB topic=%s payload=%s\n", topic, payload);
}

static bool try_read_uart_line(char *buffer, size_t buffer_size)
{
    int ch = 0;
    static size_t index = 0;

    while ((ch = getchar_timeout_us(0)) != PICO_ERROR_TIMEOUT) {
        if (ch == '\r' || ch == '\n') {
            if (index == 0) {
                continue;
            }

            buffer[index] = '\0';
            index = 0;
            return true;
        }

        if (index + 1 < buffer_size) {
            buffer[index++] = (char) ch;
        }
    }

    return false;
}

int main(void)
{
    static const char *demo_payloads[] = {
        "light=260,temp=26.5,humidity=60.0",
        "light=350,temp=26.0,humidity=55.0",
        "light=310,temp=29.3,humidity=72.0",
        "light=290,temp=26.5,humidity=63.0"
    };
    const size_t demo_payload_count = sizeof(demo_payloads) / sizeof(demo_payloads[0]);
    size_t demo_index = 0;
    uint64_t last_demo_ms = 0;
    char command_buffer[NODE_C_COMMAND_BUFFER_SIZE];
    node_c_controller_t controller;
    node_c_network_status_t network_status = {0};
    node_c_io_t io = {
        .publish = mqtt_publish_stub,
        .log = uart_log,
        .user_data = NULL,
    };

    stdio_init_all();
    sleep_ms(2000);

    node_c_controller_init(&controller, &io);
    node_c_controller_publish_boot_state(&controller);
    if (!node_c_network_init(&controller, &network_status)) {
        network_status.enabled = false;
        printf("[NODE_C] Network init failed. Running in demo mode.\n");
    } else if (!network_status.enabled) {
        printf("[NODE_C] Demo mode enabled. Type commands: mode manual, light on, window open, mode auto\n");
    }

    while (true) {
        const uint64_t current_ms = now_ms();

        if (try_read_uart_line(command_buffer, sizeof(command_buffer))) {
            node_c_controller_handle_uart_command(&controller, command_buffer, current_ms);
        }

        if (!network_status.enabled && current_ms - last_demo_ms >= NODE_C_DEMO_INTERVAL_MS) {
            last_demo_ms = current_ms;
            node_c_controller_apply_env_payload(&controller, demo_payloads[demo_index], current_ms);
            demo_index = (demo_index + 1) % demo_payload_count;
        }

        node_c_network_poll(&controller, &network_status, current_ms);
        node_c_controller_periodic(&controller, current_ms);
        sleep_ms(50);
    }
}

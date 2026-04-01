#include "node_c_controller.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define EVENT_BUFFER_SIZE 64
#define EVENT_TOPIC_SIZE 64
#define EVENT_PAYLOAD_SIZE 160

typedef struct {
    char topic[EVENT_TOPIC_SIZE];
    char payload[EVENT_PAYLOAD_SIZE];
} publish_event_t;

typedef struct {
    publish_event_t events[EVENT_BUFFER_SIZE];
    size_t event_count;
} test_context_t;

static void test_log(const char *message, void *user_data)
{
    (void) user_data;
    printf("%s\n", message);
}

static void test_publish(const char *topic, const char *payload, void *user_data)
{
    test_context_t *ctx = (test_context_t *) user_data;
    publish_event_t *event = NULL;

    assert(ctx != NULL);
    assert(ctx->event_count < EVENT_BUFFER_SIZE);

    event = &ctx->events[ctx->event_count++];
    snprintf(event->topic, sizeof(event->topic), "%s", topic);
    snprintf(event->payload, sizeof(event->payload), "%s", payload);

    printf("[TEST PUB] topic=%s payload=%s\n", topic, payload);
}

static void reset_events(test_context_t *ctx)
{
    ctx->event_count = 0;
    memset(ctx->events, 0, sizeof(ctx->events));
}

static bool has_event(const test_context_t *ctx, const char *topic, const char *payload)
{
    size_t i = 0;

    for (i = 0; i < ctx->event_count; ++i) {
        if (strcmp(ctx->events[i].topic, topic) == 0 &&
            strcmp(ctx->events[i].payload, payload) == 0) {
            return true;
        }
    }

    return false;
}

int main(void)
{
    test_context_t ctx = {0};
    node_c_controller_t controller;
    node_c_io_t io = {
        .publish = test_publish,
        .log = test_log,
        .user_data = &ctx,
    };

    node_c_controller_init(&controller, &io);
    node_c_controller_publish_boot_state(&controller);

    assert(has_event(&ctx, NODE_C_TOPIC_MODE, "AUTO"));
    assert(has_event(&ctx, NODE_C_TOPIC_STATUS_NODE_C, "mode=AUTO,light=0,temp=0.0,humidity=0.0,lamp=OFF,window=CLOSE"));

    reset_events(&ctx);
    assert(node_c_controller_apply_env_payload(&controller, "light=250,temp=29.5,humidity=72.0", 1000));
    assert(controller.lamp_state == NODE_C_SWITCH_ON);
    assert(controller.window_state == NODE_C_WINDOW_OPEN);
    assert(has_event(&ctx, NODE_C_TOPIC_CMD_LIGHT, "ON"));
    assert(has_event(&ctx, NODE_C_TOPIC_CMD_WINDOW, "OPEN"));

    reset_events(&ctx);
    assert(node_c_controller_apply_env_payload(&controller, "light=350,temp=26.0,humidity=55.0", 2000));
    assert(controller.lamp_state == NODE_C_SWITCH_OFF);
    assert(controller.window_state == NODE_C_WINDOW_CLOSED);
    assert(has_event(&ctx, NODE_C_TOPIC_CMD_LIGHT, "OFF"));
    assert(has_event(&ctx, NODE_C_TOPIC_CMD_WINDOW, "CLOSE"));

    reset_events(&ctx);
    assert(node_c_controller_handle_uart_command(&controller, "mode manual", 3000));
    assert(controller.mode == NODE_C_MODE_MANUAL);
    assert(has_event(&ctx, NODE_C_TOPIC_MODE, "MANUAL"));

    reset_events(&ctx);
    assert(node_c_controller_apply_mode_payload(&controller, "AUTO", 3050));
    assert(controller.mode == NODE_C_MODE_AUTO);
    assert(!has_event(&ctx, NODE_C_TOPIC_MODE, "AUTO"));
    assert(has_event(&ctx, NODE_C_TOPIC_STATUS_NODE_C, "mode=AUTO,light=350,temp=26.0,humidity=55.0,lamp=OFF,window=CLOSE"));

    reset_events(&ctx);
    assert(node_c_controller_apply_mode_payload(&controller, "manual", 3075));
    assert(controller.mode == NODE_C_MODE_MANUAL);
    assert(!has_event(&ctx, NODE_C_TOPIC_MODE, "MANUAL"));
    assert(has_event(&ctx, NODE_C_TOPIC_STATUS_NODE_C, "mode=MANUAL,light=350,temp=26.0,humidity=55.0,lamp=OFF,window=CLOSE"));

    reset_events(&ctx);
    assert(node_c_controller_handle_uart_command(&controller, "light on", 3100));
    assert(controller.lamp_state == NODE_C_SWITCH_ON);
    assert(has_event(&ctx, NODE_C_TOPIC_CMD_LIGHT, "ON"));

    reset_events(&ctx);
    assert(node_c_controller_apply_light_command_payload(&controller, "OFF", 3150));
    assert(controller.lamp_state == NODE_C_SWITCH_OFF);
    assert(has_event(&ctx, NODE_C_TOPIC_STATUS_NODE_C, "mode=MANUAL,light=350,temp=26.0,humidity=55.0,lamp=OFF,window=CLOSE"));

    reset_events(&ctx);
    assert(node_c_controller_handle_uart_command(&controller, "window open", 3200));
    assert(controller.window_state == NODE_C_WINDOW_OPEN);
    assert(has_event(&ctx, NODE_C_TOPIC_CMD_WINDOW, "OPEN"));

    reset_events(&ctx);
    assert(node_c_controller_apply_window_command_payload(&controller, "close", 3250));
    assert(controller.window_state == NODE_C_WINDOW_CLOSED);
    assert(has_event(&ctx, NODE_C_TOPIC_STATUS_NODE_C, "mode=MANUAL,light=350,temp=26.0,humidity=55.0,lamp=OFF,window=CLOSE"));

    reset_events(&ctx);
    assert(node_c_controller_handle_uart_command(&controller, "mode auto", 3300));
    assert(controller.mode == NODE_C_MODE_AUTO);
    assert(has_event(&ctx, NODE_C_TOPIC_MODE, "AUTO"));

    reset_events(&ctx);
    assert(!node_c_controller_apply_light_command_payload(&controller, "ON", 3325));

    reset_events(&ctx);
    assert(!node_c_controller_apply_window_command_payload(&controller, "OPEN", 3350));

    reset_events(&ctx);
    assert(node_c_controller_apply_node_b_status(&controller, "lamp=OFF,window=CLOSE", 3400));
    assert(controller.lamp_state == NODE_C_SWITCH_OFF);
    assert(controller.window_state == NODE_C_WINDOW_CLOSED);

    reset_events(&ctx);
    node_c_controller_periodic(&controller, 9000);
    assert(has_event(&ctx, NODE_C_TOPIC_STATUS_NODE_C, "mode=AUTO,light=350,temp=26.0,humidity=55.0,lamp=OFF,window=CLOSE"));

    puts("[TEST] controller smoke test passed");
    return 0;
}

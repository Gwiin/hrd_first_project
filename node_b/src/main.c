#include <stdio.h>

#include "hardware/pwm.h"
#include "pico/stdlib.h"

#include "node_b.h"

#define SERVO_PIN 16
#define PWM_WRAP 20000
#define SERVO_MIN_US 500.0f
#define SERVO_MAX_US 2400.0f

static uint servo_slice;

void servo_init(void) {
    gpio_set_function(SERVO_PIN, GPIO_FUNC_PWM);
    servo_slice = pwm_gpio_to_slice_num(SERVO_PIN);

    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, 125.0f);
    pwm_config_set_wrap(&config, PWM_WRAP - 1);
    pwm_init(servo_slice, &config, true);
}

void servo_set_angle(float angle_deg) {
    float clamped_angle = angle_deg;
    float pulse_us;
    uint16_t level;

    if (clamped_angle < 0.0f) {
        clamped_angle = 0.0f;
    }
    if (clamped_angle > 180.0f) {
        clamped_angle = 180.0f;
    }

    pulse_us = SERVO_MIN_US +
               ((SERVO_MAX_US - SERVO_MIN_US) * (clamped_angle / 180.0f));
    level = (uint16_t)pulse_us;

    pwm_set_gpio_level(SERVO_PIN, level);
}

void node_b_run(void) {
    const float test_angles[] = {0.0f, 45.0f, 90.0f, 135.0f, 180.0f, 90.0f};
    const size_t angle_count = sizeof(test_angles) / sizeof(test_angles[0]);
    size_t i;

    servo_init();
    sleep_ms(1500);

    while (true) {
        for (i = 0; i < angle_count; ++i) {
            servo_set_angle(test_angles[i]);
            printf("servo angle: %.1f deg\n", test_angles[i]);
            sleep_ms(1500);
        }
    }
}

int main(void) {
    stdio_init_all();
    sleep_ms(2000);

    printf("Pico 2 W SG90 servo test start\n");
    printf("GPIO %d -> SG90 signal\n", SERVO_PIN);

    node_b_run();
    return 0;
}

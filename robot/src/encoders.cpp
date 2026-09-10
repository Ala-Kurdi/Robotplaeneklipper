#include "../include/encoders.hpp"

#include <gpiod.h>
#include <atomic>
#include <thread>
#include <iostream>

// Encoder GPIO
static const unsigned int LEFT_ENCODER_GPIO = 24;
static const unsigned int RIGHT_ENCODER_GPIO = 25;

// Kalibrering
const double PULSES_PER_REV = 22.0;
const double WHEEL_DIAMETER_CM = 6.5;
const double PI = 3.14159;
const double WHEEL_CIRCUMFERENCE_CM = PI * WHEEL_DIAMETER_CM;
const double CM_PER_PULSE = WHEEL_CIRCUMFERENCE_CM / PULSES_PER_REV;

// Tællere
static std::atomic<long> left_count{0};
static std::atomic<long> right_count{0};

void add_left_pulse() {
    left_count++;
}

void add_right_pulse() {
    right_count++;
}

void encoder_thread() {
    gpiod_chip* chip = gpiod_chip_open("/dev/gpiochip0");

    if (!chip) {
        std::cerr << "[Encoder] Kunne ikke åbne /dev/gpiochip0" << std::endl;
        return;
    }

    gpiod_line_settings* settings = gpiod_line_settings_new();

    gpiod_line_settings_set_direction(
        settings,
        GPIOD_LINE_DIRECTION_INPUT
    );

    gpiod_line_settings_set_edge_detection(
        settings,
        GPIOD_LINE_EDGE_RISING
    );

    unsigned int offsets[] = {
        LEFT_ENCODER_GPIO,
        RIGHT_ENCODER_GPIO
    };

    gpiod_line_config* line_config = gpiod_line_config_new();

    if (gpiod_line_config_add_line_settings(
            line_config,
            offsets,
            2,
            settings) < 0) {

        std::cerr << "[Encoder] Kunne ikke konfigurere GPIO-linjer" << std::endl;
        return;
    }

    gpiod_request_config* request_config = gpiod_request_config_new();

    gpiod_request_config_set_consumer(
        request_config,
        "robot-encoders"
    );

    gpiod_line_request* request =
        gpiod_chip_request_lines(
            chip,
            request_config,
            line_config
        );

    if (!request) {
        std::cerr << "[Encoder] Kunne ikke reservere GPIO24/GPIO25" << std::endl;
        return;
    }

    gpiod_edge_event_buffer* buffer =
        gpiod_edge_event_buffer_new(16);

    std::cout
        << "[Encoder] GPIO24 og GPIO25 aktive."
        << std::endl;

    while (true) {

        int ret = gpiod_line_request_wait_edge_events(
            request,
            1000000000
        );

        if (ret <= 0) {
            continue;
        }

        int num_events =
            gpiod_line_request_read_edge_events(
                request,
                buffer,
                16
            );

        for (int i = 0; i < num_events; i++) {

            gpiod_edge_event* event =
                gpiod_edge_event_buffer_get_event(
                    buffer,
                    i
                );

            unsigned int gpio =
                gpiod_edge_event_get_line_offset(event);

            if (gpio == LEFT_ENCODER_GPIO) {
                add_left_pulse();
            }
            else if (gpio == RIGHT_ENCODER_GPIO) {
                add_right_pulse();
            }
        }
    }
}

void init_encoders() {
    std::thread thread(encoder_thread);
    thread.detach();
}

double get_left_distance_cm() {
    return left_count.load() * CM_PER_PULSE;
}

double get_right_distance_cm() {
    return right_count.load() * CM_PER_PULSE;
}

double get_average_distance_cm() {
    return (
        get_left_distance_cm() +
        get_right_distance_cm()
    ) / 2.0;
}

void reset_encoders() {
    left_count = 0;
    right_count = 0;
}

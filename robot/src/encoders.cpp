#include "../include/encoders.hpp"

// Konstanter
const double PULSES_PER_REV = 22.0;
const double WHEEL_DIAMETER_CM = 6.5;
const double PI = 3.14159;
const double WHEEL_CIRCUMFERENCE_CM = PI * WHEEL_DIAMETER_CM;
const double CM_PER_PULSE = WHEEL_CIRCUMFERENCE_CM / PULSES_PER_REV;

// Interne tællere
static long left_count = 0;
static long right_count = 0;

void add_left_pulse() {
    left_count++;
}

void add_right_pulse() {
    right_count++;
}

double get_left_distance_cm() {
    return left_count * CM_PER_PULSE;
}

double get_right_distance_cm() {
    return right_count * CM_PER_PULSE;
}

double get_average_distance_cm() {
    return (get_left_distance_cm() + get_right_distance_cm()) / 2.0;
}

void reset_encoders() {
    left_count = 0;
    right_count = 0;
}
#ifndef ENCODERS_HPP
#define ENCODERS_HPP

void init_encoders();

void add_left_pulse();
void add_right_pulse();

double get_left_distance_cm();
double get_right_distance_cm();
double get_average_distance_cm();

void reset_encoders();

#endif

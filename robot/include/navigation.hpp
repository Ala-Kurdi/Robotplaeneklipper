#ifndef NAVIGATION_HPP
#define NAVIGATION_HPP

#include <string>

struct Vector2D {
    double x;
    double y;

    Vector2D operator+(const Vector2D& other) const {
        return {x + other.x, y + other.y};
    }
};

void init_navigation();
void update_position(double delta_distance_cm);
void update_heading(const std::string& turn_direction);
bool is_at_boundary();

Vector2D get_position();
int get_heading();
int get_obstacle_count();
void register_obstacle_hit();

int get_coverage_percentage();
int get_last_checkpoint();
double get_distance_to_home();
int get_angle_to_home();

#endif
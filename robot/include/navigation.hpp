#ifndef NAVIGATION_HPP
#define NAVIGATION_HPP

void init_navigation();
bool is_at_boundary(double current_distance_cm);
void register_obstacle_hit();
int get_obstacle_count();

#endif
#ifndef LIDAR_HPP
#define LIDAR_HPP

#include <string>

int open_lidar_port();
std::string check_obstacle_cpp(int fd);

#endif
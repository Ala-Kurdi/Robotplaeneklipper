#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>

struct AppConfig {
    std::string mqtt_host;
    int mqtt_port;
    double map_width_cm;
    double map_height_cm;
    double grid_cell_size_cm;
    double wheel_diameter_cm;
    double pulses_per_rev;
};

bool load_config(const std::string& filepath);
const AppConfig& get_config();

#endif
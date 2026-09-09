#include "../include/config.hpp"
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

static AppConfig global_config;

bool load_config(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "[Config] Kunne ikke åbne " << filepath << ". Bruger standardværdier." << std::endl;
        return false;
    }

    try {
        json j;
        file >> j;

        // Læs MQTT værdier
        global_config.mqtt_host = j["mqtt"]["host"].get<std::string>();
        global_config.mqtt_port = j["mqtt"]["port"].get<int>();

        // Læs Kort / Gitter værdier
        global_config.map_width_cm = j["map"]["width_cm"].get<double>();
        global_config.map_height_cm = j["map"]["height_cm"].get<double>();
        global_config.grid_cell_size_cm = j["map"]["grid_cell_size_cm"].get<double>();

        // Læs Hardware værdier
        global_config.wheel_diameter_cm = j["robot"]["wheel_diameter_cm"].get<double>();
        global_config.pulses_per_rev = j["robot"]["pulses_per_rev"].get<double>();

        std::cout << "[Config] Konfiguration indlæst succesfuldt fra " << filepath << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "[Config] Fejl ved parsing af JSON: " << e.what() << std::endl;
        return false;
    }

    return true;
}

const AppConfig& get_config() {
    return global_config;
}
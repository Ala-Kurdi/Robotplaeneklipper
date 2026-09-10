#include <iostream>
#include <unistd.h>
#include "../include/config.hpp"
#include "../include/motor.hpp"
#include "../include/lidar.hpp"
#include "../include/encoders.hpp"
#include "../include/navigation.hpp"
#include "../include/network.hpp"

void turn_90_degrees(const std::string& direction) {
    motor_stop();
    usleep(200000);

    if (direction == "right") motor_right();
    else motor_left();
    
    usleep(600000); 
    motor_stop();
    update_heading(direction);
    usleep(200000);
}

int main() {
    std::cout << "[System] Starter robotplæneklipper..." << std::endl;

    if (!load_config("config.json")) {
        std::cerr << "[Advarsel] Kunne ikke indlæse config.json." << std::endl;
    }

    auto cfg = get_config();

    init_navigation();
    reset_encoders();

    int lidar_fd = open_lidar_port();
    struct mosquitto* mqtt_client = setup_mqtt(cfg.mqtt_host, cfg.mqtt_port);

    double last_dist = get_average_distance_cm();

    while (true) {
        // Hent batteriniveau
        int current_battery = get_battery_level();

        if (current_battery <= 20) {
            motor_stop();
            std::cout << "[SIKKERHEDSSTOP] Batteri er lavt (" << current_battery << "%). Stopper normal drift!" << std::endl;
            
            if (mqtt_client) {
                Vector2D pos = get_position();
                send_telemetry(mqtt_client, pos.x, pos.y, get_heading(), 
                               get_coverage_percentage(), get_obstacle_count(), current_battery, "LOW_BATTERY_STOP");
            }

            usleep(5000000);
            continue;
        }

        if (is_robot_running()) {
            
            double current_dist = get_average_distance_cm();
            double delta = current_dist - last_dist;
            if (delta > 0) {
                update_position(delta);
                last_dist = current_dist;
            }

            std::string obstacle = check_obstacle_cpp(lidar_fd);

            if (mqtt_client) {
                Vector2D pos = get_position();
                send_telemetry(mqtt_client, pos.x, pos.y, get_heading(), 
                               get_coverage_percentage(), get_obstacle_count(), current_battery, obstacle);
            }

            if (obstacle != "") {
                motor_stop();
                register_obstacle_hit();
                
                motor_backward();
                usleep(500000);
                motor_stop();

                if (obstacle == "left") turn_90_degrees("right");
                else if (obstacle == "right") turn_90_degrees("left");
                else if (obstacle == "center") {
                    turn_90_degrees("right");
                    turn_90_degrees("right");
                }
                last_dist = get_average_distance_cm();
            } 
            else if (is_at_boundary()) {
                motor_stop();
                motor_backward();
                usleep(500000);
                motor_stop();
                turn_90_degrees("right");
                last_dist = get_average_distance_cm();
            } 
            else {
                motor_forward();
            }

        } else {
            motor_stop();
            last_dist = get_average_distance_cm();

            if (mqtt_client) {
                Vector2D pos = get_position();
                send_telemetry(mqtt_client, pos.x, pos.y, get_heading(), 
                               get_coverage_percentage(), get_obstacle_count(), current_battery, "IDLE");
            }
        }

        usleep(50000); 
    }

    if (lidar_fd >= 0) close(lidar_fd);
    return 0;
}
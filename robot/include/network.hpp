#ifndef NETWORK_HPP
#define NETWORK_HPP

#include <string>
#include <mosquitto.h>

bool is_robot_running();
int get_battery_level();
struct mosquitto* setup_mqtt(const std::string& host, int port);
void send_telemetry(struct mosquitto* mosq, double x, double y, int heading, int coverage, int obstacles_hit, int battery_pct, const std::string& obstacle_status);

#endif
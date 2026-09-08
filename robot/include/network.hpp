#ifndef NETWORK_HPP
#define NETWORK_HPP

#include <string>
#include <mosquitto.h>

bool is_robot_running();
struct mosquitto* setup_mqtt(const std::string& host, int port);
void send_telemetry(struct mosquitto* mosq, double distance_cm, const std::string& obstacle_status);

#endif
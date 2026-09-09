#include "../include/network.hpp"
#include <iostream>
#include <mosquitto.h>

static bool g_is_running = false;

bool is_robot_running() { return g_is_running; }

void on_connect(struct mosquitto* mosq, void* obj, int rc) {
    if (rc == 0) {
        std::cout << "[MQTT] Forbundet! Abonnerer på 'robot/commands'..." << std::endl;
        mosquitto_subscribe(mosq, NULL, "robot/commands", 1);
    } else {
        std::cerr << "[MQTT] Forbindelsesfejl med kode: " << rc << std::endl;
    }
}

void on_message(struct mosquitto* mosq, void* obj, const struct mosquitto_message* msg) {
    if (msg->payload) {
        std::string command(static_cast<char*>(msg->payload), msg->payloadlen);
        while (!command.empty() && (command.back() == '\n' || command.back() == '\r' || command.back() == ' ')) {
            command.pop_back();
        }
        
        std::cout << "\n[MQTT] Modtog kommando: " << command << std::endl;
        if (command == "START") {
            g_is_running = true;
        } else if (command == "STOP") {
            g_is_running = false;
        }
    }
}

struct mosquitto* setup_mqtt(const std::string& host, int port) {
    mosquitto_lib_init();
    struct mosquitto* mosq = mosquitto_new("robot_client", true, NULL);
    if (!mosq) return nullptr;

    mosquitto_connect_set_callback(mosq, on_connect);
    mosquitto_message_callback_set(mosq, on_message);

    if (mosquitto_connect(mosq, host.c_str(), port, 60) != MOSQ_ERR_SUCCESS) {
        mosquitto_destroy(mosq);
        return nullptr;
    }

    mosquitto_loop_start(mosq);
    return mosq;
}

void send_telemetry(struct mosquitto* mosq, double x, double y, int heading, int coverage, int obstacles_hit, const std::string& obstacle_status) {
    if (!mosq) return;

    std::string payload = "{\"x\": " + std::to_string(x) + 
                          ", \"y\": " + std::to_string(y) +
                          ", \"heading\": " + std::to_string(heading) +
                          ", \"coverage_pct\": " + std::to_string(coverage) +
                          ", \"obstacles_hit\": " + std::to_string(obstacles_hit) +
                          ", \"obstacle\": \"" + obstacle_status + 
                          "\", \"running\": " + (g_is_running ? "true" : "false") + "}";

    mosquitto_publish(mosq, NULL, "robot/telemetry", payload.length(), payload.c_str(), 1, false);
}
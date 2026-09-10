#include "../include/network.hpp"
#include <iostream>
#include <mosquitto.h>

static bool g_is_running = false;
static int g_battery_level = 100;

bool is_robot_running() { return g_is_running; }
int get_battery_level() { return g_battery_level; }

void on_connect(struct mosquitto* mosq, void* obj, int rc) {
    if (rc == 0) {
        std::cout << "[MQTT] Forbundet! Abonnerer på emner..." << std::endl;
        mosquitto_subscribe(mosq, NULL, "robot/commands", 1);
        mosquitto_subscribe(mosq, NULL, "robot/battery", 1);
    } else {
        std::cerr << "[MQTT] Forbindelsesfejl med kode: " << rc << std::endl;
    }
}

void on_message(struct mosquitto* mosq, void* obj, const struct mosquitto_message* msg) {
    if (!msg->payload) return;

    std::string topic(msg->topic);
    std::string payload_str(static_cast<char*>(msg->payload), msg->payloadlen);

    // Rens for linjeskift
    while (!payload_str.empty() && (payload_str.back() == '\n' || payload_str.back() == '\r' || payload_str.back() == ' ')) {
        payload_str.pop_back();
    }

    // Tjek hvilket emne besked kommer fra
    if (topic == "robot/commands") {
        std::cout << "\n[MQTT] Modtog kommando: " << payload_str << std::endl;
        if (payload_str == "START") {
            g_is_running = true;
        } else if (payload_str == "STOP") {
            g_is_running = false;
        }
    } 
    else if (topic == "robot/battery") {
        try {
            g_battery_level = std::stoi(payload_str);
            std::cout << "[MQTT] Modtog batteriniveau fra Arduino: " << g_battery_level << "%" << std::endl;
        } catch (...) {
            std::cerr << "[MQTT] Fejl ved udlæsning af batteri-payload: " << payload_str << std::endl;
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

void send_telemetry(struct mosquitto* mosq, double x, double y, int heading, int coverage, int obstacles_hit, int battery_pct, const std::string& obstacle_status) {
    if (!mosq) return;

    std::string payload = "{\"x\": " + std::to_string(x) + 
                          ", \"y\": " + std::to_string(y) +
                          ", \"heading\": " + std::to_string(heading) +
                          ", \"coverage_pct\": " + std::to_string(coverage) +
                          ", \"obstacles_hit\": " + std::to_string(obstacles_hit) +
                          ", \"battery_pct\": " + std::to_string(battery_pct) +
                          ", \"obstacle\": \"" + obstacle_status + 
                          "\", \"running\": " + (g_is_running ? "true" : "false") + "}";

    mosquitto_publish(mosq, NULL, "robot/telemetry", payload.length(), payload.c_str(), 1, false);
}
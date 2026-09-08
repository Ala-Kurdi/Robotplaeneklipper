#include "../include/network.hpp"
#include <iostream>
#include <mosquitto.h>

// Global variabel til at styre om robotten må køre
static bool g_is_running = false;

bool is_robot_running() {
    return g_is_running;
}

void on_connect(struct mosquitto* mosq, void* obj, int rc) {
    if (rc == 0) {
        std::cout << "[MQTT] Forbundet til broker! Abonnerer på kommandoer..." << std::endl;
        mosquitto_subscribe(mosq, NULL, "robot/commands", 1);
    } else {
        std::cerr << "[MQTT] Forbindelsesfejl med kode: " << rc << std::endl;
    }
}

void on_message(struct mosquitto* mosq, void* obj, const struct mosquitto_message* msg) {
    if (msg->payload) {
        std::string command(static_cast<char*>(msg->payload), msg->payloadlen);
        
        // Rens eventuelle linjeskift
        while (!command.empty() && (command.back() == '\n' || command.back() == '\r' || command.back() == ' ')) {
            command.pop_back();
        }
        
        std::cout << "\n[MQTT] Modtog kommando fra server: " << command << std::endl;
        if (command == "START") {
            g_is_running = true;
            std::cout << "[Robot] Status ændret til: KØRER (START)" << std::endl;
        } else if (command == "STOP") {
            g_is_running = false;
            std::cout << "[Robot] Status ændret til: STOPPET (STOP)" << std::endl;
        }
    }
}

struct mosquitto* setup_mqtt(const std::string& host, int port) {
    mosquitto_lib_init();
    struct mosquitto* mosq = mosquitto_new("robot_client", true, NULL);
    if (!mosq) {
        std::cerr << "[MQTT] Kunne ikke oprette klient struktur." << std::endl;
        return nullptr;
    }

    mosquitto_connect_set_callback(mosq, on_connect);
    mosquitto_message_callback_set(mosq, on_message);

    if (mosquitto_connect(mosq, host.c_str(), port, 60) != MOSQ_ERR_SUCCESS) {
        std::cerr << "[MQTT] Kunne ikke oprette forbindelse til " << host << std::endl;
        mosquitto_destroy(mosq);
        return nullptr;
    }

    // Start baggrundsloop
    mosquitto_loop_start(mosq);
    return mosq;
}

void send_telemetry(struct mosquitto* mosq, double distance_cm, const std::string& obstacle_status) {
    if (!mosq) return;

    // Byg en simpel JSON-streng
    std::string payload = "{\"distance_cm\": " + std::to_string(distance_cm) + 
                          ", \"obstacle\": \"" + obstacle_status + 
                          "\", \"running\": " + (g_is_running ? "true" : "false") + "}";

    mosquitto_publish(mosq, NULL, "robot/telemetry", payload.length(), payload.c_str(), 1, false);
}
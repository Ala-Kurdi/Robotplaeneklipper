#include <iostream>
#include <string>
#include <unistd.h>
#include <mosquitto.h>

// Definer variabler der matcher din eksisterende kodestruktur
int current_checkpoint = 1;
bool is_running = true;

void on_connect(struct mosquitto* mosq, void* obj, int rc) {
    if (rc == 0) {
        std::cout << "[MQTT] Forbundet til serverens broker!" << std::endl;
    } else {
        std::cerr << "[MQTT] Forbindelsesfejl med kode: " << rc << std::endl;
    }
}

int main() {
    // Initialiser Mosquitto biblioteket
    mosquitto_lib_init();
    
    struct mosquitto* mosq = mosquitto_new("pi_network_test", true, NULL);
    if (!mosq) {
        std::cerr << "[MQTT] Kunne ikke oprette klient struktur." << std::endl;
        return 1;
    }

    mosquitto_connect_callback_set(mosq, on_connect);

    const char* broker = "192.168.106.10";
    int port = 1883;

    std::cout << "[MQTT] Forsøger at forbinde til broker på " << broker << ":" << port << "..." << std::endl;

    if (mosquitto_connect(mosq, broker, port, 60) != MOSQ_ERR_SUCCESS) {
        std::cerr << "[MQTT] Kunne ikke oprette forbindelse til broker." << std::endl;
        mosquitto_destroy(mosq);
        mosquitto_lib_cleanup();
        return 1;
    }

    // Start baggrundsloop
    mosquitto_loop_start(mosq);

    // Send test-telemetri hvert 3. sekund i et loop
    for (int i = 0; i < 10; i++) {
        double distance = 12.5 + i;
        std::string obstacle = (i % 2 == 0) ? "none" : "front";

        std::string payload = "{\"distance_cm\": " + std::to_string(distance) + 
                              ", \"checkpoint\": " + std::to_string(current_checkpoint) +
                              ", \"obstacle\": \"" + obstacle + 
                              "\", \"running\": " + (is_running ? "true" : "false") + "}";

        int ret = mosquitto_publish(mosq, NULL, "robot/telemetry", payload.length(), payload.c_str(), 1, false);
        
        if (ret == MOSQ_ERR_SUCCESS) {
            std::cout << "[Sendt] " << payload << std::endl;
        } else {
            std::cerr << "[Fejl] Kunne ikke sende besked!" << std::endl;
        }

        sleep(3);
    }

    // Ryd op
    mosquitto_loop_stop(mosq, true);
    mosquitto_disconnect(mosq);
    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();

    std::cout << "[Test afsluttet]" << std::endl;
    return 0;
}

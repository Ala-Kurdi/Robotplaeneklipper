#include "../include/navigation.hpp"
#include <iostream>

// Interne variabler til at holde styr på robotternes tilstand i zonen
static double total_distance_traveled = 0.0;
static int obstacle_encounter_count = 0;
const double FIELD_SIZE_LIMIT_CM = 100.0; // Grænse for zonen

void init_navigation() {
    total_distance_traveled = 0.0;
    obstacle_encounter_count = 0;
    std::cout << "[Navigation] Reaktiv navigation initialiseret for 1x1m felt." << std::endl;
}

// Tjekker om robotten nærmer sig kanten af det tilladte område baseret på encodere
bool is_at_boundary(double current_distance_cm) {
    if (current_distance_cm >= FIELD_SIZE_LIMIT_CM) {
        return true;
    }
    return false;
}

// Registrerer at robotten har mødt en forhindring
void register_obstacle_hit() {
    obstacle_encounter_count++;
    std::cout << "[Navigation] Forhindring nr. " << obstacle_encounter_count << " registreret og undviget." << std::endl;
}

int get_obstacle_count() {
    return obstacle_encounter_count;
}
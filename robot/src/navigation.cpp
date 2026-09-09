#include "../include/navigation.hpp"
#include <iostream>
#include <cmath>

static Vector2D current_pos = {0.0, 0.0}; 
static int current_heading = 0; 
static int obstacle_encounter_count = 0;

const double MAP_MAX_X = 100.0;
const double MAP_MAX_Y = 100.0;
const int GRID_CELL_SIZE = 10;

static bool map_grid[10][10] = {false};
static int visited_cells = 0;
static int last_checkpoint_triggered = 0;

void init_navigation() {
    current_pos = {0.0, 0.0};
    current_heading = 0;
    obstacle_encounter_count = 0;
    visited_cells = 0;
    last_checkpoint_triggered = 0;

    for(int i=0; i<10; i++) {
        for(int j=0; j<10; j++) {
            map_grid[i][j] = false;
        }
    }
    std::cout << "[Navigation] Vektor-kort og gitter initialiseret." << std::endl;
}

void update_position(double delta_distance_cm) {
    double rad = current_heading * M_PI / 180.0;
    
    Vector2D movement = {
        delta_distance_cm * std::sin(rad),
        delta_distance_cm * std::cos(rad)
    };

    current_pos = current_pos + movement;
    if (current_pos.x < 0) current_pos.x = 0;
    if (current_pos.y < 0) current_pos.y = 0;
    if (current_pos.x > MAP_MAX_X) current_pos.x = MAP_MAX_X;
    if (current_pos.y > MAP_MAX_Y) current_pos.y = MAP_MAX_Y;

    // Gitter-kortlægning
    int grid_x = (int)(current_pos.x / GRID_CELL_SIZE);
    int grid_y = (int)(current_pos.y / GRID_CELL_SIZE);

    if (grid_x >= 0 && grid_x < 10 && grid_y >= 0 && grid_y < 10) {
        if (!map_grid[grid_x][grid_y]) {
            map_grid[grid_x][grid_y] = true;
            visited_cells++;
            
            if (visited_cells % 20 == 0 && visited_cells > last_checkpoint_triggered) {
                last_checkpoint_triggered = visited_cells;
                std::cout << "[Navigation] CHECKPOINT NÅET: " << visited_cells << "% dækket." << std::endl;
            }
        }
    }
}

void update_heading(const std::string& turn_direction) {
    if (turn_direction == "right") {
        current_heading = (current_heading + 90) % 360;
    } else if (turn_direction == "left") {
        current_heading = (current_heading - 90 + 360) % 360;
    }
}

bool is_at_boundary() {
    return (current_pos.x >= MAP_MAX_X || current_pos.y >= MAP_MAX_Y || current_pos.x <= 0 || current_pos.y <= 0);
}

void register_obstacle_hit() { obstacle_encounter_count++; }
Vector2D get_position() { return current_pos; }
int get_heading() { return current_heading; }
int get_obstacle_count() { return obstacle_encounter_count; }

int get_coverage_percentage() { return visited_cells; }
int get_last_checkpoint() { return last_checkpoint_triggered; }

double get_distance_to_home() {
    return std::sqrt(std::pow(current_pos.x, 2) + std::pow(current_pos.y, 2));
}

int get_angle_to_home() {
    double angle_rad = std::atan2(-current_pos.x, -current_pos.y);
    int angle_deg = (int)(angle_rad * 180.0 / M_PI);
    if (angle_deg < 0) angle_deg += 360;
    return angle_deg;
}
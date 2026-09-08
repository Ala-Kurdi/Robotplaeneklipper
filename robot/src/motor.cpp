#include "../include/motor.hpp"
#include <cstdlib>

void motor_stop() {
    // Stopper begge motorer
    std::system("pinctrl set 17 op dl");
    std::system("pinctrl set 27 op dl");
    std::system("pinctrl set 22 op dl");
    std::system("pinctrl set 23 op dl");
}

void motor_forward() {
    // Kører fremad
    std::system("pinctrl set 17 op dh");
    std::system("pinctrl set 27 op dl");
    std::system("pinctrl set 22 op dh");
    std::system("pinctrl set 23 op dl");
}

void motor_backward() {
    // Bakker
    std::system("pinctrl set 17 op dl");
    std::system("pinctrl set 27 op dh");
    std::system("pinctrl set 22 op dl");
    std::system("pinctrl set 23 op dh");
}

void motor_left() {
    // Drej til venstre
    std::system("pinctrl set 17 op dl");
    std::system("pinctrl set 27 op dh");
    std::system("pinctrl set 22 op dh");
    std::system("pinctrl set 23 op dl");
}

void motor_right() {
    // Drej til højre
    std::system("pinctrl set 17 op dh");
    std::system("pinctrl set 27 op dl");
    std::system("pinctrl set 22 op dl");
    std::system("pinctrl set 23 op dh");
}
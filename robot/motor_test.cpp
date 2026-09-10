#include <iostream>
#include <unistd.h>
#include "include/motor.hpp"

int main() {
    std::cout << "[Test] Starter direkte motor-test..." << std::endl;

    // FREM
    std::cout << "[Test] Kører FREM i 3 sekunder..." << std::endl;
    motor_forward();
    usleep(3000000);

    std::cout << "[Test] STOP..." << std::endl;
    motor_stop();
    usleep(1000000);

    // BAGUD
    std::cout << "[Test] Kører BAGUD i 3 sekunder..." << std::endl;
    motor_backward();
    usleep(3000000);

    std::cout << "[Test] STOP..." << std::endl;
    motor_stop();
    usleep(1000000);

    // VENSTRE
    std::cout << "[Test] Drejer VENSTRE i 2 sekunder..." << std::endl;
    motor_left();
    usleep(2000000);

    std::cout << "[Test] STOP..." << std::endl;
    motor_stop();
    usleep(1000000);

    // HØJRE
    std::cout << "[Test] Drejer HØJRE i 2 sekunder..." << std::endl;
    motor_right();
    usleep(2000000);

    std::cout << "[Test] STOP og afslutter..." << std::endl;
    motor_stop();

    std::cout << "[Test] Motor-test færdig." << std::endl;

    return 0;
}

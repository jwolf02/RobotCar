#include <iostream>
#include <L298NHBridge.hpp>
#include <cstdlib>

static constexpr int ENA = 20;
static constexpr int IN1 = 6;
static constexpr int IN2 = 13;
static constexpr int IN3 = 19;
static constexpr int IN4 = 26;
static constexpr int ENB = 16;

int main(int argc, const char *argv[]) {
    auto bridge = L298NHBridge(ENA, IN1, IN2, IN3, IN4, ENB);

    std::cout << "driving forward" << std::endl;
    bridge.set_motors(1.0, 1.0);
    sleep(1);

    std::cout << "driving backward" << std::endl;
    bridge.set_motors(-1.0, -1.0);
    sleep(1);

    std::cout << "turning right" << std::endl;
    bridge.set_motors(-1.0, 1.0);
    sleep(1);

    std::cout << "turning left" << std::endl;
    bridge.set_motors(1.0, -1.0);
    sleep(1);

    return EXIT_SUCCESS;
}

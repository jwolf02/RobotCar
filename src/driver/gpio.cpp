#include <gpio.hpp>
#include <stdexcept>

bool gpio::hardware_support() {
    #if defined(WIRING_PI) || defined(SYS_GPIO)
    return true;
    #else
    return false;
    #endif
}

void gpio::init() {
    static bool _init = false;
    if (_init)
        return;
    #if defined(WIRING_PI)
    if (wiringPiSetupGpio() < 0) {
        throw std::runtime_error("cannot setup wiringPi");
    }
    #elif defined(SYS_GPIO)
    if (setupGpio() < 0) {
        throw std::runtime_error("cannot setup sys_gpio");
    }
    #endif
    _init = true;
}
#include <HC_SR04.hpp>
#include <unistd.h>
#include <ctime>
#include <gpio/gpio.hpp>

HC_SR04::HC_SR04(int trigger, int echo) {
    gpio::init();
    _trigger = trigger;
    _echo = echo;
    gpio::setup(trigger, gpio::OUTPUT);
    gpio::setup(echo, gpio::OUTPUT);
    gpio::setup(echo, gpio::INPUT);
    sleep(2);
}

HC_SR04::~HC_SR04() {
    gpio::write(_trigger, gpio::LOW);
}

double HC_SR04::distance() {
    if (!gpio::hardware_support()) {
        return -1.0;
    }

    gpio::write(_trigger, gpio::HIGH);
    usleep(10);
    gpio::write(_trigger, gpio::LOW);

    clock_t pulse_start_time = 0, pulse_end_time = 0;

    while (gpio::read(_echo) == gpio::LOW) {
        pulse_start_time = clock();
    }

    while (gpio::read(_echo) == gpio::HIGH) {
        pulse_end_time = clock();
    }

    const double time_of_flight = double(pulse_end_time - pulse_start_time) / CLOCKS_PER_SEC;
    return time_of_flight * 17250.0;
}

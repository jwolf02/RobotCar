#include <HC_SR04.hpp>
#include <wiringPi.h>
#include <unistd.h>
#include <ctime>

HC_SR04::HC_SR04(int trigger, int echo) {
    wiringPiSetupGpio();
    _trigger = trigger;
    _echo = echo;
    pinMode(trigger, OUTPUT);
    pinMode(echo, INPUT);
    digitalWrite(trigger, LOW);
    sleep(2);
}

HC_SR04::~HC_SR04() {
    digitalWrite(_trigger, LOW);
}

double HC_SR04::distance() {
    digitalWrite(_trigger, HIGH);
    usleep(10);
    digitalWrite(_trigger, LOW);

    clock_t pulse_start_time = 0, pulse_end_time = 0;

    while (digitalRead(_echo) == LOW) {
        pulse_start_time = clock();
    }

    while (digitalRead(_echo) == HIGH) {
        pulse_end_time = clock();
    }

    const double time_of_flight = double(pulse_end_time - pulse_start_time) / CLOCKS_PER_SEC;
    return time_of_flight * 17250.0;
}

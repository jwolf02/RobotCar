#include <HC_SR04.hpp>
#include <unistd.h>
#include <ctime>

#ifdef RASPBERRY_PI
#include <wiringPi.h>
#endif

HC_SR04::HC_SR04(int trigger, int echo) {
    #ifdef RASPBERRY_PI
    wiringPiSetupGpio();
    _trigger = trigger;
    _echo = echo;
    pinMode(trigger, OUTPUT);
    pinMode(echo, INPUT);
    digitalWrite(trigger, LOW);
    sleep(2);
    #endif
}

HC_SR04::~HC_SR04() {
    #ifdef RASPBERRY_PI
    digitalWrite(_trigger, LOW);
    #endif
}

double HC_SR04::distance() {
    #ifdef RASPBERRY_PI
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
    #else
    return 0.0;
    #endif
}

#include "Servo.hpp"
#include <wiringPi.h>

Servo::Servo(int pin, double degrees) {
    _pin = pin;
    _max_degrees = degrees;
}

Servo::~Servo() {

}

double Servo::maxDegrees() const {

}

void Servo::move(double degrees) {

}

void Servo::reset() {
    move(0.0);
}
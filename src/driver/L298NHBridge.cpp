#include <L298NHBridge.hpp>
#include <stdexcept>
#include <cmath>
#include <gpio.hpp>

L298NHBridge::L298NHBridge(int ENA, int IN1, int IN2, int IN3, int IN4, int ENB, double min_speed) :
        ENA(ENA), IN1(IN1), IN2(IN2), IN3(IN3), IN4(IN4), ENB(ENB) {
    if (min_speed < 0.0 || min_speed > 1.0) {
        throw std::range_error("min_speed out of scope");
    } else {
        this->min_speed = min_speed;
    }

    gpio::init();
    gpio::pwm::create(ENA, 0, 100);
    gpio::setup(IN1, gpio::OUTPUT);
    gpio::setup(IN2, gpio::OUTPUT);
    gpio::setup(IN3, gpio::OUTPUT);
    gpio::setup(IN4, gpio::OUTPUT);
    gpio::pwm::create(ENB, 0, 100);
}

L298NHBridge::~L298NHBridge() {
    gpio::write(IN1, gpio::LOW);
    gpio::write(IN2, gpio::LOW);
    gpio::write(IN3, gpio::LOW);
    gpio::write(IN4, gpio::LOW);
    gpio::pwm::write(ENA, 0);
    gpio::pwm::write(ENB, 0);
}

void L298NHBridge::set_motor(int pin1, int pin2, int pwm, double speed) {
    if (speed < -1.0 || speed > 1.0) {
        throw std::range_error("speed value out of range");
    }

    if (speed > 0.0) {
        gpio::write(pin1, gpio::HIGH);
        gpio::write(pin2, gpio::LOW);
    } else if (speed < 0.0) {
        gpio::write(pin1, gpio::LOW);
        gpio::write(pin2, gpio::HIGH);
    } else {
        gpio::write(pin1, gpio::LOW);
        gpio::write(pin2, gpio::LOW);
    }

    const int value = speed != 0.0 ? int((std::abs(speed) * (1.0 - min_speed) + min_speed) * 100.0) : 0;
    gpio::pwm::write(pwm, value);
}

void L298NHBridge::set_motors(double motor_a_speed, double motor_b_speed) {
    set_motor(IN1, IN2, ENA, motor_a_speed);
    set_motor(IN3, IN4, ENB, motor_b_speed);
}

void L298NHBridge::stop_motors() {
    set_motors(0.0, 0.0);
}

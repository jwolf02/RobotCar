#include <L298NHBridge.hpp>
#include <stdexcept>
#include <cmath>

#include <gpio.hpp>

#define SETUP(pin, mode)            gpio::setup_pin(pin, mode)
#define WRITE(pin, val)             gpio::write(pin, val)
#define READ(pin, val)              gpio::read(pin, val)

#define PWM_CREATE(pin, init, range)    gpio::pwm::create(pin, init, range)
#define PWM_WRITE(pin, val)             gpio::pwm::set_value(pin, val)

void L298NHBridge::set_motor(int pin1, int pin2, int pwm, double speed) {
    if (speed < -1.0 || speed > 1.0) {
        throw std::range_error("speed value out of range");
    }

    if (speed > 0.0) {
        WRITE(pin1, 1);
        WRITE(pin2, 0);
    } else if (speed < 0.0) {
        WRITE(pin1, 0);
        WRITE(pin2, 1);
    } else {
        WRITE(pin1, 0);
        WRITE(pin2, 0);
    }

    int value = speed != 0.0 ? static_cast<int>((std::abs(speed) * (1.0 - min_speed) + min_speed) * 100) : 0.0;
    PWM_WRITE(pwm, value);
}

L298NHBridge::L298NHBridge(int ENA, int IN1, int IN2, int IN3, int IN4, int ENB, double min_speed) {
  this->ENA = ENA;
  this->IN1 = IN1;
  this->IN2 = IN2;
  this->IN3 = IN3;
  this->IN4 = IN4;
  this->ENB = ENB;
  if (min_speed < 0.0 || min_speed > 1.0)
    throw std::range_error("min_speed out of scope");
  else
    this->min_speed = min_speed;
  setup();
}

L298NHBridge::~L298NHBridge() {
    cleanup();
}

void L298NHBridge::cleanup() {
    WRITE(IN1, 0);
    WRITE(IN2, 0);
    WRITE(IN3, 0);
    WRITE(IN4, 0);
    PWM_WRITE(ENA, 0);
    PWM_WRITE(ENB, 0);
}

void L298NHBridge::setup() {
    SETUP(ENA, gpio::OUTPUT);
    SETUP(IN1, gpio::OUTPUT);
    SETUP(IN2, gpio::OUTPUT);
    SETUP(IN3, gpio::OUTPUT);
    SETUP(IN4, gpio::OUTPUT);
    SETUP(ENB, gpio::OUTPUT);
    PWM_CREATE(ENA, 0, 100);
    PWM_CREATE(ENB, 0, 100);
}

void L298NHBridge::set_motors(double motor_a_speed, double motor_b_speed) {
    set_motor(IN1, IN2, ENA, motor_a_speed);
    set_motor(IN3, IN4, ENB, motor_b_speed);
}

void L298NHBridge::stop_motors() {
  set_motors(0.0, 0.0);
}

#include <L298NHBridge.hpp>
#include <gpio.hpp>
#include <stdexcept>

#define ABS(x)    ((x) < 0 ? -(x) : (x))

void L298NHBridge::set_motor(int pin1, int pin2, int pwm, double speed) {
    if (speed < -1.0 || speed > 1.0)
        throw std::range_error("speed value out of range");

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

    if (speed != 0.0) {
        gpio::pwm::set_value(pwm, (int) ((ABS(speed) * (1.0 - min_speed) + min_speed) * 100));
    } else {
        gpio::pwm::set_value(pwm, 0);
    }
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
    gpio::write(IN1, gpio::LOW);
    gpio::write(IN2, gpio::LOW);
    gpio::write(IN3, gpio::LOW);
    gpio::write(IN4, gpio::LOW);
    gpio::pwm::set_value(ENA, 0);
    gpio::pwm::set_value(ENB, 0);
}

void L298NHBridge::setup() {
    gpio::setup();
    gpio::setup_pin(ENA, gpio::OUTPUT);
    gpio::setup_pin(IN1, gpio::OUTPUT);
    gpio::setup_pin(IN2, gpio::OUTPUT);
    gpio::setup_pin(IN3, gpio::OUTPUT);
    gpio::setup_pin(IN4, gpio::OUTPUT);
    gpio::setup_pin(ENB, gpio::OUTPUT);
    gpio::pwm::create(ENA, 0, 100);
    gpio::pwm::create(ENB, 0, 100);
}

void L298NHBridge::setMotors(double motor_a_speed, double motor_b_speed) {
    set_motor(IN1, IN2, ENA, motor_a_speed);
    set_motor(IN3, IN4, ENB, motor_b_speed);
}

void L298NHBridge::stopMotors() {
  setMotors(0.0, 0.0);
}

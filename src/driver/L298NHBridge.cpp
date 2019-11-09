#include <L298NHBridge.hpp>
#include <stdexcept>
#include <cmath>
#include <gpio.hpp>

#ifdef RASPBERRY_PI
#include <wiringPi.h>
#include <softPwm.h>
#endif

L298NHBridge::L298NHBridge(int ENA, int IN1, int IN2, int IN3, int IN4, int ENB, double min_speed) {
    this->ENA = ENA;
    this->IN1 = IN1;
    this->IN2 = IN2;
    this->IN3 = IN3;
    this->IN4 = IN4;
    this->ENB = ENB;

    if (min_speed < 0.0 || min_speed > 1.0) {
        throw std::range_error("min_speed out of scope");
    } else {
        this->min_speed = min_speed;
    }

    #ifdef RASPBERRY_PI
    wiringPiSetupGpio();
    pinMode(ENA, OUTPUT);
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);
    pinMode(ENB, OUTPUT);
    softPwmCreate(ENA, 0, 100);
    softPwmCreate(ENB, 0, 100);
    #endif
}

L298NHBridge::~L298NHBridge() {
    #ifdef RASPBERRY_PI
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
    softPwmWrite(ENA, LOW);
    softPwmWrite(ENB, LOW);
    #endif
}

void L298NHBridge::set_motor(int pin1, int pin2, int pwm, double speed) {
    if (speed < -1.0 || speed > 1.0) {
        throw std::range_error("speed value out of range");
    }

    #ifdef RASPBERRY_PI
    if (speed > 0.0) {
        digitalWrite(pin1, HIGH);
        digitalWrite(pin2, LOW);
    } else if (speed < 0.0) {
        digitalWrite(pin1, LOW);
        digitalWrite(pin2, HIGH);
    } else {
        digitalWrite(pin1, LOW);
        digitalWrite(pin2, LOW);
    }

    const int value = speed != 0.0 ? int((std::abs(speed) * (1.0 - min_speed) + min_speed) * 100.0) : 0;
    softPwmWrite(pwm, value);
    #endif
}

void L298NHBridge::set_motors(double motor_a_speed, double motor_b_speed) {
    set_motor(IN1, IN2, ENA, motor_a_speed);
    set_motor(IN3, IN4, ENB, motor_b_speed);
}

void L298NHBridge::stop_motors() {
    set_motors(0.0, 0.0);
}

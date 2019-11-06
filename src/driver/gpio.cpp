#include <gpio.hpp>

#ifdef RASPBERRY_PI
#include <wiringPi.h>
#include <softPwm.h>
#elif JETSON_NANO
#else
#include <iostream>
#endif

void gpio::setup() {
    #ifdef RASPBERRY_PI
    wiringPiSetupGpio();
    #else
    std::cout << "no GPIO hardware support" << std::endl;
    #endif
}

void gpio::setup_pin(int pin, int mode) {
    #ifdef RASPBERRY_PI
    pinMode(pin, mode == gpio::OUTPUT ? OUT : IN);
    #endif
}

void gpio::write(int pin, int value) {
    #ifdef RASPBERRY_PI
    digitalWrite(pin, value ? HIGH : LOW);
    #else
    std::cout << "pin " << pin << " : " << (value ? "HIGH" : "LOW") << std::endl;
    #endif
}

int gpio::read(int pin) {
    #ifdef RASPBERRY_PI
    return digitalRead(pin);
    #else
    return 0;
    #endif
}

int gpio::pwm::create(int pin, int init_value, int range) {
    #ifdef RASPBERRY_PI
    // hardware pwm on pin 18
    if (pin == 18) {
        pwmSetMode(PWM_MODE_MS);
        pwmSetRange(range);
        pwmSetClock(3840);
        pwmWrite(pin, 0);
        return 0;
    } else {
        return softPwmCreate(pin, init_value, range);
    }
    #else
    return 0;
    #endif
}

void gpio::pwm::set_value(int pin, int value) {
    #ifdef RASPBERRY_PI
    if (pin == 18) {
        pwmWrite(pin, value);
    } else {
        softPwmWrite(pin, value);
    }
    #endif
}



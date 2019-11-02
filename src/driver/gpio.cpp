#include <gpio.hpp>

#ifdef RASPBERRY_PI
#include <wiringPi.h>
#elif JETSON_NANO
#else
#endif

void gpio::setup() {
    #ifdef RASPBERRY_PI
    wiringPiSetupGpio();
    #endif
}

void gpio::setup_pin(int pin, int mode) {
    #ifdef RASPBERRY_PI
    pinMode(pin, mode);
    #endif
}

void gpio::write(int pin, int value) {
    #ifdef RASPBERRY_PI
    digitalWrite(pin, value);
    #endif
}

int gpio::read(int pin) {
    #ifdef RASPBERRY_PI
    return digitalRead(pin);
    #else
    return LOW;
    #endif
}

int gpio::pwm::create(int pin, int init_value, int range) {
    #ifdef RASPBERRY_PI
    // hardware pwm on pin 18
    if (pi == 18) {
        pwmSetMode(PWM_MODE_MS);
        pwmSetRange(range);
        pwmSetClock(3840);
        pwmWrite(pin, 0);
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



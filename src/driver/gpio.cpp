#include <gpio.hpp>

#ifdef RASPBERRY_PI
#include <wiringPi.h>
#include <softPwm.h>
#endif

#ifndef INPUT
#define INPUT_VAL   0
#else
#define INPUT_VAL   INPUT
#undef INPUT
#endif

#ifndef OUTPUT
#define OUTPUT_VAL  1
#else
#define OUTPUT_VAL  OUTPUT
#undef OUTPUT
#endif

#ifndef PWM_OUTPUT
#define PWM_VAL     2
#else
#define PWM_VAL     PWM_OUTPUT
#endif

#ifndef CLOCK
#define CLOCK_VAL   3
#else
#define CLOCK_VAL   GPIO_CLOCK
#endif

#ifndef HIGH
#define HIGH_VAL    1
#else
#define HIGH_VAL    HIGH
#undef HIGH
#endif

#ifndef LOW
#define LOW_VAL     0
#else
#define LOW_VAL     LOW
#undef LOW
#endif

const int gpio::INPUT = INPUT_VAL;
const int gpio::OUTPUT = OUTPUT_VAL;
const int gpio::PWM = PWM_VAL;
const int gpio::CLOCK = CLOCK_VAL;
const int gpio::HIGH = HIGH_VAL;
const int gpio::LOW = LOW_VAL;

void gpio::init() {
    static bool already_set_up = false;
    if (!already_set_up) {
        #ifdef RASPBERRY_PI
        if (wiringPiSetupGpio() < 0) {
            throw std::runtime_error("cannot setup wiringPi");
        }
        #endif
    }
}

void gpio::setup(int pin, int mode) {
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
    return 0;
    #endif
}

void gpio::pwm::create(int pin, int init, int range, int clock_rate) {
    #ifdef RASPBERRY_PI
    if (pin == 18) {
        setup(pin, PWM);
        pwmSetMode(PWM_MODE_MS);
        pwmSetClock(19200000 / clock_rate);
        pwmSetRange(range);
        pwmWrite(pin, init);
    } else {
        softPwmCreate(pin, init, range);
    }
    #endif
}

void gpio::pwm::write(int pin, int value) {
    #ifdef RASPBERRY_PI
    if (pin == 18) {
        pwmWrite(pin, value);
    } else {
        softPwmWrite(pin, value);
    }
    #endif
}

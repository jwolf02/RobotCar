#ifndef __GPIO_HPP
#define __GPIO_HPP

#include <functional>

#if defined(WIRING_PI)
#include <wiringPi.h>
#include <softPwm.h>
#elif defined(SYS_GPIO)
#include <sys_gpio.hpp>
#endif

#ifndef INPUT
constexpr int __INPUT_VAL = 0;
#else
constexpr int __INPUT_VAL = INPUT;
#undef INPUT
#endif

#ifndef OUTPUT
constexpr int __OUTPUT_VAL = 1;
#else
constexpr int __OUTPUT_VAL = OUTPUT;
#undef OUTPUT
#endif

#ifndef PWM_OUTPUT
constexpr int __PWM_VAL = 2;
#else
constexpr int __PWM_VAL = PWM_OUTPUT;
#endif

#ifndef CLOCK
constexpr int __CLOCK_VAL = 3;
#else
constexpr int __CLOCK_VAL = GPIO_CLOCK;
#endif

#ifndef LOW
constexpr int __LOW_VAL = 0;
#else
constexpr int __LOW_VAL = LOW;
#undef LOW
#endif

#ifndef HIGH
constexpr int __HIGH_VAL = 1;
#else
constexpr int __HIGH_VAL = HIGH;
#undef HIGH
#endif

namespace gpio {

    // pin mode constants
    constexpr int INPUT = __INPUT_VAL;
    constexpr int OUTPUT = __OUTPUT_VAL;
    constexpr int PWM = __PWM_VAL;
    constexpr int CLOCK = __CLOCK_VAL;

    // pin value constants
    constexpr int HIGH = __HIGH_VAL;
    constexpr int LOW = __LOW_VAL;

    // check if build uses some GPIO library
    bool hardware_support();

    // setup GPIO library
    void init();

    inline void setup(int pin, int mode) {
        #if defined(WIRING_PI) || defined(SYS_GPIO)
        pinMode(pin, mode);
        #endif
    }

    inline void write(int pin, int value) {
        #if defined(WIRING_PI) || defined(SYS_GPIO)
        digitalWrite(pin, value);
        #endif
    }

    inline int read(int pin) {
        #if defined(WIRING_PI) || defined(SYS_GPIO)
        return digitalRead(pin);
        #else
        return -1;
        #endif
    }

    // pwm wrapper
    // can handle Raspberry Pi hardware PWM on pin 18
    // and also wiringPi's softpwm
    namespace pwm {

        inline void create(int pin, int init, int range, int clock_rate=5000) {
            #ifdef WIRING_PI
            if (pin == 18) {
                setup(pin, PWM);
                pwmSetMode(PWM_MODE_MS);
                pwmSetClock(19200000 / clock_rate);
                pwmSetRange(range);
                pwmWrite(pin, init);
            } else {
                setup(pin, OUTPUT);
                softPwmCreate(pin, init, range);
            }
            #endif
        }

        inline void write(int pin, int value) {
            #ifdef WIRING_PI
            if (pin == 18) {
                pwmWrite(pin, value);
            } else {
                softPwmWrite(pin, value);
            }
            #endif
        }
    }
}

#endif // __GPIO_HPP

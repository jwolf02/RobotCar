#ifndef __GPIO_HPP
#define __GPIO_HPP

namespace gpio {

    // pin mode constants
    extern const int INPUT;
    extern const int OUTPUT;
    extern const int PWM;
    extern const int CLOCK;

    // pin value constants
    extern const int HIGH;
    extern const int LOW;

    // setup GPIO library
    void init();

    void setup(int pin, int mode);

    void write(int pin, int value);

    int read(int pin);

    // pwm wrapper
    // can handle Raspberry Pi hardware PWM on pin 18
    // and also wiringPi's softpwm
    namespace pwm {

        void create(int pin, int init, int range, int clock_rate=5000);

        void write(int pin, int value);

    }

}

#endif // __GPIO_HPP

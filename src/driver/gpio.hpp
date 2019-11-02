#ifndef __GPIO_HPP
#define __GPIO_HPP

namespace gpio {

    constexpr int OUTPUT = 0;
    constexpr int INPUT = 1;

    constexpr int LOW = 0;
    constexpr int HIGH = 1;

    void setup();

    void setup_pin(int pin, int mode);

    void write(int pin, int value);

    int read(int pin);

    namespace pwm {

        int create(int pin, int init_value, int range);

        void set_value(int pin, int value);

    }

}

#endif // __GPIO_HPP
#ifndef __GPIO_HPP
#define __GPIO_HPP

namespace gpio {

    extern const int OUTPUT;
    extern const int INPUT;
    extern const int LOW;
    extern const int HIGH;

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
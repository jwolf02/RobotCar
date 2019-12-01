#ifndef __SYS_GPIO_HPP
#define __SYS_GPIO_HPP

#include <functional>

#define INPUT       0
#define OUTPUT      1

#define LOW         0
#define HIGH        1

int setupGpio();

int pinMode(int gpio, int mode);

int digitalWrite(int pin, int value);

int digitalRead(int pin);

int setIsr(int pin, int edge_type, const std::function<void (int pin)> &callback);

#endif // _SYS_GPIO_HPP

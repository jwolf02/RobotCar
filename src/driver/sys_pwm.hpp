#ifndef __SYS_PWM_HPP
#define __SYS_PWM_HPP

int pwmCreate(int pin, int frequency=100);

int pwmDestroy(int pin);

int pwmChangeDutyCycle(int pin, int duty_cycle);

#endif // __SYS_PWM_HPP

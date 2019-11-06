#ifndef __L298NHBridge_HPP
#define __L298NHBridge_HPP

// Wrapper for the L298N Dual H-Bridge
// motor A in assumed to be connected to (ENA, IN1, IN2)
// motor B is assumed to be connected to (ENB, IN4, IN3)
class L298NHBridge {
public:

    L298NHBridge() = default;

    L298NHBridge(int ENA, int IN1, int IN2, int IN3, int IN4, int ENB, double min_speed=0.3);

    ~L298NHBridge();

    void set_motors(double motor_a_speed, double motor_b_speed);

    void stop_motors();

private:

    void set_motor(int pin1, int pin2, int pwm, double speed);

    int ENA = 0;

    int IN1 = 0;

    int IN2 = 0;

    int IN3 = 0;

    int IN4 = 0;

    int ENB = 0;

    double min_speed = 0.0;

};

#endif // __L298NHBridge_HPP

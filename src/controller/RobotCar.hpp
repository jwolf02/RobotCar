#ifndef __ROBOTCAR_HPP
#define __ROBOTCAR_HPP

#include <L298NHBridge.hpp>
#include <string>

namespace RobotCar {

    // encodes if the car is currently rotating or driving
    enum Mode {
        Drive, Rotate
    };

    void setup(int ENA, int IN1, int IN2, int IN3, int IN4, int ENB);

    void cleanup();

    void drive(double motor_a_speed, double motor_b_speed);

    void drive(double speed);

    void rotate(double speed);

    void stop();

    double get_motor_a_speed();

    double get_motor_b_speed();

    Mode get_mode();
}

#endif // __ROBOTCAR_HPP

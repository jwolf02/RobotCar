#ifndef __ROBOTCAR_HPP
#define __ROBOTCAR_HPP

#include <L298NHBridge.hpp>
#include <string>

class RobotCar {
public:

    enum Mode {
        Drive, Rotate
    };

    RobotCar() = default;

    ~RobotCar();

    void rotate(double speed);

    void drive(double speed);

    void drive(double motor_a_speed, double motor_b_speed);

    void stop();

    double speed() const;

    Mode mode() const;

private:

    L298NHBridge _bridge;

    double _speed = 0;

    Mode _mode = Drive;

};

#endif // __ROBOTCAR_HPP

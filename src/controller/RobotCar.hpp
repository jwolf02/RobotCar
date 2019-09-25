#ifndef __ROBOTCAR_HPP
#define __ROBOTCAR_HPP

#include <string>

namespace RobotCar {

    // encodes if the car is currently rotating or driving
    enum Mode {
        Drive, Rotate
    };

    void setup(int ENA, int IN1, int IN2, int IN3, int IN4, int ENB, uint8_t address=0x68);

    void calibrate(int num_measurements=1000);

    void roll_pitch_yaw(double &roll, double &pitch, double &yaw);

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

#include <RobotCar.hpp>
#include <cstdlib>
#include <cmath>
#include <L298NHBridge.hpp>
#include <MPU9250.hpp>
#include <stdexcept>

#define X           0
#define Y           1
#define Z           2

#define SATURATE_RANGE(lower, upper, x)     if (x < lower) x = lower; else if (x > upper) x = upper

using namespace RobotCar;

static void _drive(double a, double b);

// H-Bridge
L298NHBridge bridge;

// IMU
MPU9250 imu;

// { motor_a_speed, motor_b_speed }
double motor_speed[2] = { 0, 0 };

void RobotCar::setup(int ENA, int IN1, int IN2, int IN3, int IN4, int ENB) {
    // setup H-Bridge
    bridge = L298NHBridge(ENA, IN1, IN2, IN3, IN4, ENB);

    // cleanup module after exit
    atexit(RobotCar::cleanup);
}

void RobotCar::cleanup() {
    bridge.cleanup();
}

void RobotCar::rotate(double speed) {
    _drive(speed, -speed);
}

void RobotCar::drive(double speed) {
    _drive(speed, speed);
}

void _drive(double motor_a_speed, double motor_b_speed) {
    // set motor_a_speed to range [-1, 1]
    SATURATE_RANGE(-1.0, 1.0, motor_a_speed);

    // set motor_b_speed to range [-1, 1]
    SATURATE_RANGE(-1.0, 1.0, motor_b_speed);

    bridge.setMotors(motor_a_speed, motor_b_speed);

    motor_speed[0] = motor_a_speed;
    motor_speed[1] = motor_b_speed;
}

void RobotCar::stop() {
    bridge.stopMotors();
    motor_speed[0] = 0;
    motor_speed[1] = 0;
}

double RobotCar::get_motor_a_speed() {
    return motor_speed[0];
}

double RobotCar::get_motor_b_speed() {
    return motor_speed[1];
}

RobotCar::Mode RobotCar::get_mode() {
    if (std::abs(motor_speed[0] + motor_speed[1]) < 0.01) {
        return Rotate;
    } else {
        return Drive;
    }
}

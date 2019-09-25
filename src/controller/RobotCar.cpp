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

// H-Bridge
L298NHBridge bridge;

MPU9250 imu;

double accel[3];
double gyro[3];
double magn[3];

double a_mean[3];
double g_mean[3];
double m_mean[3];

double roll, pitch, yaw;

// { motor_a_speed, motor_b_speed }
double motor_speed[2] = { 0, 0 };

static void get_imu_data() {
    int16_t ax, ay, az, gx, gy, gz, mx, my, mz;
    imu.getMotion9(&ax, &ay, &az, &gx, &gy, &gz, &mx, &my, &mz);

    accel[X] = double(ax) / 16384.0 - a_mean[0];
    accel[Y] = double(ay) / 16384.0 - a_mean[1];
    accel[Z] = double(az) / 16384.0 - a_mean[2];

    gyro[X] = double(gx) * 250.0 / 32768 - g_mean[0];
    gyro[Y] = double(gy) * 250.0 / 32768 - g_mean[1];
    gyro[Z] = double(gz) * 250.0 / 32768 - g_mean[2];

    magn[X] = double(mx) * 1200.0 / 4096.0 - m_mean[0];
    magn[Y] = double(mx) * 1200.0 / 4096.0 - m_mean[1];
    magn[Z] = double(mx) * 1200.0 / 4096.0 - m_mean[2];

    pitch = 180.0 * atan2(accel[0], sqrt(accel[1] * accel[1] + accel[1] * accel[2])) / M_PI;
    roll = 180.0 * atan2(accel[1], sqrt(accel[0] * accel[0] + accel[2] * accel[2])) / M_PI;

    auto mag_x = magn[X] * cos(pitch) + magn[Y] * sin(roll) * sin(pitch) + magn[Z] * cos(roll) * sin(pitch);
    auto mag_y = magn[Y] * cos(roll) - magn[Z] * sin(roll);
    yaw = 180.0 * atan2(-mag_y, mag_x) / M_PI;
}

void RobotCar::setup(int ENA, int IN1, int IN2, int IN3, int IN4, int ENB, uint8_t address) {
    // setup H-Bridge
    bridge = L298NHBridge(ENA, IN1, IN2, IN3, IN4, ENB);

    // setup MPU
    imu = MPU9250(address);
    imu.initialize();
    if (!imu.testConnection()) {
        throw std::runtime_error("connection to IMU failed");
    }

    // cleanup module after exit
    atexit(RobotCar::cleanup);
}

void RobotCar::roll_pitch_yaw(double &vroll, double &vpitch, double &vyaw) {
    get_imu_data();
    vroll = roll;
    vpitch = pitch;
    vyaw = yaw;
}

void RobotCar::cleanup() {
    bridge.cleanup();
}

void RobotCar::rotate(double speed) {
    drive(speed, -speed);
}

void RobotCar::drive(double speed) {
    drive(speed, speed);
}

void RobotCar::drive(double motor_a_speed, double motor_b_speed) {
    // set motor_a_speed to range [-1, 1]
    SATURATE_RANGE(-1.0, 1.0, motor_a_speed);

    // set motor_b_speed to range [-1, 1]
    SATURATE_RANGE(-1.0, 1.0, motor_b_speed);

    bridge.setMotors(motor_a_speed, motor_b_speed);

    motor_speed[0] = motor_a_speed;
    motor_speed[1] = motor_b_speed;
}

void RobotCar::calibrate(int num_measurements) {
    int16_t ax, ay, az, gx, gy, gz, mx, my, mz;
    double m_min[3] = { INFINITY, INFINITY, INFINITY };
    double m_max[3] = { -INFINITY, -INFINITY, -INFINITY };
    double m_sample[3];

    for (int i = 0; i < num_measurements; ++i) {
        // obtain data from sensor
        imu.getMotion9(&ax, &ay, &az, &gx, &gy, &gz, &mx, &my, &mz);

        // process accelerometer data
        a_mean[0] = (double(ax) / 16384.0) / double(num_measurements);
        a_mean[1] = (double(ay) / 16384.0) / double(num_measurements);
        a_mean[2] = (double(az) / 16384.0) / double(num_measurements);

        // process gyroscope data
        g_mean[0] = (double(gx) * 250.0 / 32768) / double(num_measurements);
        g_mean[1] = (double(gy) * 250.0 / 32768) / double(num_measurements);
        g_mean[2] = (double(gz) * 250.0 / 32768) / double(num_measurements);

        // process magnetometer data
        m_sample[0] = double(mx) * 1200.0 / 4096.0;
        m_sample[1] = double(mx) * 1200.0 / 4096.0;
        m_sample[2] = double(mx) * 1200.0 / 4096.0;

        for (int j = 0; j < 3; ++j)
            m_min[j] = std::min(m_min[j], m_sample[j]);

        for (int j = 0; j < 3; ++j)
            m_max[j] = std::max(m_max[j], m_sample[j]);

    }

    for (int j = 0; j < 3; ++j)
        m_mean[j] = (m_max[j] - m_min[j]) / 2;
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

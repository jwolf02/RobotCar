#include <MPU9250.hpp>

MPU9250::MPU9250(const std::string &settings) {
    imu = RTIMU::createIMU(new RTIMUSettings(settings));
    if ((imu == nullptr) or (imu->IMUType() == RTIMU_NULL_TYPE)) {
        throw std::runtime_error("no IMU found");
    }

    imu->IMUInit();

    imu->setSlerpPower(0.02);
    imu->setGyroEnable(true);
    imu->setAccelEnable(true);
    imu->setCompassEnable(true);
}

MPU9250::~MPU9250() {
    delete imu;
}

bool MPU9250::read() {
    return imu->IMURead();
}

Double3D MPU9250::orientation(bool in_degrees) {
    const double factor = in_degrees ? 180.0 / M_PI : 1.0;
    Double3D orient;
    auto data = imu->getIMUData();
    orient[0] = data.fusionPose.x() * factor;
    orient[1] = data.fusionPose.y() * factor;
    orient[2] = data.fusionPose.z() * factor;
    return orient;
}

Double3D MPU9250::acceleration() {
    Double3D accel;
    auto data = imu->getIMUData();
    accel[0] = data.accel.x();
    accel[1] = data.accel.y();
    accel[2] = data.accel.z();
    return accel;
}

Double3D MPU9250::gyroscope() {
    Double3D gyro;
    auto data = imu->getIMUData();
    gyro[0] = data.gyro.x();
    gyro[1] = data.gyro.y();
    gyro[2] = data.gyro.z();
    return gyro;
}

Double3D MPU9250::compass() {
    Double3D comp;
    auto data = imu->getIMUData();
    comp[0] = data.compass.x();
    comp[1] = data.compass.y();
    comp[2] = data.compass.z();
    return comp;
}

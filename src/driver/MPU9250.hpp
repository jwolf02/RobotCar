#ifndef __MPU9250_HPP
#define __MPU9250_HPP

#include <string>
#include <RTIMULib.h>
#include <VecND.hpp>

class MPU9250 {
public:

    MPU9250() = default;

    MPU9250(const std::string &settings);

    ~MPU9250();

    uint64_t pollInterval();

    bool read();

    Double3D rotation(bool in_degrees=true);

    Double3D acceleration();

    Double3D gyroscope();

    Double3D compass();

private:

    RTIMU *imu = nullptr;

};

#endif // __MPU9250_HPP
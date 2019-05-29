#include <Socket.hpp>
#include <unistd.h>
#include <stdexcept>
#include <errno.h>
#include <cstring>

static inline int close_call(int fd) {
    return close(fd);
}

Socket::~Socket() {}

size_t Socket::send(const void *buf, size_t n) {
    // take start point if measurement is on
    clock_t begin = 0, end = 0;
    if (speed_measurement) {
        begin = clock();
    }

    ssize_t b = 0;
    if ((b = write(sockfd, buf, n)) < (ssize_t) n) {
        throw std::runtime_error(std::string("send:") + strerror(errno));
    }

    if (speed_measurement) {
        end = clock();
        last_measured_speed = (double(end - begin) / CLOCKS_PER_SEC) * double(n);
    }

    return (size_t) b;
}

size_t Socket::recv(void *buf, size_t n) {
    // take start point if measurement is on
    clock_t begin = 0, end = 0;
    if (speed_measurement) {
        begin = clock();
    }

    ssize_t b = 0;
    if ((b = read(sockfd, buf, n)) < 0) {
        throw std::runtime_error(std::string("recv:") + strerror(errno));
    }

    if (speed_measurement) {
        end = clock();
        last_measured_speed = (double(end - begin) / CLOCKS_PER_SEC) * double(n);
    }

    return (size_t) b;
}

void Socket::close() {
    if (close_call(sockfd) < 0) {
        throw std::runtime_error(std::string("close:") + strerror(errno));
    }
}

void Socket::setSpeedMeasurement(bool tracking) {
    speed_measurement = tracking;
}

bool Socket::speedMeasurement() const {
    return speed_measurement;
}

double Socket::lastMeasuredSpeed() const {
    return last_measured_speed;
}

int Socket::port() const {
    return portno;
}
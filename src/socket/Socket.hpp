#ifndef __SOCKET_HPP
#define __SOCKET_HPP

#include <cstddef>

class Socket {
public:

    // default constructor
    Socket() = default;

    // virtual default constructor that closes the socket
    virtual ~Socket() = 0;

    // send n bytes from the buffer
    size_t send(const void *buf, size_t n);

    // read up to n bytes to the buffer
    size_t recv(void *buf, size_t n);

    // close socket
    virtual void close();

    // set if connection speed should be measured
    void setSpeedMeasurement(bool tracking);

    // get if speed measurement is turned on
    bool speedMeasurement() const;

    // get the measured speed
    double lastMeasuredSpeed() const;

    int port() const;

protected:

    // socket file descriptor
    int sockfd = 0;

    bool speed_measurement = false;

    double last_measured_speed = 0.0;

    int portno = -1;

};

#endif // __SOCKET_HPP

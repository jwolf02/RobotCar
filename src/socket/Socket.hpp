#ifndef __SOCKET_HPP
#define __SOCKET_HPP

// This is the base class for both the socket on the server
// as well as on the client side one.
// It cannot itself establish a connection as this is highly
// dependant on the type of connection but it can send and receive
// data and close the connection

#include <cstddef>
#include <unordered_map>

class Socket {
public:
    // the type of connection that is created
    enum ConnectionType {
        Inet, Bluetooth
    };

    // default constructor
    Socket() = default;

    // virtual default constructor that closes the socket
    virtual ~Socket() = 0;

    // send n bytes from the buffer
    size_t send(const void *buf, size_t n);

    // send a string, the receiver has to call the appropriate function
    void send(const std::string &msg);

    // read up to n bytes to the buffer
    size_t recv(void *buf, size_t n);

    // receive a message send with send(const std::string&)
    void recv(std::string &msg);

    // see above
    std::string recv();

    // close socket
    virtual void close() = 0;

    int port() const;

    int channel() const;

protected:

    ConnectionType conn_type = Inet;

    // socket file descriptor
    int sockfd = 0;

    int portno = -1;

    int channelno = -1;

};

namespace BluetoothSocket {

    std::unordered_map<std::string, std::string> scan_for_devices();

}

#endif // __SOCKET_HPP

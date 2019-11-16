#ifndef __SOCKET_HPP
#define __SOCKET_HPP

#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class Socket {
public:

    Socket() = default;

    virtual ~Socket() {
        close();
    }

    virtual void write(const void *buf, size_t n) = 0;

    virtual void read(void *buf, size_t n) = 0;

    virtual void close() = 0;

};

#endif // __SOCKET_HPP

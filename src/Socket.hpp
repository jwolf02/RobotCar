#ifndef __SOCKET_HPP
#define __SOCKET_HPP

#include <cstddef>

class Socket {
public:

    Socket() = default;

    virtual ~Socket() = 0;

    size_t send(const void *buf, size_t n);

    size_t recv(void *buf, size_t n);

    void close();

protected:

    int sockfd = 0;

};

#endif // __SOCKET_HPP

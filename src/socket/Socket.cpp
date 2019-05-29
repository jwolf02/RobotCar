#include <net/Socket.hpp>
#include <unistd.h>
#include <stdexcept>

static inline int close_call(int fd) {
    return close(fd);
}

Socket::~Socket() {}

size_t Socket::send(const void *buf, size_t n) {
    ssize_t b = 0;
    if ((b = write(sockfd, buf, n)) < (ssize_t) n) {
        throw std::runtime_error("IO-error (send)");
    }
    return (size_t) b;
}

size_t Socket::recv(void *buf, size_t n) {
    ssize_t b = 0;
    if ((b = read(sockfd, buf, n)) < (ssize_t) n) {
        throw std::runtime_error("IO-Error (recv)");
    }
    return (size_t) b;
}

void Socket::close() {
    if (close_call(sockfd) < 0) {
        // throw std::runtime_error("closing socket failed");
    }
}
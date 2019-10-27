#include <Socket.hpp>
#include <unistd.h>
#include <stdexcept>
#include <cerrno>
#include <cstring>
#include <socket_wrappers.hpp>
#include <cstdint>
#include <netinet/in.h>

Socket::~Socket() {}

size_t Socket::send(const void *buf, size_t n) {
    ssize_t b = 0;
    if ((b = write(sockfd, buf, n)) < (ssize_t) n) {
        throw std::runtime_error(std::string("send() ") + strerror(errno));
    }

    return (size_t) b;
}

void Socket::send(const std::string &msg) {
    auto msg_size = (uint32_t) htonl((uint32_t) msg.size());
    send(&msg_size, sizeof(uint32_t));
    send(msg.data(), msg.size());
}

size_t Socket::recv(void *buf, size_t n) {
    ssize_t b = 0;
    if ((b = read(sockfd, buf, n)) < 0) {
        throw std::runtime_error(std::string("recv() ") + strerror(errno));
    }

    return (size_t) b;
}

void Socket::recv(std::string &msg) {
    uint32_t msg_size;
    recv(&msg_size, sizeof(uint32_t));
    msg_size = (uint32_t) ntohl((uint32_t) msg_size);
    msg.reserve(msg_size);
    recv((void *) msg.data(), msg_size);
}

std::string Socket::recv() {
    std::string str;
    recv(str);
    return str;
}

int Socket::port() const {
    return portno;
}
#include <ClientSocket.hpp>
#include <stdexcept>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstdio>

static inline int connect_call(int fd, struct sockaddr *addr, size_t size) {
    return connect(fd, addr, size);
}

ClientSocket::~ClientSocket() {
    close();
}

void ClientSocket::connect(const std::string &addr, int port) {
    if (inet_pton(AF_INET, addr.c_str(), &serv_addr.sin_addr) < 0) {
        throw std::runtime_error("invalid address");
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    if (connect_call(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        throw std::runtime_error("connection failed");
    }
}
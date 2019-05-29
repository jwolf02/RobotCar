#include <ClientSocket.hpp>
#include <stdexcept>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstdio>
#include <cstring>
#include <errno.h>

static inline int connect_call(int fd, struct sockaddr *addr, size_t size) {
    return connect(fd, addr, size);
}

ClientSocket::~ClientSocket() {
    close();
}

void ClientSocket::connect(const std::string &addr, int port) {

    portno = port;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        throw std::runtime_error(std::string("socket:") + strerror(errno));
    }

    if (inet_pton(AF_INET, addr.c_str(), &serv_addr.sin_addr) < 0) {
        throw std::runtime_error(std::string("inet_pton") + strerror(errno));
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portno);

    if (connect_call(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        throw std::runtime_error(std::string("connect:") + strerror(errno));
    }
}
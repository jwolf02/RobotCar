#include <ServerSocket.hpp>

ServerSocket::ServerSocket() {
    memset(&address, 0x00, sizeof(address));
}

ServerSocket::ServerSocket(int port) : ServerSocket() {

    portno = port;

    if ((connfd = socket(AF_INET, SOCK_STREAM, 0)) <= 0) {
        throw std::runtime_error(std::string("socket:") + strerror(errno));
    }

    int opt = 1;
    if (setsockopt(connfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) < 0) {
        throw std::runtime_error(std::string("setsockopt:") + strerror(errno));
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(portno);

    if (bind(connfd, (struct sockaddr*) &address, sizeof(address)) < 0) {
        throw std::runtime_error(std::string("bind:") + strerror(errno));
    }

    if (listen(connfd, 1) < 0) {
        throw std::runtime_error(std::string("listen:") + strerror(errno));
    }
}

ServerSocket::~ServerSocket() {
    close();
}

void ServerSocket::waitForConnection() {
    constexpr int addrlen = sizeof(address);
    if ((sockfd = accept(connfd, (struct sockaddr *) &address, (socklen_t *) &addrlen)) < 0) {
        throw std::runtime_error(std::string("accept:") + strerror(errno));
    }
}
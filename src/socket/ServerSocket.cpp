#include <ServerSocket.hpp>
#include <socket_wrappers.hpp>

ServerSocket::ServerSocket(int port) {
    portno = port;

    if ((connfd = socket(AF_INET, SOCK_STREAM, 0)) <= 0) {
        throw std::runtime_error(std::string("socket() ") + strerror(errno));
    }

    int opt = 1;
    if (setsockopt(connfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) < 0) {
        throw std::runtime_error(std::string("setsockopt() ") + strerror(errno));
    }

    inet_address.sin_family = AF_INET;
    inet_address.sin_addr.s_addr = htonl(INADDR_ANY);
    inet_address.sin_port = htons(portno);

    if (bind(connfd, (struct sockaddr*) &inet_address, sizeof(inet_address)) < 0) {
        throw std::runtime_error(std::string("bind() ") + strerror(errno));
    }

    if (listen(connfd, 1) < 0) {
        throw std::runtime_error(std::string("listen() ") + strerror(errno));
    }
}

ServerSocket::~ServerSocket() {
    close();
}

void ServerSocket::waitForConnection() {
    int addrlen = sizeof(inet_address);
    struct sockaddr *addr = (struct sockaddr *) &inet_address;

    if ((sockfd = accept(connfd, addr, (socklen_t *) &addrlen)) < 0) {
        throw std::runtime_error(std::string("accept() ") + strerror(errno));
    }
}

void ServerSocket::close() {
    close_wrapper(sockfd);
}

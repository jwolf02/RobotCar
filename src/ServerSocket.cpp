#include <ServerSocket.hpp>

ServerSocket::ServerSocket() {
    memset(&serv_addr, 0x00, sizeof(serv_addr));
}

ServerSocket::ServerSocket(int port) : ServerSocket() {
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        throw std::runtime_error("creating socket failed");
    }

    int opt = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) < 0) {
        throw std::runtime_error("setsockopt failed");
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(port);

    if (bind(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0) {
        throw std::runtime_error("bind failed");
    }

    if (listen(sockfd, 1) < 0) {
        throw std::runtime_error("listen failed");
    }
}

ServerSocket::~ServerSocket() {
    close();
}

void ServerSocket::waitForConnection() {
    if ((connfd = accept(sockfd, (struct sockaddr*) NULL, NULL)) < 0) {
        throw std::runtime_error("accept failed");
    }
}
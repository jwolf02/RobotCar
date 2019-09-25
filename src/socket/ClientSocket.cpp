#include <ClientSocket.hpp>
#include <stdexcept>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <cerrno>
#include <socket_wrappers.hpp>
#include <bluetooth/bluetooth.h>

ClientSocket::ClientSocket(Socket::ConnectionType type) {
    conn_type = type;
}

ClientSocket::~ClientSocket() {
    close();
}

void ClientSocket::connect(const std::string &addr, int port_or_channel) {
    if (conn_type == Socket::Inet) {
        portno = port_or_channel;

        if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            throw std::runtime_error(std::string("socket() ") + strerror(errno));
        }

        if (inet_pton(AF_INET, addr.c_str(), &inet_address.sin_addr) < 0) {
            throw std::runtime_error(std::string("inet_pton() ") + strerror(errno));
        }

        inet_address.sin_family = AF_INET;
        inet_address.sin_port = htons(portno);

        if (connect_wrapper(sockfd, (struct sockaddr *) &inet_address, sizeof(inet_address)) < 0) {
            throw std::runtime_error(std::string("connect() ") + strerror(errno));
        }
    } else {
        channelno = port_or_channel;

        if ((sockfd = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM)) < 0) {
            throw std::runtime_error(std::string("socket() ") + strerror(errno));
        }

        // set the connection parameters (who to connect to)
        bt_address.rc_family = AF_BLUETOOTH;
        bt_address.rc_channel = (uint8_t) channelno;
        str2ba(addr.c_str(), &bt_address.rc_bdaddr);

        if (connect_wrapper(sockfd, (struct sockaddr *) &bt_address, sizeof(bt_address)) < 0) {
            throw std::runtime_error(std::string("connect() ") + strerror(errno));
        }
    }
}

#include <Socket.hpp>
#include <unistd.h>
#include <stdexcept>
#include <cerrno>
#include <cstring>
#include <socket_wrappers.hpp>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

Socket::~Socket() {}

size_t Socket::send(const void *buf, size_t n) {
    ssize_t b = 0;
    if ((b = write(sockfd, buf, n)) < (ssize_t) n) {
        throw std::runtime_error(std::string("send:") + strerror(errno));
    }

    return (size_t) b;
}

size_t Socket::recv(void *buf, size_t n) {
    ssize_t b = 0;
    if ((b = read(sockfd, buf, n)) < 0) {
        throw std::runtime_error(std::string("recv:") + strerror(errno));
    }

    return (size_t) b;
}

void Socket::close() {
    if (close_wrapper(sockfd) < 0) {
        throw std::runtime_error(std::string("close:") + strerror(errno));
    }
}

int Socket::port() const {
    return portno;
}

int Socket::channel() const {
    return channelno;
}

std::unordered_map<std::string, std::string> BluetoothSocket::scan_for_devices() {
    int max_rsp, num_rsp;
    int dev_id, sock, len, flags;
    int i;
    char addr[19] = { 0 };
    char name[248] = { 0 };

    dev_id = hci_get_route(nullptr);
    sock = hci_open_dev( dev_id );
    if (dev_id < 0 || sock < 0) {
        throw std::runtime_error(std::string("hci_open_dev: ") + strerror(errno));
    }

    len  = 8;
    max_rsp = 255;
    flags = IREQ_CACHE_FLUSH;
    auto *ii = (inquiry_info *) malloc(max_rsp * sizeof(inquiry_info));

    num_rsp = hci_inquiry(dev_id, len, max_rsp, nullptr, &ii, flags);
    if( num_rsp < 0 ) {
        throw std::runtime_error(std::string("hci_inquiry: ") + strerror(errno));
    }

    std::unordered_map<std::string, std::string> dev(num_rsp);
    for (i = 0; i < num_rsp; i++) {
        ba2str(&(ii + i)->bdaddr, addr);
        memset(name, 0, sizeof(name));
        if (hci_read_remote_name(sock, &(ii + i)->bdaddr, sizeof(name),
                                 name, 0) < 0)
            strcpy(name, "[unknown]");
        dev.insert(std::pair<std::string, std::string>(addr, name));
    }

    free(ii);
    close_wrapper(sock);

    return dev;
}
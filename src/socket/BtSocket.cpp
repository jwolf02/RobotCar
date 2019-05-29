#include <BtSocket.hpp>
#include <stdexcept>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <cstring>
#include <cerrno>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <socket_wrappers.hpp>

std::unordered_map<std::string, std::string> BtSocket::scan_for_devices() {
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
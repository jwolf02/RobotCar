#include <socket_wrappers.hpp>
#include <unistd.h>
#include <sys/socket.h>

int close_wrapper(int fd) {
    return close(fd);
}

ssize_t recv_wrapper(int fd, void *buf, size_t n) {
    return 0;
}

ssize_t send_wrapper(int fd, const void *buf, size_t n) {
    return 0;
}

int connect_wrapper(int fd, struct sockaddr *addr, size_t size) {
    return connect(fd, addr, size);
}

#ifndef __SOCKET_WRAPPERS_HPP
#define __SOCKET_WRAPPERS_HPP

#include <stdio.h>

int close_wrapper(int fd);

ssize_t recv_wrapper(int fd, void *buf, size_t n);

ssize_t send_wrapper(int fd, const void *buf, size_t n);

int connect_wrapper(int fd, struct sockaddr *addr, size_t size);

#endif // __SOCKET_WRAPPERS_HPP

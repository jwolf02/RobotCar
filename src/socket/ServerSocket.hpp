#ifndef __SERVERSOCKET_HPP
#define __SERVERSOCKET_HPP

#include <Socket.hpp>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <sys/types.h>
#include <ctime>
#include <stdexcept>

class ServerSocket : public Socket {
public:

	ServerSocket() = default;

	ServerSocket(int port);

	~ServerSocket() override;

	void waitForConnection();

	void close() override;

private:

	int connfd = 0;

	struct sockaddr_in inet_address = { 0 };

};

#endif // __SERVERSOCKET_HPP



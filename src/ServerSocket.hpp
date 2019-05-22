#ifndef __SERVERSOCKET_HPP
#define __SERVERSOCKET_HPP

#include <Socket.hpp>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <time.h> 
#include <stdexcept>

class ServerSocket : public Socket {
public:

	ServerSocket();

	ServerSocket(int port);

	virtual ~ServerSocket();

	void waitForConnection();

private:

	int connfd = 0;

	struct sockaddr_in serv_addr;

};

#endif // __SERVERSOCKET_HPP



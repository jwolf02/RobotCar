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
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>

class ServerSocket : public Socket {
public:

	ServerSocket() = default;

	ServerSocket(Socket::ConnectionType type, int port_or_channel);

	virtual ~ServerSocket();

	void waitForConnection();

	virtual void close();

private:

    void createInetSocket(int port);

    void createBtSocket(int channel);

	int connfd = 0;

	struct sockaddr_in inet_address = { 0 };

	struct sockaddr_rc bt_address = { 0 };

};

#endif // __SERVERSOCKET_HPP



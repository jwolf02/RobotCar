#ifndef __CLIENTSOCKET_HPP
#define __CLIENTSOCKET_HPP

#include <net/Socket.hpp>
#include <string>
#include <netinet/in.h>

class ClientSocket : public Socket {
public:

	ClientSocket() = default;

	~ClientSocket();

	void connect(const std::string &addr, int port);

private:

	struct sockaddr_in serv_addr;

};

#endif // __CLIENTSOCKET_HPP

#ifndef __CLIENTSOCKET_HPP
#define __CLIENTSOCKET_HPP

#include <Socket.hpp>
#include <string>
#include <netinet/in.h>

class ClientSocket : public Socket {
public:

	ClientSocket() = default;

	~ClientSocket();

	void connect(const std::string &addr, int port);

private:

	struct sockaddr_in serv_addr = { 0 };

};

#endif // __CLIENTSOCKET_HPP

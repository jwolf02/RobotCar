#ifndef __CLIENTSOCKET_HPP
#define __CLIENTSOCKET_HPP

#include <Socket.hpp>
#include <string>
#include <netinet/in.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>

class ClientSocket : public Socket {
public:

	ClientSocket() = default;

	explicit ClientSocket(Socket::ConnectionType type);

	~ClientSocket() override;

	void connect(const std::string &addr, int port_or_channel);

	void close() override;

private:

	struct sockaddr_in inet_address = { 0 };

	struct sockaddr_rc bt_address = { 0 };

};

#endif // __CLIENTSOCKET_HPP

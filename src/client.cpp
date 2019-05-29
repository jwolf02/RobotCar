#include <iostream>
#include <net/ClientSocket.hpp>
#include <cstring>
#include <string>
#include <terminal.hpp>
#include <cstdio>
#include <cstdlib>

int main(int argc, const char *argv[]) {

	if (argc < 3) {
		std::cout << "Usage: " << argv[0] << " <address> <port>" << std::endl;
		return 1;
	}

	std::string addr = argv[1];
	int port = strtol(argv[2], nullptr, 10);

	std::cout << "connecting to " << addr << ":" << port << std::endl;

	auto socket = ClientSocket();
	try {
		socket.connect(addr, port);
	} catch (std::exception &e) {
		std::cout << e.what() << std::endl;
		exit(1);
	}

	std::cout << "connection established" << std::endl;

	terminal::set_echo_off();
	terminal::set_canonical_off();

	char c;
	while ((c = terminal::getch()) != 0x00 && c != 'x') {
		try {
			printf("pressed %c\n\r", c);
			socket.send(&c, 1);
		} catch (std::exception &e) {
			printf("%s\n\r", e.what());
		}
	}
	printf("%c\n\r", c);

	socket.close();

	return 0;
}

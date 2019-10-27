#include <iostream>
#include <ClientSocket.hpp>
#include <string>
#include <terminal.hpp>
#include <cstdio>
#include <cstdlib>
#include <vector>

#define PORT        8225

int main(int argc, const char *argv[]) {

    const std::vector<std::string> args(argv, argv + argc);

	if (argc < 2) {
		std::cout << "Usage: " << argv[0] << " <address>" << std::endl;
		return 1;
	}

    auto socket = ClientSocket(Socket::Inet);

	const std::string addr = argv[argc - 1];
    const int port = PORT;

    std::cout << "connecting to " << addr << ":" << port << std::endl;
    try {
        socket.connect(addr, port);
    } catch (std::exception &e) {
        std::cout << e.what() << std::endl;
        exit(1);
    }
	std::cout << "connection established" << std::endl;

	terminal::set_echo_off();
	terminal::set_canonical_off();

	bool terminate = false;
	char c=0x00;

	while (!terminate && (c = terminal::getch()) != 0x00) {
		try {
			socket.send(&c, 1);
		} catch (std::exception &e) {
			printf("%s\n\r", e.what());
		}

		if (c == 'x') {
		    terminate = true;
		}
	}

	printf("connection closed\n\r");
	socket.close();

	return 0;
}

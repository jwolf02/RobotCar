#include <iostream>
#include <ClientSocket.hpp>
#include <cstring>
#include <string>
#include <terminal.hpp>
#include <cstdio>
#include <cstdlib>

// operation modes
#define DRIVE		0
#define ROTATE		1

void printMenu(int operation, double speed) {
    terminal::clear();
    const char *op = operation ? "ROTATE" : "DRIVE";
    printf("operation:\t %s\n\r", op);
    printf("speed:\t %.0f%%\n\r", speed * 100);
}

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

	printMenu(0, 0);

	char c;
	while ((c = terminal::getch()) != 0x00) {
        double speed = 0.0;
        uint32_t operation = DRIVE;

		try {
		    // send input
			socket.send(&c, 1);

			// receive speed and mode of operation
			socket.recv(&speed, sizeof(speed));
			socket.recv(&operation, sizeof(operation));

		} catch (std::exception &e) {
			printf("%s\n\r", e.what());
		}

		printMenu(operation, speed);

		if (c == 'x')
		    break;
	}

	printf("connection closed\n\r");
	socket.close();

	return 0;
}

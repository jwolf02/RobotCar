#include <ServerSocket.hpp>
#include <util.hpp>
#include <controller.hpp>
#include <iostream>
#include <map>

#define DRIVE		0
#define ROTATE		1

#define PORT        8225

// ./controller [--input_type={ keyboard | socket | controller }]
int main(int argc, const char *argv[]) {

    const std::vector<std::string> args(argv, argv + argc);
    if (args.size() > 1) {

    }

    // connect to monitor
    const int port = PORT;

    std::cout << "waiting for connection..." << std::endl;
    ServerSocket socket;
    try {
        socket = ServerSocket(Socket::Inet, port);
        socket.waitForConnection();
    } catch (std::exception &ex) {
        std::cout << ex.what() << std::endl;
        std::cout << "connection failed" << std::endl;
    }
    std::cout << "connection established" << std::endl;

    // map keyboard inputs to actions for controller
	const std::map<char, unsigned int> actions = {
            { 'q', controller::STOP },
            { 'w', controller::DRIVE_FORWARD },
            { 's', controller::DRIVE_BACKWARD },
            { 'a', controller::ROTATE_LEFT },
            { 'd', controller::ROTATE_RIGHT }
	};

    bool terminate = false;
    char c = 0x00;

	while (!terminate) {
	    socket.recv(&c, 1);

	    if (c == 'x') {
            terminate = true;
        } else {
	        const auto it = actions.find(c);
	        if (it != actions.end()) {
	            controller::action(it->second);
	        } else {
                std::cout << "unrecognized action \'" << c << '\'' << std::endl;
            }
	    }
	}

    std::cout << "connection closed" << std::endl;

	socket.close();
	controller::cleanup();

	return EXIT_SUCCESS;
}


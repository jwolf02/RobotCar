#include <cstdio>
#include <cstdlib>
#include <ServerSocket.hpp>
#include <iostream>
#include <RobotCar.hpp>
#include <util.hpp>
#include <config.hpp>

#define DRIVE		0
#define ROTATE		1

#define STEP        1

// program arguments
// connection={inet, bluetooth}
// port=PORTNO
// config=CONFIG_FILE
int main(int argc, const char *argv[]) {

    const std::vector<std::string> args(argv, argv + argc);

	if (args.size() > 1 && args[1].find("--config=")) {
        const std::string config_path = args[1].substr(9);

        std::cout << "loading config file '" << config_path << '\'' << std::endl;

        try {
            config::load(config_path);
        } catch (std::exception &ex) {
            std::cout << ex.what() << std::endl;
        }
    }

    // H-Bridge pins
    int ENA = config::get_or_default("ENA", 20);
    int IN1 = config::get_or_default("IN1", 6);
    int IN2 = config::get_or_default("IN2", 13);
    int IN3 = config::get_or_default("IN3", 19);
    int IN4 = config::get_or_default("IN4", 26);
    int ENB = config::get_or_default("ENB", 21);

    const int port = config::get_or_default("PORT", 8225);

    printf("wait for connection...\n");
    auto socket = ServerSocket(Socket::Inet, port);
    socket.waitForConnection();
    printf("connection established\n");

	// setup defaults
	double motor_a_speed = 0.0, motor_b_speed = 0.0;
	int operation = DRIVE;
	RobotCar::setup(ENA, IN1, IN2, IN3, IN4, ENB);

	char c;
	do {
	    c = 0x00;

	    try {
	        while (socket.recv(&c, 1) == 0)
	            ;
	    } catch (std::exception &e) {
	        printf("%s\n\r", e.what());
	    }

		switch (c) {
            case 'a': {
                if (operation == ROTATE && motor_a_speed < .99) {
                    motor_a_speed += STEP;
                    motor_b_speed += STEP;
                } else if (operation == DRIVE) {
                    operation = ROTATE;
                    motor_a_speed = STEP;
                    motor_b_speed = STEP;
                }
                break;
            }
            case 'd': {
                if (operation == ROTATE && motor_a_speed > -0.99) {
                    motor_a_speed -= STEP;
                    motor_b_speed -= STEP;
                } else if (operation == DRIVE) {
                    operation = ROTATE;
                    motor_a_speed = -STEP;
                    motor_b_speed = -STEP;
                }
                break;
            }
            case 's': {
                if (operation == DRIVE && motor_a_speed < 0.99) {
                    motor_a_speed += STEP;
                    motor_b_speed -= STEP;
                } else if (operation == ROTATE) {
                    operation = DRIVE;
                    motor_a_speed = STEP;
                    motor_b_speed = -STEP;
                }
                break;
            }
            case 'w': {
                if (operation == DRIVE && motor_a_speed > -0.99) {
                    motor_a_speed -= STEP;
                    motor_b_speed += STEP;
                } else if (operation == ROTATE) {
                    operation = DRIVE;
                    motor_a_speed = -STEP;
                    motor_b_speed = STEP;
                }
                break;
            } case 'q' : {
                operation = DRIVE;
                motor_a_speed = 0.0;
                motor_b_speed = 0.0;
                break;
            }
            default: {
                if (c != 'x')
                    printf("invalid input encountered: %c\n", c);
            }
        }

	    try {
	        double s = motor_a_speed >= 0 ? motor_a_speed : -motor_a_speed;
	        uint32_t op = operation;
	        socket.send(&s, sizeof(s));
	        socket.send(&op, sizeof(op));
	    } catch (std::exception &e) {
            printf("%s\n\r", e.what());
            exit(EXIT_FAILURE);
	    }

		RobotCar::drive(motor_a_speed, motor_b_speed);
	} while (c != 'x');

    printf("connection closed\n");
    socket.close();

	return EXIT_SUCCESS;
}


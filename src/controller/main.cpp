#include <unistd.h>
#include <terminal.hpp>
#include <cstdio>
#include <cstdlib>
#include <L298NHBridge.hpp>
#include <ServerSocket.hpp>
#include <iostream>

#define DRIVE		0
#define ROTATE		1

#define STEP		0.2

// H-Bridge pins
int ENA = 20;
int IN1 = 6;
int IN2 = 13;
int IN3 = 19;
int IN4 = 26;
int ENB = 21;

int main(int argc, const char *argv[]) {

	if (argc < 2) {
	    std::cout << "Usage: " << argv[0] << " <port>" << std::endl;
	    exit(1);
	}

	int port = strtol(argv[1], nullptr, 10);

	printf("wait for connection...\n");
	auto socket = ServerSocket(Socket::Inet, port);
	socket.waitForConnection();

	printf("connection established\n");

	// setup defaults
	double motor_a_speed = 0.0, motor_b_speed = 0.0;
	int operation = DRIVE;
	auto bridge = L298NHBridge(ENA, IN1, IN2, IN3, IN4, ENB);

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
                printf("invalid input encountered: %c\n", c);
            }
        }

	    try {
	        double s = motor_a_speed >= 0 ? motor_a_speed : -motor_a_speed;
	        uint32_t op = operation;
	        socket.send(&s, sizeof(s));
	        socket.send(&op, sizeof(op));
	    } catch (std::exception &e) {

	    }

		bridge.setMotors(motor_a_speed, motor_b_speed);
	} while (c != 'x');

    printf("connection closed\n");
    socket.close();

	return 0;
}


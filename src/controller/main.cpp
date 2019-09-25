#include <cstdio>
#include <cstdlib>
#include <ServerSocket.hpp>
#include <iostream>
#include <RobotCar.hpp>
#include <util.hpp>

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

void execute_command(const std::string &cmd) {

}

// program arguments
// connection={inet, bluetooth}
// port=PORTNO
// config=CONFIG_FILE
int main(int argc, const char *argv[]) {

    const std::vector<std::string> args(argv, argv + argc);
	if (args.size() < 2) {
	    std::cout << "Usage: " << args[0] << " [--bluetooth] <port>" << std::endl;
	    exit(EXIT_FAILURE);
	}

	Socket::ConnectionType type = Socket::Inet;
	if (args.size() > 2 && args[1] == "--bluetooth") {
	    type = Socket::Bluetooth;
	}

	const int port = util::strto<int>(args.back());

	printf("wait for connection...\n");
	auto socket = ServerSocket(type, port);
	socket.waitForConnection();

	printf("connection established\n");

	// setup defaults
	double motor_a_speed = 0.0, motor_b_speed = 0.0;
	int operation = DRIVE;
	RobotCar::setup(ENA, IN1, IN2, IN3, IN4, ENB);

	std::cout << "calibrating IMU..." << std::endl;
	RobotCar::calibrate();
	std::cout << "done" << std::endl;

	while (true) {
	    double roll, pitch, yaw;
	    RobotCar::roll_pitch_yaw(roll, pitch, yaw);
	    printf("\rroll: %.3f pitch: %.3f yaw: %.3f         ", roll, pitch, yaw);
	}

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
	        case 'c': {
	            execute_command(socket.recv());
	        }
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


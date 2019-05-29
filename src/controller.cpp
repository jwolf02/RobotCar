#include <unistd.h>
#include <terminal.hpp>
#include <cstdio>
#include <cstdlib>
//#include <L298NHBridge.hpp>
#include <net/ServerSocket.hpp>

#define DRIVE		0
#define ROTATE		1

#define STEP		0.2

#define PORT        22222

// H-Bridge pins
int ENA = 20;
int IN1 = 6;
int IN2 = 13;
int IN3 = 19;
int IN4 = 26;
int ENB = 21;

void printMenu(int operation, double left_speed, double right_speed) {
	terminal::clear();
	const char *op = operation ? "ROTATE" : "DRIVE";
	printf("operation:\t %s\n\r", op);
	printf("left speed:\t %.0f%%\n\rright speed:\t %.0f%%\n\r", left_speed * 100, right_speed * 100);
	
}

int main(int argc, const char *argv[]) {
	// get unbuffered keyboard input
	// enableRawMode();	

	printf("wait for connection...\n");
	auto socket = ServerSocket(PORT);
	socket.waitForConnection();

	printf("connection established\n");
	char d = 0;
	size_t n;
	try {
        while ((n = socket.recv(&d, 1)) > 0 && d != 'x') {
            printf("%ld pressed %x\n", n, d);
        }
        printf("%x\n\r", d);
	} catch (std::exception &e) {
	    printf("%s\n\r", e.what());
	}

	printf("connection closed\n");
	socket.close();
	return 0;

	// setup defaults
	double motor_a_speed = 0.0, motor_b_speed = 0.0;
	int operation = DRIVE;
	printMenu(0, 0, 0);
	//auto bridge = L298NHBridge(ENA, IN1, IN2, IN3, IN4, ENB);

	char c;
	while ((c = terminal::getch()) != 0x00 && c != 'x') {
		switch (c) {
			case 'a': {
				if (operation == DRIVE && motor_a_speed < .99) {
					motor_a_speed += STEP;
					motor_b_speed += STEP;
				} else if (operation == ROTATE) {
					operation = DRIVE;
					motor_a_speed = STEP;
					motor_b_speed = STEP;
				}
				break;
			} case 'd': {
				if (operation == DRIVE && motor_a_speed > -0.99) {
					motor_a_speed -= STEP;
					motor_b_speed -= STEP;
				} else if (operation == ROTATE) {
					operation = DRIVE;
					motor_a_speed = -STEP;
					motor_b_speed = -STEP;
				}
				break;
			} case 's': {
				if (operation == ROTATE && motor_a_speed < 0.99) {
					motor_a_speed += STEP;
					motor_b_speed -= STEP;
				} else if (operation == DRIVE) {
					operation = ROTATE;
					motor_a_speed = STEP;
					motor_b_speed = -STEP;
				}
				break;
			} case 'w': {
				if (operation == ROTATE && motor_a_speed > -0.99) {
					motor_a_speed -= STEP;
					motor_b_speed += STEP;
				} else if (operation == DRIVE) {
					operation = ROTATE;
					motor_a_speed = -STEP;
					motor_b_speed = STEP;
				}
				break;
			} case 'q' : {
				operation = DRIVE;
				motor_a_speed = 0.0;
				motor_b_speed = 0.0;
			}

		}
		//bridge.setMotors(motor_a_speed, motor_b_speed);
		printMenu(operation, motor_a_speed, motor_b_speed);
	}

	printf("program terminated\n");

	return 0;
}

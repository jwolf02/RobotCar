#include <unistd.h>
#include <terminal.hpp>
#include <cstdio>
#include <cstdlib>
#include <L298NHBridge.hpp>

#define DRIVE		0
#define ROTATE		1

#define STEP		1.0

// H-Bridge pins
int ENA = 20;
int IN1 = 6;
int IN2 = 13;
int IN3 = 19;
int IN4 = 26;
int ENB = 21;

void printMenu(int operation, double left_speed, double right_speed) {
	clear();
	const char *op = operation ? "ROTATE" : "DRIVE";
	printf("operation:\t %s\n\r", op);
	printf("left speed:\t %.0f%%\n\rright speed:\t %.0f%%\n\r", left_speed * 100, right_speed * 100);
	
}

int main(int argc, const char *argv[]) {
	// get unbuffered keyboard input
	enableRawMode();	

	// setup defaults
	double motor_a_speed = 0.0, motor_b_speed = 0.0;
	int operation = DRIVE;
	printMenu(0, 0, 0);
	auto bridge = L298NHBridge(ENA, IN1, IN2, IN3, IN4, ENB);

	char c;
	while ((c = getch()) != 0x00 && c != 'x') {
		switch (c) {
			case 'd': {
				if (operation == DRIVE && motor_a_speed < .99) {
					motor_a_speed += STEP;
					motor_b_speed += STEP;
				} else if (operation == ROTATE) {
					operation = DRIVE;
					motor_a_speed = STEP;
					motor_b_speed = STEP;
				}
				break;
			} case 'a': {
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
		bridge.setMotors(motor_a_speed, motor_b_speed);
		printMenu(operation, motor_a_speed, motor_b_speed);
	}

	printf("program terminated\n");

	return 0;
}


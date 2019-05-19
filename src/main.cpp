#include <unistd.h>
#include <terminal.hpp>
#include <cstdio>
#include <cstdlib>

#define DRIVE		0
#define ROTATE		1

// H-Bridge pins
int ENA = 0;
int IN1 = 0;
int IN2 = 0;
int IN3 = 0;
int IN4 = 0;
int ENB = 0;

void printMenu(int operation, double left_speed, double right_speed) {
	clear();
	const char *op = operation ? "ROTATE" : "DRIVE";
	printf("operation:\t %s\n\r", op);
	printf("left speed:\t %.0f%%\n\rright speed:\t %.0f%%\n\r", left_speed * 100, right_speed * 100);
	
}

int main(int argc, const char *argv[]) {

	enableRawMode();	

	double motor_a_speed = 0.0, motor_b_speed = 0.0;
	int operation = DRIVE;
	printMenu(0, 0, 0);

	char c;
	while (read(STDIN_FILENO, &c, 1) == 1 && c != 'q') {
		switch (c) {
			case 'w': {
				if (operation == DRIVE && motor_a_speed < .99) {
					motor_a_speed += 0.2;
					motor_b_speed += 0.2;
				} else if (operation == ROTATE) {
					operation = DRIVE;
					motor_a_speed = 0.2;
					motor_b_speed = 0.2;
				}
				break;
			} case 's': {
				if (operation == DRIVE && motor_a_speed > -0.99) {
					motor_a_speed -= 0.2;
					motor_b_speed -= 0.2;
				} else if (operation == ROTATE) {
					operation = DRIVE;
					motor_a_speed = -0.2;
					motor_b_speed = -0.2;
				}
				break;
			} case 'a': {
				if (operation == ROTATE && motor_a_speed < 0.99) {
					motor_a_speed += 0.2;
					motor_b_speed -= 0.2;
				} else if (operation == DRIVE) {
					operation = ROTATE;
					motor_a_speed = 0.2;
					motor_b_speed = -0.2;
				}
				break;
			} case 'd': {
				if (operation == ROTATE && motor_a_speed > -0.99) {
					motor_a_speed -= 0.2;
					motor_b_speed += 0.2;
				} else if (operation == DRIVE) {
					operation = ROTATE;
					motor_a_speed = -0.2;
					motor_b_speed = 0.2;
				}
				break;
			} case 'x' : {
				operation = DRIVE;
				motor_a_speed = 0.0;
				motor_b_speed = 0.0;
			}

		}
		printMenu(operation, motor_a_speed, motor_b_speed);
	}

	printf("program terminated\n");

	return 0;
}


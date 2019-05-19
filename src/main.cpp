#include <unistd.h>
#include <terminal.hpp>
#include <cstdio>
#include <cstdlib>
#include <L298NHBridge.hpp>
#include <cmath>

#define DRIVE		0
#define ROTATE		1

#define STEP		0.2

#define ABS(x)		((x) < 0 ? -(x) : (x))

// H-Bridge pins
int ENA = 20;
int IN1 = 6;
int IN2 = 13;
int IN3 = 19;
int IN4 = 26;
int ENB = 21;

void printMenu(int operation, double speed) {
	clear();
	const char *op = operation ? "ROTATE" : "DRIVE";
	
	printf("operation:\t %s\n\r", op);
	int s = (int) (speed * 100);
	printf("speed:    \t %d%%\n\r", s);
}

int main(int argc, const char *argv[]) {
	// get unbuffered keyboard input
	enableRawMode();	

	// setup defaults
	double left_speed = 0.0, right_speed = 0.0;
	int operation = DRIVE;
	printMenu(0, 0);
	auto bridge = L298NHBridge(ENA, IN1, IN2, IN3, IN4, ENB);

	char c;
	while ((c = getch()) != 0x00 && c != 'x') {
		switch (c) {
			case 'w': {
				if (operation == DRIVE && left_speed < .99) {
					left_speed += STEP;
					right_speed += STEP;
				} else if (operation == ROTATE) {
					operation = DRIVE;
					left_speed = STEP;
					right_speed = STEP;
				}
				break;
			} case 's': {
				if (operation == DRIVE && left_speed > -0.99) {
					left_speed -= STEP;
					right_speed -= STEP;
				} else if (operation == ROTATE) {
					operation = DRIVE;
					left_speed = -STEP;
					right_speed = -STEP;
				}
				break;
			} case 'd': {
				if (operation == ROTATE && left_speed > -0.99) {
					left_speed -= STEP;
					right_speed += STEP;
				} else if (operation == DRIVE) {
					operation = ROTATE;
					left_speed = -STEP;
					right_speed = STEP;
				}
				break;
			} case 'a': {
				if (operation == ROTATE && left_speed < 0.99) {
					left_speed += STEP;
					right_speed -= STEP;
				} else if (operation == DRIVE) {
					operation = ROTATE;
					left_speed = STEP;
					right_speed = -STEP;
				}
				break;
			} case 'q' : {
				operation = DRIVE;
				left_speed = 0.0;
				right_speed = 0.0;
			}

		}
		// map right and left motor mounted on car to
		// motors A and B on the H-Bridge
		bridge.setMotors(-1.0 * right_speed, left_speed);
		printMenu(operation, ABS(left_speed));
	}

	printf("program terminated\n");

	return 0;
}


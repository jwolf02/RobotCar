#include <terminal.hpp>
#include <unistd.h>
#include <termios.h>
#include <cstdlib>

// backup terminal settings
struct termios orig_termios;

void disableRawMode() {
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

void enableRawMode() {
        tcgetattr(STDIN_FILENO, &orig_termios);
        atexit(disableRawMode);

        struct termios raw = orig_termios;

        raw.c_lflag &= ~(ECHO | ICANON | ISIG);
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

char getch() {
        char c;
        read(STDIN_FILENO, &c, 1);
        return c;
}

void clear() {
	write(STDOUT_FILENO, "\x1b[2J", 4);
	write(STDOUT_FILENO, "\x1b[H", 3);
}


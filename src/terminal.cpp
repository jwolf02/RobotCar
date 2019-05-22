#include <terminal.hpp>
#include <termios.h>
#include <cstdlib>
#include <unistd.h>

#define CHECK_MODIFIED_STATE        if (!state_modified) { \
                                        tcgetattr(STDIN_FILENO, &orig_termios); \
                                        atexit(restore_default); \
                                    }

// backup terminal settings
static struct termios orig_termios;

// flag to indicate that the default terminal settings have been
// modified and need to be restored at exit
static bool state_modified = false;

void restore_default() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

void terminal::set_echo_off() {
    CHECK_MODIFIED_STATE

    struct termios settings;
    tcgetattr(STDIN_FILENO, &settings);

    settings.c_lflag &= ~(ECHO);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &settings);
}

void terminal::set_canonical_off() {
    CHECK_MODIFIED_STATE

    struct termios settings;
    tcgetattr(STDIN_FILENO, &settings);

    settings.c_lflag &= ~(ICANON);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &settings);
}

void terminal::set_signals_off() {
    CHECK_MODIFIED_STATE

    struct termios settings;
    tcgetattr(STDIN_FILENO, &settings);

    settings.c_lflag &= ~(ISIG);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &settings);
}

char terminal::getch() {
    char c;
    if (!read(STDIN_FILENO, &c, 1))
        return 0x00;
    return c;
}

void terminal::clear() {
    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);
}

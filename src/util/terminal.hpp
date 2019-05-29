#ifndef __TERMINAL_HPP
#define __TERMINAL_HPP

namespace terminal {

    // read a single unbuffered character from stdin
    char getch();

    // turn the echo mode of the terminal off, e.g. the pressed character
    // does not appear on the command line
    void set_echo_off();


    void set_canonical_off();

    // mask all signals
    void set_signals_off();

    // clear scree (like the clear command in terminal)
    void clear();

}

#endif // __TERMINAL_HPP

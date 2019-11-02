#ifndef __CONTROLLER_HPP
#define __CONTROLLER_HPP

namespace controller {

    constexpr unsigned int STOP = 0;

    constexpr unsigned int DRIVE_FORWARD = 1;

    constexpr unsigned int DRIVE_BACKWARD = 2;

    constexpr unsigned int ROTATE_LEFT = 3;

    constexpr unsigned int ROTATE_RIGHT = 4;

    void setup();

    void action(unsigned int action_number);

    void cleanup();

}

#endif // __CONTROLLER_HPP

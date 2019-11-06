#include <controller.hpp>
#include <stdexcept>
#include <L298NHBridge.hpp>

L298NHBridge bridge;

static void _stop() {
    bridge.stop_motors();
}

static void _drive_forward() {
    bridge.set_motors(1.0, -1.0);
}

static void _drive_backward() {
    bridge.set_motors(-1.0, 1.0);
}

static void _rotate_left() {
    bridge.set_motors(0.7, 0.7);
}

static void _rotate_right() {
    bridge.set_motors(-0.7, -0.7);
}

typedef void (*action_t)();

static const action_t actions[] = { _stop, _drive_forward, _drive_backward, _rotate_left, _rotate_right };

void controller::setup() {

    atexit([]{ bridge.stop_motors(); });
}

void controller::action(unsigned int action_number) {
    action_number <= 4 ? actions[action_number]() : throw std::runtime_error("invalid action");
}

#include <controller.hpp>
#include <stdexcept>
#include <L298NHBridge.hpp>

L298NHBridge bridge;

static void _stop() {
    bridge.stopMotors();
}

static void _drive_forward() {
    bridge.setMotors(1.0, -1.0);
}

static void _drive_backward() {
    bridge.setMotors(-1.0, 1.0);
}

static void _rotate_left() {
    bridge.setMotors(0.8, 0.8);
}

static void _rotate_right() {
    bridge.setMotors(-0.8, -0.8);
}

typedef void (*action_func)();

static const action_func actions[] = { _stop, _drive_forward, _drive_backward, _rotate_left, _rotate_right };

void controller::setup() {
    bridge = L298NHBridge(20, 6, 13, 19, 26, 21);
}

void controller::action(unsigned int action_number) {
    if (action_number <= 4) {
        actions[action_number]();
    } else {
        throw std::runtime_error("invalid action");
    }
}

void controller::cleanup() {
    bridge.cleanup();
}

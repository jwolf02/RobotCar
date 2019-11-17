#include <unistd.h>
#include <fcntl.h>
#include <string>
#include <stdexcept>
#include <unordered_map>
#include <fstream>
#include <filesystem>

#define SYSFS_GPIO_DIR "/sys/class/gpio"

typedef struct {
    // BCM pin number
    int pin;

    // file descriptor for value file
    int fd;

    // in/out
    int direction;

    // INPUT, OUTPUT, PWM
    int mode;

    int value;

    int duty_cycle;

    int frequency;

} channel_info_t;

static std::unordered_map<int, int> _pins;

static void gpio_export(int gpio) {
    const std::string str = SYSFS_GPIO_DIR "/gpio" + std::to_string(gpio);
    if (access(str.c_str(), F_OK) == 0) {
        return;
    }

    std::ofstream file(SYSFS_GPIO_DIR "/export");
    if (!file) {
        throw std::runtime_error("unable to export gpio pin");
    }

    file << gpio;
    file.close();
}

static void gpio_unexport(int gpio) {
    const std::string str = SYSFS_GPIO_DIR "/gpio" + std::to_string(gpio);
    if (access(str.c_str(), F_OK) != 0) {
        return;
    }

    std::ofstream file(SYSFS_GPIO_DIR "/unexport");
    if (!file) {
        throw std::runtime_error("unable to unexport gpio pin");
    }

    file << gpio;
    file.close();
}

static void gpio_set_dir(int gpio, int out_flag) {
    std::ofstream file(SYSFS_GPIO_DIR "/gpio" + std::to_string(gpio) + "/direction");
    if (!file) {
        throw std::runtime_error("cannot set direction for gpio pin");
    }

    file << (out_flag ? "out" : "in");
    file.close();
}

static int gpio_open(int gpio) {
    const std::string str = SYSFS_GPIO_DIR "/gpio" + std::to_string(gpio) + "/value";
    int fd = open(str.c_str(), O_RDWR);
    if (fd < 0) {
        throw std::runtime_error("cannot set value for gpio pin");
    }
    return fd;
}

static void cleanup() {
    for (const auto &it : _pins) {
        const int pin = it.first;
        const int fd = it.second;
        close(fd);
        gpio_unexport(pin);
    }
    _pins.clear();
}

int setupGpio() {
    atexit(cleanup);
    return 0;
}

int pinMode(int pin, int mode) {
    gpio_export(pin);
    gpio_set_dir(pin, mode);
    _pins[pin] = gpio_open(pin);
    return 0;
}

int digitalWrite(int pin, int value) {
    const auto &it = _pins.find(pin);
    if (it == _pins.end()) {
        throw std::runtime_error("trying to access pin that was not set up");
    }

    int fd = it->second;
    const char val[2] = { value ? '1' : '0', '\0' };
    if (write(fd, val, sizeof(val)) != 2) {
        throw std::runtime_error("cannot write to gpio pin");
    }

    return 0;
}

int digitalRead(int pin) {
    const auto &it = _pins.find(pin);
    if (it == _pins.end()) {
        throw std::runtime_error("trying to access pin that was not set up");
    }

    int fd = it->second;
    uint8_t ch;
    if (read(fd, &ch, sizeof(ch)) != 1) {
        throw std::runtime_error("cannot read from gpio pin");
    }

    return (int) ch;
}

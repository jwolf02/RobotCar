#include <SPIDev.hpp>

int main() {
    SPIDev spi("/dev/spidev0.0", 0, 500000, 0, 8);

    uint8_t buf[100];
    spi.transfer("Hello ", buf, 6);
    spi.transfer("World\n", 6);

    spi.close();
}
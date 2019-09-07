#ifndef __SPI_HPP
#define __SPI_HPP

#include <cstdint>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <cstddef>
#include <string>

class SPI {
public:

    static constexpr int LOOP = SPI_LOOP;
    static constexpr int LSB_FIRST = SPI_LSB_FIRST;
    static constexpr int READY = SPI_READY;
    static constexpr int THREE_WIRE = SPI_3WIRE;
    static constexpr int CS_HIGH = SPI_CS_HIGH;
    static constexpr int NO_CS = SPI_NO_CS;
    static constexpr int CPHA = SPI_CPHA;
    static constexpr int CPOL = SPI_CPOL;

    SPI() = default;

    explicit SPI(const std::string &devname="/dev/spidev0.0", uint32_t speed=500000);

    ~SPI();

    void open(const std::string &devname, int openmode=0);

    size_t transfer(void *buf, size_t n);

    void close();

    [[nodiscard]]
    uint32_t speed() const;

private:

    int _fd = -1;

    uint32_t _speed = 0;

    uint8_t _mode = 0;

    uint8_t _bits_per_word = 8;

    uint64_t _delay = 0;

};

#endif // __SPI_HPP

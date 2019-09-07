#include <SPI.hpp>
#include <stdexcept>

#define BUFFER_SIZE      4

static int open_wrapper(const char *filename, int openmode) {
    return open(filename, openmode);
}

static int close_wrapper(int fd) {
    return close(fd);
}

SPI::SPI(const std::string &devname, uint32_t speed) {
    _speed = speed;
    open(devname);
}

SPI::~SPI() {
    close();
}

void SPI::open(const std::string &devname, int openmode) {
    _fd = open_wrapper(devname.c_str(), O_RDWR);
    if (_fd < 0) {
        throw std::runtime_error("unable to open '" + devname + '\'');
    }

    _mode = openmode;
    if (ioctl(_fd, SPI_IOC_WR_MODE, &_mode) == -1 || ioctl(_fd, SPI_IOC_RD_MODE, &_mode) == -1) {
        throw std::runtime_error("cannot set spi mode");
    }

    if (ioctl(_fd, SPI_IOC_WR_BITS_PER_WORD, &_bits_per_word) == -1 || ioctl(_fd, SPI_IOC_RD_BITS_PER_WORD, &_bits_per_word) == -1) {
        throw std::runtime_error("cannot set bits per word");
    }

    if (ioctl(_fd, SPI_IOC_WR_MAX_SPEED_HZ, &_speed) == -1 || ioctl(_fd, SPI_IOC_RD_MAX_SPEED_HZ, &_speed) == -1) {
        throw std::runtime_error("cannot set speed");
    }
}

size_t SPI::transfer(void *buf, size_t n) {
    size_t bytes_transferred = 0;
    unsigned char buffer[BUFFER_SIZE] = { 0 };
    while (bytes_transferred < n) {
        const size_t m = std::min<size_t>(BUFFER_SIZE, n - bytes_transferred);

        struct spi_ioc_transfer tr = {
                .tx_buf = (unsigned long) ((char *) buf + bytes_transferred),
                .rx_buf = (unsigned long) buffer,
                .len = (unsigned int) m,
                .delay_usecs = (unsigned short) _delay,
                .speed_hz = (unsigned int) _speed,
                .bits_per_word = (unsigned char) _bits_per_word
        };

        if (ioctl(_fd, SPI_IOC_MESSAGE(1), &tr) == -1) {
            throw std::runtime_error("cannot send spi message");
        }

        for (size_t i = 0; i < m; ++i) {
            ((unsigned char *) buf)[bytes_transferred + i] = buffer[i];
        }

        bytes_transferred += m;
    }
    return bytes_transferred;
}

void SPI::close() {
    close_wrapper(_fd);
}

unsigned int SPI::speed() const {
    return _speed;
}

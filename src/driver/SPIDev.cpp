#include <SPIDev.hpp>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <stdexcept>

#define MEMCPY(dest, src, len)      for (int __i = 0; __i < len; ++__i) \
                                        ((uint8_t *) dest)[__i] = ((const uint8_t *)src)[__i];

#define BUFFER_SIZE                 32

// wrapper around syscall with same name as member function
static int _close(int fd) {
    return close(fd);
}

static int _open(const char *fname, int mode) {
    return open(fname, mode);
}

SPIDev::SPIDev(const std::string &fname, uint32_t mode) {
    open(fname, mode);
}

SPIDev::~SPIDev() {
    close();
}

void SPIDev::open(const std::string &fname, uint32_t mode) {
    _fd = _open(fname.c_str(), O_RDWR);
    if (_fd < 0) {
        throw std::runtime_error("cannot open SPI device");
    }
    _mode = mode;
    if (ioctl(_fd, SPI_IOC_RD_MODE, &_mode) < 0 || ioctl(_fd, SPI_IOC_WR_MODE, &_mode) < 0) {
        throw std::runtime_error("cannot set SPI device mode");
    }
}

void SPIDev::transfer(const void * sendbuf, void *recvbuf, uint32_t n) {
    struct spi_ioc_transfer s = { 0 };
    s.tx_buf = (unsigned long) sendbuf;
    s.rx_buf = (unsigned long) recvbuf;
    s.len = n;
    s.delay_usecs = _delay;
    s.speed_hz = _speed;
    s.bits_per_word = _bpw;
    if (ioctl(_fd, SPI_IOC_MESSAGE(1), &s) < 0) {
        throw std::runtime_error("cannot send SPI message");
    }
}

void SPIDev::transfer(const void * sendbuf, uint32_t n) {
    uint8_t tmp[BUFFER_SIZE] = { 0 };
    for (uint32_t c = 0; c < n; c += BUFFER_SIZE) {
        transfer(((const uint8_t *)sendbuf) + c, tmp, std::min<uint32_t>(n - c, BUFFER_SIZE));
    }
}

void SPIDev::close() {
    _close(_fd);
}

void SPIDev::setSpeed(uint32_t speed) {
    _speed = speed;
    if (ioctl(_fd, SPI_IOC_RD_MAX_SPEED_HZ, &_speed) < 0 || ioctl(_fd, SPI_IOC_WR_MAX_SPEED_HZ, &_speed) < 0) {
        throw std::runtime_error("cannot set SPI speed");
    }
}

uint32_t SPIDev::speed() const {
    return _speed;
}

void SPIDev::setDelay(uint32_t delay) {
    _delay = delay;
}

uint32_t SPIDev::delay() const {
    return _delay;
}

void SPIDev::setBitsPerWord(uint32_t bpw) {
    _bpw = bpw;
    if (ioctl(_fd, SPI_IOC_RD_BITS_PER_WORD, &_bpw) < 0 || ioctl(_fd, SPI_IOC_WR_BITS_PER_WORD) < 0) {
        throw std::runtime_error("cannot set SPI bits per word");
    }
}

uint32_t SPIDev::bitsPerWord() const {
    return _bpw;
}

uint32_t SPIDev::getMode() const {
    return _mode;
}

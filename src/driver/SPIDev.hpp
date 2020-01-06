#ifndef __SPIDEV_HPP
#define __SPIDEV_HPP

#include <cstdint>
#include <string>
#include <linux/spi/spidev.h>

/***
 * SPI device wrapper class
 */
class SPIDev {
public:

    /// default contructor
    SPIDev() = default;

    /// constructor, wrapper around open, can set various values if non-zero
    explicit SPIDev(const std::string &fname, uint32_t mode=0, uint32_t speed=0, uint32_t delay=0, uint32_t bpw=0);

    /// destructor
    ~SPIDev();

    /***
     * open SPI device and set the mode
     * @param fname
     * @param mode
     */
    void open(const std::string &fname, uint32_t mode=0);

    /***
     * transfer n bytes from sendbuf and store the bytes
     * returned from the slave in recvbuf
     * @param sendbuf
     * @param recvbuf
     * @param n
     */
    void transfer(const void *sendbuf, void *recvbuf, uint32_t n);

    /***
     * send n bytes from sendbuf to slave without returning the
     * slave's output
     * @param sendbuf
     * @param n
     */
    void transfer(const void *sendbuf, uint32_t n);

    /***
     * close the device
     */
    void close();

    /***
     * set the speed for the interface
     * @param speed
     */
    void setSpeed(uint32_t speed);

    /***
     * get the interface speed
     * @return
     */
    uint32_t speed() const;

    /***
     * set the delay in usec for the interface
     * @param delay
     */
    void setDelay(uint32_t delay);

    /***
     * get the delay
     * @return
     */
    uint32_t delay() const;

    /***
     * set the number of bits per word for the interface
     * @param bpw
     */
    void setBitsPerWord(uint32_t bpw);

    /***
     * get the bits per word
     * @return
     */
    uint32_t bitsPerWord() const;

    /***
     * get the mode word
     * @return
     */
    uint32_t getMode() const;

private:

    int _fd = 0; // file descriptor

    uint32_t _speed = 0; // speed in hz

    uint32_t _delay = 0; // delay in usec

    uint32_t _bpw = 8; // bytes per word

    uint32_t _mode = 0; // bit field indicating the mode

};

#endif // __SPIDEV_HPP

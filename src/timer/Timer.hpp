#ifndef __TIMER_HPP
#define __TIMER_HPP

#include <unistd.h>
#include <functional>
#include <cstdint>
#include <ctime>
#include <chrono>

/***
 * Wrapper class around POSIX timers
 */
class Timer {
public:

    // clock types
    enum {
        MONOTONIC_CLOCK = CLOCK_MONOTONIC,
        REALTIME_CLOCK = CLOCK_REALTIME,
        BOOTTIME_CLOCK = CLOCK_BOOTTIME
    };

    // factor values to determine the units in which the time is given
    // by default MILLISECONDS
    enum {
        NANOSECONDS = 1000000000,
        MICROSECONDS = 1000000,
        MILLISECONDS = 1000,
        SECONDS = 1
    };

    /// default constructor
    Timer() = default;

    /***
     * create new timer, when expire time is reached, handler is called
     * if interval_time is not zero, then this is repeated again
     * @param func handler function
     * @param expire_time time until first expiration
     * @param interval_time repeated expiration time
     * @param clockid clock type
     * @param factor how times are interpreted
     */
    Timer(std::function<void (void)> &&func, uint64_t expire_time, uint64_t interval_time=0,
            int clockid=MONOTONIC_CLOCK, int factor=MILLISECONDS);

    /// prevent timer copy
    Timer(const Timer &timer) = delete;

    /***
     * move assign a timer by swapping timers
     * @param timer
     */
    Timer(Timer &&timer) noexcept;

    /***
     * delete timer
     */
    ~Timer();

    /// prevent timer copy
    Timer& operator=(const Timer &timer) = delete;

    /***
     * allow move assignment
     * @param timer
     * @return
     */
    Timer& operator=(Timer &&timer) noexcept;

    /***
     * get the time until expiration
     * @param factor how times are interpreted
     * @return
     */
    uint64_t getTimeUntilExpiration(int factor=MILLISECONDS) const;

    /***
     * change time on timer
     * @param expire_time time till first expiration
     * @param interval_time
     * @param factor how times are interpreted
     */
    void setTime(uint64_t expire_time, uint64_t interval_time=0, int factor=MILLISECONDS);

    /***
     * delete timer
     */
    void stop();

    /***
     * get the number of missed invocations
     * @return
     */
    uint32_t getOverruns() const;

    /***
     * get the interval time
     * @param factor how times are interpreted
     * @return
     */
    uint64_t getIntervalTime(int factor=MILLISECONDS) const;

    /***
     * call the handler
     */
    void callHandler() const;

    /***
     * swap timers
     * @param timer
     */
    void swap(Timer &timer);

private:

    std::function<void (void)> _handler; // handler to be called opon expiration

    timer_t _timer = nullptr; // basic handle for POSIX timers

    uint64_t _interval_time = 0; // repetition time

};

#endif // __TIMER_HPP

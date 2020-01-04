#ifndef __TIMER_HPP
#define __TIMER_HPP

#include <unistd.h>
#include <functional>
#include <cstdint>
#include <ctime>
#include <chrono>
#include <csignal>

// macro to auto convert to microseconds
#define SECONDS(x)          ((x) * 1000000)
#define MILLISECONDS(x)     ((x) * 1000)
#define MICROSECONDS(x)     (x)
#define NANOSECONDS(x)      ((x) / 1000)

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

    /***
     * create a single shot timer, which calls its handler after expire_time
     * and then gets deleted automatically
     * a timer created this way can neither be stopped or gets its time changed
     * @param func handler to be called upon expiration
     * @param expire_time in usec
     * @param clockid
     */
    static void singleShot(const std::function<void (void)> &func, uint64_t expire_time,
                                int signum=SIGALRM, int clockid=MONOTONIC_CLOCK);

    /// default constructor
    Timer() = default;

    /***
     * create new timer, when expire time is reached, handler is called
     * if interval_time is not zero, then this is repeated again
     * @param func handler function
     * @param expire_time time until first expiration in usec
     * @param interval_time repeated expiration time
     * @param clockid clock type
     */
    Timer(std::function<void (void)> &&func, uint64_t expire_time, uint64_t interval_time=0,
            int signum=SIGALRM, int clockid=MONOTONIC_CLOCK);

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
     * get the time until expiration in usec
     * @return
     */
    uint64_t getTimeUntilExpiration() const;

    /***
     * change time on timer
     * @param expire_time time till first expiration in usec
     * @param interval_time in usec
     * @param factor how times are interpreted
     */
    void setTime(uint64_t expire_time, uint64_t interval_time=0);

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
     * get the interval time in usec
     * @param factor how times are interpreted
     * @return
     */
    uint64_t getIntervalTime() const;

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

    std::function<void (void)> _handler; // handler to be called upon expiration

    timer_t _timer = nullptr; // basic handle for POSIX timers

    uint64_t _interval_time = 0; // repetition time in nsec

};

#endif // __TIMER_HPP

#ifndef __TIMER_HPP
#define __TIMER_HPP

#include <unistd.h>
#include <string>
#include <functional>

namespace timer {

    /*
     * get the number of active timers
     */
    unsigned int active_timers();

    /*
     * setup a timer to go off after expire_time milliseconds and repeat itself after
     * every interval_time. whenever it goes off func is called
     */
    timer_t create(const std::function<void (void)> &action, uint64_t expire_time, uint64_t interval_time=0,
            clockid_t clockid=CLOCK_MONOTONIC);

    /*
     * reset the time
     */
    void set_time(timer_t timer, uint64_t expire_time, uint64_t interval_time=0);

    /*
     * get the remaining time until the next expiration in milliseconds
     */
    uint64_t get_time(timer_t timer);

    /*
     * delete the timer
     */
    void stop(timer_t timer);

}

#endif // __TIMER_HPP

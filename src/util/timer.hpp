#ifndef __TIMER_HPP
#define __TIMER_HPP

#include <unistd.h>
#include <string>
#include <functional>

namespace timer {

    /*
     * get the maximum number of available timers
     */
    int max_timers();

    /*
     * get the number of active timers
     */
    int num_timers();

    /*
     * setup a timer to go off after expire_time milliseconds and repeat itself after
     * every interval_time. whenever it goes off func is called
     */
    timer_t create(const std::function<void (void)> &action, uint64_t expire_time, uint64_t interval_time,
            clockid_t clockid=CLOCK_REALTIME);

    /*
     * reset the time
     */
    void set_time(timer_t timer, uint64_t expire_time, uint64_t interval_time);

    /*
     * get the remaining time until the next expiration in milliseconds
     */
    uint64_t get_time(timer_t timer);

    /*
     * delete the timer
     */
    void disarm(timer_t timer);

}

#endif // __TIMER_HPP

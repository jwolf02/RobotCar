#ifndef __CLOCK_HPP
#define __CLOCK_HPP

#include <chrono>
#include <cmath>
#include <type_traits>

#define STEADY_CLOCK            std::chrono::steady_clock
#define SYSTEM_CLOCK            std::chrono::system_clock

template <typename ClockType>
class Clock {
public:

    static_assert(std::is_same<ClockType, STEADY_CLOCK>::value || std::is_same<ClockType, SYSTEM_CLOCK>::value,
            "invalid template parameter");

    Clock() = default;

    void start() {
        if (std::is_same<ClockType, STEADY_CLOCK>::value) {
            _begin = std::chrono::steady_clock::now();
        } else if (std::is_same<ClockType, SYSTEM_CLOCK>::value) {
            _begin = std::chrono::system_clock::now();
        }
    }

    void stop() {
        if (std::is_same<ClockType, STEADY_CLOCK>::value) {
            _end = std::chrono::steady_clock::now();
        } else if (std::is_same<ClockType, SYSTEM_CLOCK>::value) {
            _end = std::chrono::system_clock::now();
        }
    }

    double nanoseconds() const {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(_end - _begin).count();
    }

    double microseconds() const {
        return std::chrono::duration_cast<std::chrono::microseconds>(_end - _begin).count();
    }

    double milliseconds() const {
        return std::chrono::duration_cast<std::chrono::milliseconds>(_end - _begin).count();
    }

    double seconds() const {
        return std::chrono::duration_cast<std::chrono::seconds>(_end - _begin).count();
    }

    double minutes() const {
        return std::trunc(seconds() / 60.0);
    }

    double hours() const {
        return std::trunc(seconds() / 3600.0);
    }

    double days() const {
        return std::trunc(seconds() / (3600.0 * 24.0));
    }

private:

    std::chrono::time_point<ClockType, std::chrono::nanoseconds> _begin;

    std::chrono::time_point<ClockType, std::chrono::nanoseconds> _end;

};

using SteadyClock = Clock<STEADY_CLOCK>;
using SystemClock = Clock<SYSTEM_CLOCK>;

#endif // __CLOCK_HPP

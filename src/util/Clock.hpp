#ifndef __CLOCK_HPP
#define __CLOCK_HPP

#include <chrono>
#include <cmath>
#include <type_traits>

class Clock {
public:

    Clock() = default;

    void start() {
        _begin = std::chrono::system_clock::now();
    }

    void stop() {
        _end = std::chrono::system_clock::now();
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
        return std::chrono::duration_cast<std::chrono::minutes>(_end - _begin).count();
    }

    double hours() const {
        return std::chrono::duration_cast<std::chrono::hours>(_end - _begin).count();
    }

private:

    std::chrono::system_clock::time_point _begin;

    std::chrono::system_clock::time_point _end;

};

#endif // __CLOCK_HPP

#include <Timer.hpp>
#include <ctime>
#include <csignal>
#include <stdexcept>

static void _signal_handler(int signum, siginfo_t *siginfo, void *context) {
    const auto *timer = static_cast<const Timer*>(siginfo->si_value.sival_ptr);
    timer->callHandler();
}

static void _setup_handler(int signum) {
    static bool handler_setup = false;
    if (!handler_setup) {
        struct sigaction sa = { nullptr };
        sa.sa_flags = SA_SIGINFO;
        sa.sa_sigaction = _signal_handler;
        sigemptyset(&sa.sa_mask);
        if (sigaction(signum, &sa, nullptr) == -1) {
            throw std::runtime_error("setting up signal handler failed");
        }
        handler_setup = true;
    }
}

void Timer::singleShot(const std::function<void (void)> &func, uint64_t expire_time, int signum, int clockid) {
    Timer *timer = nullptr;
    timer = new Timer([=]{ func(); delete timer; }, expire_time, 0, signum, clockid);
}

Timer::Timer(std::function<void(void)> &&func, uint64_t expire_time, uint64_t interval_time, int signum, int clockid) {
    _setup_handler(signum);

    // move assign handler
    _handler = std::forward<std::function<void(void)>>(func);

    // create timer
    struct sigevent te = { 0 };
    te.sigev_notify = SIGEV_SIGNAL;
    te.sigev_signo = signum;
    te.sigev_value.sival_ptr = static_cast<void*>(this);
    if (timer_create(clockid, &te, &_timer) < 0) {
        throw std::runtime_error("creating timer failed");
    }

    // set time on timer
    setTime(expire_time, interval_time);
}

Timer::Timer(Timer &&timer) noexcept {
    swap(timer);
}

Timer::~Timer() {
    stop();
}

Timer& Timer::operator=(Timer &&timer) noexcept {
    swap(timer);
    return *this;
}

uint64_t Timer::getTimeUntilExpiration() const {
    struct itimerspec its = { 0 };
    if (timer_gettime(_timer, &its) < 0) {
        throw std::runtime_error("getting time failed");
    }
    return static_cast<uint64_t>(its.it_value.tv_sec * 1000000 + its.it_value.tv_nsec / 1000);
}

void Timer::setTime(uint64_t expire_time, uint64_t interval_time) {
    struct itimerspec its = { 0 };
    // compute interval time in seconds/nanoseconds
    its.it_interval.tv_sec = interval_time / 1000000;
    its.it_interval.tv_nsec = (interval_time % 1000000) * 1000;
    // compute expire time in seconds/nanoseconds
    its.it_value.tv_sec = expire_time / 1000000;
    its.it_value.tv_nsec = (expire_time % 1000000) * 1000;
    if (timer_settime(_timer, 0, &its, nullptr) < 0) {
        throw std::runtime_error("setting time failed");
    }
    // set time in nanoseconds
    _interval_time = interval_time * 1000;
}

void Timer::stop() {
    if (_timer != nullptr) {
        if (timer_delete(_timer) < 0) {
            throw std::runtime_error("deleting timer failed");
        }
    }
}

uint32_t Timer::getOverruns() const {
    return timer_getoverrun(_timer);
}

uint64_t Timer::getIntervalTime() const {
    return _interval_time / 1000;
}

void Timer::callHandler() const {
    if (_handler) {
        _handler();
    }
}

void Timer::swap(Timer &timer) {
    std::swap(timer._timer, _timer);
    std::swap(timer._handler, _handler);
    std::swap(timer._interval_time, _interval_time);
}

#include <iostream>

int main() {

    Timer::singleShot([]{ std::cout << "Hello World" << std::endl; }, SECONDS(2));

    while (true)
        pause();
}
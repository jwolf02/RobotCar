#include <timer.hpp>
#include <ctime>
#include <csignal>
#include <stdexcept>
#include <unordered_map>

static std::unordered_map<int, std::function<void (void)>> _map;

static std::unordered_map<timer_t, int> _timer_map;

static bool _valid_timer(timer_t timer) {
    return _timer_map.find(timer) != _timer_map.end();
}

static void _signal_handler(int signum, siginfo_t *siginfo, void *context) {
    auto it = _map.find(signum);
    if (it != _map.end()) {
        (*it).second();
    }
}

int timer::max_timers() {
    return SIGRTMAX - SIGRTMIN;
}

int timer::num_timers() {
    return _map.size();
}

timer_t timer::create(const std::function<void (void)> &func, uint64_t expire_time, uint64_t interval_time, clockid_t clockid) {
    struct sigevent te = { 0 };
    struct sigaction sa = { 0 };
    timer_t timer = nullptr;

    // select smallest signum that is not already registered
    int signum = SIGRTMIN;
    while (signum <= SIGRTMAX && _map.find(signum) != _map.end())
        signum++;
    if (signum > SIGRTMAX) {
        throw std::runtime_error("maximum number of per process timers exceeded");
    }

    // if it is the first call, register signal handler
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = _signal_handler;
    sigemptyset(&sa.sa_mask);
    if (sigaction(signum, &sa, nullptr) == -1) {
        throw std::runtime_error("failed to setup signal handler");
    }

    // create timer
    te.sigev_notify = SIGEV_SIGNAL;
    te.sigev_signo = signum;
    te.sigev_value.sival_ptr = &timer;
    timer_create(clockid, &te, &timer);

    timer::set_time(timer, expire_time, interval_time);

    // add mapping of timer to function
    _map[signum] = func;

    // add mapping between timer and the signal
    _timer_map[timer] = signum;

    return timer;
}

void timer::set_time(timer_t timer, uint64_t expire_time, uint64_t interval_time) {
    if (!_valid_timer(timer)) {
        throw std::runtime_error("invalid timer");
    }
    struct itimerspec its = { 0 };
    its.it_interval.tv_sec = interval_time / 1000;
    its.it_interval.tv_nsec = (interval_time % 1000) * 1000000;
    its.it_value.tv_sec = expire_time / 1000;
    its.it_value.tv_nsec = (expire_time % 1000) * 1000000;
    timer_settime(timer, 0, &its, nullptr);
}

uint64_t timer::get_time(timer_t timer) {
    if (!_valid_timer(timer)) {
        throw std::runtime_error("invalid timer");
    }
    struct itimerspec its = { 0 };
    timer_gettime(timer, &its);
    return its.it_value.tv_sec * 1000 - its.it_value.tv_nsec / 1000000;
}

void timer::disarm(timer_t timer) {
    if (!_valid_timer(timer)) {
        throw std::runtime_error("invalid timer");
    }
    auto signum = _timer_map[timer];
    _timer_map.erase(timer);
    _map.erase(signum);
    timer_delete(timer);
}
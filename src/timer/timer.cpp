#include <timer.hpp>
#include <ctime>
#include <csignal>
#include <stdexcept>
#include <unordered_map>
#include <thread>
#include <condition_variable>
#include <queue>
#include <atomic>

// using a hash map to provide amortised constant time access
static std::unordered_map<timer_t, std::function<void (void)>> _map;

// indicate if the signal handler is already setup
static bool _handler_setup = false;

// seperate handler thread
static std::thread _thread;

// thread safe queue to enqueue function to be called
static std::queue<std::function<void (void)>> _queue;

// mutex to make queue thread safe
static std::mutex _mtx;

// signal handler thread to wake up
static std::condition_variable _cond;

// number of unhandled timers
static std::atomic_uint32_t _missed_timers(0);

// flag to signal thread to execute
static volatile bool _exec = false;

static void _signal_handler(int signum, siginfo_t *siginfo, void *context) {
    // only process signal if signal number is correct and
    // there is currently no active signal handler executing
    // this bears the small chance that timers expire without notice
    static std::atomic_bool _flag(false);
    bool t = true, f = false;
    if (!_flag.compare_exchange_strong(f, t)) {
        auto timer_ptr = (timer_t *) siginfo->si_value.sival_ptr;
        auto it = _map.find(*timer_ptr);
        if (it != _map.end()) {
            std::unique_lock<std::mutex> lock(_mtx);
            _queue.push((*it).second);
            _mtx.unlock();
            _cond.notify_all();
        }
        _flag = false;
    } else {
        _missed_timers += 1;
    }
}

static void _thread_func() {
    while (_exec) {
        std::unique_lock<std::mutex> lock(_mtx);
        while (_queue.empty()) {
            _cond.wait(lock, []{ return _queue.empty(); });
        }
        auto func = _queue.front();
        _queue.pop();
        func();
    }
}

static void _terminate_thread() {
    _exec = false;
    _cond.notify_all();
}

unsigned int timer::active_timers() {
    return _map.size();
}

timer_t timer::create(const std::function<void (void)> &func, uint64_t expire_time, uint64_t interval_time, clockid_t clockid) {
    struct sigevent te = { 0 };
    struct sigaction sa = { 0 };
    auto timer = (timer_t *) new timer_t;
    const int signum = SIGALRM;

    // if it is the first call, register signal handler
    if (!_handler_setup) {
        sa.sa_flags = SA_SIGINFO;
        sa.sa_sigaction = _signal_handler;
        sigemptyset(&sa.sa_mask);
        if (sigaction(signum, &sa, nullptr) == -1) {
            throw std::runtime_error("failed to setup signal handler");
        }

        _exec = true;
        // create handler thread
        _thread = std::thread(_thread_func);

        _handler_setup = true;

        atexit(_terminate_thread);
    }

    // create timer
    te.sigev_notify = SIGEV_SIGNAL;
    te.sigev_signo = signum;
    te.sigev_value.sival_ptr = timer;
    timer_create(clockid, &te, timer);

    // add mapping of timer to function
    _map[*timer] = func;

    // arm timer
    timer::set_time(*timer, expire_time, interval_time);

    return *timer;
}

void timer::set_time(timer_t timer, uint64_t expire_time, uint64_t interval_time) {
    struct itimerspec its = { 0 };
    its.it_interval.tv_sec = interval_time / 1000;
    its.it_interval.tv_nsec = (interval_time % 1000) * 1000000;
    its.it_value.tv_sec = expire_time / 1000;
    its.it_value.tv_nsec = (expire_time % 1000) * 1000000;
    timer_settime(timer, 0, &its, nullptr);
}

uint64_t timer::get_time(timer_t timer) {
    struct itimerspec its = { 0 };
    timer_gettime(timer, &its);
    return its.it_value.tv_sec * 1000 - its.it_value.tv_nsec / 1000000;
}

void timer::stop(timer_t timer) {
    _map.erase(timer);
    timer_delete(timer);
}

unsigned int timer::missed_timers() {
    return _missed_timers;
}
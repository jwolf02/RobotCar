#ifndef __SPINLOCK_HPP
#define __SPINLOCK_HPP

#include <atomic>

class SpinLock {
public:

    SpinLock() = default;

    void lock() {
       while (_lock.test_and_set(std::memory_order_acquire)) {
           #if defined(__x86_64__) || defined(__i386__)
           asm volatile("pause" ::: "memory");
           #else
           ;
           #endif
       }
    }

    bool try_lock() {
        if (!_lock.test_and_set(std::memory_order_acquire)) {
            return true;
        }
        return false;
    }

    void unlock() {
        _lock.clear(std::memory_order_release);
    }

private:

    std::atomic_flag _lock = ATOMIC_FLAG_INIT;

};

#endif // __SPINLOCK_HPP

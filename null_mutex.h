/// null_mutex
/// use null_mutex can control some template class which use mutex in
/// multi-threads has better porfarmance in single thread

#pragma once

struct NullMutex {
    void lock() const {}
    void unlock() const {}
    bool try_lock() const { return true; }
};

/*

// usage
#include <functional>
#include <mutex>
#include <thread>
#include <vector>

struct console_mutex {
    using mutex_t = std::mutex;
    static mutex_t& mutex() {
        static mutex_t s_mutex;
        return s_mutex;
    }
};

struct console_null_mutex {
    using mutex_t = NullMutex;
    static mutex_t& mutex() {
        static mutex_t s_mutex;
        return s_mutex;
    }
};

template <typename T>
class Sink {
public:
    using mutex_t = typename T::mutex_t;

    Sink() : num_(0), mutex_(T::mutex()) {}

    void add() {
        std::lock_guard<mutex_t> lock(mutex_);
        for (int i = 0; i < 100000; i++) {
            num_++;
        }
    }

    int get() { return num_; }

private:
    int num_;
    mutex_t& mutex_;
};

using sink_lock = Sink<console_mutex>;
using sink_null_lock = Sink<console_null_mutex>;

int main() {
    std::vector<std::thread> v1, v2;
    sink_lock s1;
    sink_null_lock s2;

    for (int i = 0; i < 10; i++) {
        v1.emplace_back(std::thread(&sink_lock::add, &s1));
    }

    for (auto& x : v1) {
        x.join();
    }

    printf("the mutex answer is: %d\n", s1.get());

    for (int i = 0; i < 10; i++) {
        v2.emplace_back(std::thread(&sink_null_lock::add, &s2));
    }

    for (auto& x : v2) {
        x.join();
    }

    printf("the null mutex answer is: %d\n", s2.get());

    return 0;
}

// output:
// the mutex answer is: 1000000
// the null mutex answer is: 234112

*/

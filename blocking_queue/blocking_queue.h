/**
 * @file blocking_queue.h
 * @author Ji Jie (396438446@qq.com)
 * @brief Blocking queue use circular buffer in boost, the cirular buffer is
 * a buffer which acts like a queue but has a limited size. When pushed number
 * exceed the limited size, elements pushed earlier will be discord
 * @version 0.1
 * @date 2019-08-28
 *
 * @copyright Copyright (c) 2019
 *
 */

#pragma once

#include <chrono>
#include <condition_variable>
#include <mutex>
#include "boost/circular_buffer.hpp"

/**
 * @brief Blocking queue, used in scene of producer and consumer model
 * 
 * @tparam T The type of element staged in cirular buffer 
 */
template <typename T>
class BlockingQueue {
public:
    typedef T value_type;

    /**
     * @brief Construct a new Blocking Queue object
     * 
     * @param max_size The max size of cirular buffer
     */
    explicit BlockingQueue(size_t max_size) : que_(max_size) {}
    ~BlockingQueue() = default;

    void push(T &&item) {
        std::unique_lock<std::mutex> lock(mutex_);
        push_cv_.wait(lock, [this] { return !this->que_.full(); });
        que_.push_back(std::move(item));
        pop_cv_.notify_one();
    }

    /**
     * @brief Push with no wait, will not be blocked because the circular buffer
     * is full, just discard the former element 
     * 
     * @param item The item need be pushed, should use std::forward to transfer parameter
     */
    void pushNoWait(T &&item) {
        std::unique_lock<std::mutex> lock(mutex_);
        que_.push_back(std::move(item));
        pop_cv_.notify_one();
    }

    /**
     * @brief Remove and return the front element by condition variable waiting
     * for some miliseconds
     *
     * @param popped_item Reference of item be popped from circular buffer
     * @param wait_duration The wait duration of condtion variable
     * @return true The item has been popped successfully
     * @return false The circular buffer is empty or overtime
     */
    bool popFor(T &popped_item, std::chrono::milliseconds wait_duration) {
        std::unique_lock<std::mutex> lock(mutex_);
        if (!pop_cv_.wait_for(lock, wait_duration,
                              [this] { return !this->que_.empty(); })) {
            return false;
        }
        popped_item = std::move(que_.front());
        que_.pop_front();
        push_cv_.notify_one();
        return true;
    }

    size_t size() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return que_.size();
    }

private:
    mutable std::mutex mutex_;
    std::condition_variable push_cv_;
    std::condition_variable pop_cv_;

    boost::circular_buffer<T> que_;
};

#pragma once

#include <functional>
#include <future>
#include <memory>
#include <queue>
#include <thread>
#include <vector>

/**
 * @brief 线程池
 *
 */
class ThreadPool {
public:
    ThreadPool(size_t count);
    ~ThreadPool();

    /**
     * @brief 往线程池中添加线程
     * 
     * @tparam F 
     * @tparam Args 
     * @param f 运行函数
     * @param args 运行函数的参数
     * @return std::future<decltype(f(args...))> 返回异步结果
     */
    template <typename F, typename... Args>
    auto insert(F&& f, Args&&... args) -> std::future<decltype(f(args...))>;

private:
    std::vector<std::thread> workers_;
    std::queue<std::function<void()>> tasks_;

    std::mutex queue_mutex_;
    std::condition_variable condition_;
    bool stop_;
};

//线程池初始化，所有线程均在等待
inline ThreadPool::ThreadPool(size_t count) : stop_(false) {
    for (int i = 0; i < count; i++) {
        workers_.emplace_back([this] {
            while (1) {
                std::function<void()> task;

                {
                    std::unique_lock<std::mutex> lock(this->queue_mutex_);

                    //条件变量等到直到收到stop信号或task队列不为空
                    condition_.wait(lock, [this] {
                        return this->stop_ || !this->tasks_.empty();
                    });
                    if (this->stop_ && this->tasks_.empty()) {
                        return;
                    }
                    task = std::move(tasks_.front());
                    tasks_.pop();
                }

                task();
            }
        });
    }
}

//向任务队列中加入新任务函数，利用返回类型后置语法得到任务函数返回值
template <typename F, typename... Args>
auto ThreadPool::insert(F&& f, Args&&... args)
    -> std::future<decltype(f(args...))> {

    // using表示类型别名,类似typedef
    // typename std::result_of<F(Args...)>::type
    using result_type = decltype(f(args...));

    //利用std::packaged_task封装bind，可以异步执行
    //在这里使用packaged_task的主要原因是希望能够返回result，执行完任务后可以
    //得到结果，单纯使用std::function无法得到该效果
    auto task = std::make_shared<std::packaged_task<result_type()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...));

    std::future<result_type> result = task->get_future();

    {
        std::unique_lock<std::mutex> lock(queue_mutex_);
        if (stop_) throw std::runtime_error("Insert the stopped ThreadPool");

        //利用std::function再次封装一层
        tasks_.emplace([task] { (*task)(); });
    }

    //条件变量通知正在等待的线程中的一个，达到从线程池中取出一个线程使用的目的
    condition_.notify_one();
    return result;
}

ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(queue_mutex_);
        stop_ = true;
    }

    condition_.notify_all();
    for (std::thread& worker : workers_) {
        worker.join();
    }
}
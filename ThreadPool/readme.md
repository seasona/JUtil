<!-- TOC -->

- [线程池](#%e7%ba%bf%e7%a8%8b%e6%b1%a0)
  - [基本原理](#%e5%9f%ba%e6%9c%ac%e5%8e%9f%e7%90%86)
  - [c11特性](#c11%e7%89%b9%e6%80%a7)
    - [std::packaged_task和std::future](#stdpackagedtask%e5%92%8cstdfuture)
    - [返回类型后置语法](#%e8%bf%94%e5%9b%9e%e7%b1%bb%e5%9e%8b%e5%90%8e%e7%bd%ae%e8%af%ad%e6%b3%95)

<!-- /TOC -->

# 线程池

主要是copy了一下[github](https://github.com/progschj/ThreadPool.git)中的线程池的实现

## 基本原理

线程池的实现不难，基本思路就是因为线程执行完一个任务函数就会终止该线程，所以将任务函数改为while(1)死循环，然后利用条件变量wait，再使用一个队列来保存任务，如果队列不为空就notify_one从线程池中取出一个线程处理任务

## c11特性

### std::packaged_task和std::future

这个线程池中使用了很多c11中的特性，比如利用std::function和std::bind绑定任务函数，使用std::forward完美转发等，主要写一下std::packaged_task的作用，std::packaged_task跟std::function的作用其实差不多，但是这里使用前者的原因是任务函数绑定后回调无法在线程中返回值，回调是在这里：

```cpp
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
```

task()进行任务函数的回调，但此时无法得到任务函数的返回值，理论上来说应该在线程注册函数时就可以得到返回值，std::function是无法做到这一点的，但是std::packaged_task可以返回一个std::future，std::future可以保证在任务函数执行完毕后保存它的返回值。

类模板 std::future 提供访问异步操作结果的机制：

- （通过 std::async 、 std::packaged_task 或 std::promise 创建的）异步操作能提供一个 std::future 对象给该异步操作的创建者

- 然后，异步操作的创建者能用各种方法查询、等待或从 std::future 提取值。若异步操作仍未提供值，则这些方法可能阻塞。

- 异步操作准备好发送结果给创建者时，它能通过修改链接到创建者的 std::future 的共享状态（例如 std::promise::set_value ）进行

注意， std::future 所引用的共享状态不与另一异步返回对象共享（与 std::shared_future 相反）

```cpp
vector<future<int>> results;
{
    ThreadPool pool(4);

    for (int i = 0; i < 8; i++) {
        results.emplace_back(pool.enqueue(calandprint, i));
    }
}

for (auto&& res : results) {
    cout << res.get() << " ";
}
cout << endl;
```

如上，这样就可以异步得到任务函数的返回值

### 返回类型后置语法

模板中会遇到没有办法得知返回值的类型的情况

```cpp
template <typename F, typename... Args>
auto ThreadPool::enqueue(F&& f, Args&&... args)
    -> std::future<typename std::result_of<F(Args...)>::type>
```

如上，通过返回类型后置，可以根据模板参数类型返回特有的返回值类型，也可以和decltype结合使用

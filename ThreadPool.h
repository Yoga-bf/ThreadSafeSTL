#include <iostream>
#include <future>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include "PthreadSafeQueue.h"

class ThreadPool {
private:
    bool stoped;
    std::mutex m;
    std::condition_variable m_conditional_lock;
    std::vector<std::thread> workers;
    Thread_safe_queue<std::function<void()>> tasks;
    
public:
    ThreadPool(const size_t num_threads = 4)
        : stoped(false)
    {
        for(size_t i = 0; i < num_threads; ++i) {
            workers.emplace_back([this] {
                for(;;)
                {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(this->m);
                        this->m_conditional_lock.wait(lock, [this]{return this->stoped || !this->tasks.empty();});
                        if(this->stoped && this->tasks.empty())
                            return;
                        this->tasks.pop(task);
                    }
                    task();
                }
            });
        }
    }

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool(const ThreadPool&&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&&) = delete;

    template<typename F, typename... Args>
    auto enqueue(F&& f, Args&&... args) -> std::future<decltype(std::forward<F>(f)(std::forward<Args>(args)...))>
    {
        std::function<decltype(std::forward<F>(f)(std::forward<Args>(args)...))()> func = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
        auto task_ptr = std::make_shared<std::packaged_task<decltype(std::forward<F>(f)(std::forward<Args>(args)...))()>>(func);
        std::function<void()> warpfunc = [task_ptr](){(*task_ptr)();};
        std::unique_lock<std::mutex> lock(m);
        if(stoped) throw std::runtime_error("enqueue on stopped ThreadPool");
        tasks.emplace(warpfunc);
        m_conditional_lock.notify_one();
        return task_ptr->get_future();
    }

    void stopAll()
    {
        {
            stoped = true;
            std::unique_lock<std::mutex> lock(this->m);
        }
        m_conditional_lock.notify_all();
        for(std::thread& worker : workers) {
            if(worker.joinable())
                worker.join();
        }
    }

    ~ThreadPool()
    {
        {
            
            std::unique_lock<std::mutex> lock(this->m);
        }
        stoped = true;
        m_conditional_lock.notify_all();
        
        for(std::thread& worker : workers) {
            if(worker.joinable())
                worker.join();
        }
    }
};

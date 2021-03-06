#include <iostream>
#include <mutex>
#include <queue>
#include <memory>
#include <thread>

struct empty_queue: std::exception
{
    const char* what() const throw() {
        return "empty queue!";
    };
};

template<typename T>
class Thread_safe_queue {
private:
    mutable std::mutex m;
    std::queue<T> data;
public:
    Thread_safe_queue()
        : data(std::queue<T>()) {}
    Thread_safe_queue(const Thread_safe_queue<T>& rhs)
    {
        std::lock_guard<std::mutex> lock(m);
        data = rhs.data;
    }
    Thread_safe_queue& operator=(const Thread_safe_queue&) = delete;
    Thread_safe_queue(const Thread_safe_queue<T>&& rhs) = delete;
    Thread_safe_queue& operator=(const Thread_safe_queue&&) = delete;
    bool empty() const
    {
        std::lock_guard<std::mutex> lock(m);
        return data.empty();
    }
    auto size() const
    {
        std::lock_guard<std::mutex> lock(m);
        return data.size();
    }
    void push(T& new_value)
    {
        std::lock_guard<std::mutex> lock(m);
        data.push(new_value);
    }
    void emplace(T& new_value)
    {
        std::lock_guard<std::mutex> lock(m);
        data.emplace(new_value);
    }
    bool pop(T& value)
    {
        std::lock_guard<std::mutex> lock(m);
        if(data.empty()){
            std::cout << "the queue is empty!" << std::endl;
            return false;
        };
        value = std::move(data.front());
        data.pop();
        return true;
    }
    // auto front()
    // {
    //     std::lock_guard<std::mutex> lock(m);
    //     if(data.empty()) throw empty_queue();
    //     return data.front();
    // }
    std::shared_ptr<T> pop()
    {
        std::lock_guard<std::mutex> lock(m);
        if(data.empty()) throw empty_queue();
        std::shared_ptr<T> res(std::make_shared<T>(data.front()));
        data.pop();
        return res;
    }
    ~Thread_safe_queue()
    {
        
    }
};

#include <iostream>
#include <mutex>
#include <stack>
#include <memory>

struct empty_stack: std::exception
{
    const char* what() const throw() {
        return "empty stack!";
    };
};

template<typename T>
class Thread_safe_stack {
private:
    std::stack<T> data;
    mutable std::mutex m;
public:
    Thread_safe_stack()
        : data(std::stack<T>()) {}
    Thread_safe_stack(const Thread_safe_stack<T>& rhd)
    {
        std::lock_guard<std::mutex> lock(rhd.m);
        this->m = rhd.m;
    }
    Thread_safe_stack& operator=(const Thread_safe_stack&) = delete;
    Thread_safe_stack(const Thread_safe_stack<T>&& rhd) = delete;
    Thread_safe_stack& operator=(const Thread_safe_stack&&) = delete;
    bool empty() const
    {
        std::lock_guard<std::mutex> lock(this->m);
        return data.empty();
    }
    void push(T& value)
    {
        std::lock_guard<std::mutex> lock(this->m);
        data.push(value);
    }
    std::shared_ptr<T> pop()
    {
        std::lock_guard<std::mutex> lock(this->m);
        if(data.empty()) throw empty_stack();
        std::shared_ptr<T> const res(std::make_shared<T>(data.top()));
        data.pop();
        return res;
    }
    void pop(T& top)
    {
        std::lock_guard<std::mutex> lock(this->m);
        if(data.empty()) throw empty_stack();

        top = data.top();
        data.pop();
    }
};

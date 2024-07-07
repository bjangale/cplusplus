
#include <iostream>
#include <thread>
#include <mutex>
#include <stack>
#include <exception>
#include <memory>

struct EmptyStack : std::exception
{
    const char *what()
    {
        return "Stack is empty";
    }
};

template <typename T>
class ThreadSafeStack
{
    std::stack<T> stack;
    std::mutex mtx;

public:
    ThreadSafeStack() = default;
    ThreadSafeStack(const ThreadSafeStack &other)
    {
        std::lock_guard<std::mutex> gaurd(other.mtx);
        this->stack = other.stack;
    }

    void push(const T &value)
    {
        std::lock_guard<std::mutex> gaurd(mtx);
        stack.push(value);
    }

    std::shared_ptr<T> pop()
    {
        std::lock_guard<std::mutex> gaurd(mtx);
        if (stack.empty())
        {
            throw EmptyStack();
        }
        auto ptr = std::make_shared(stack.top());
        stack.pop();
        return ptr;
    }

    void pop(T &value)
    {
        std::lock_guard<std::mutex> gaurd(mtx);
        if (stack.empty())
        {
            throw EmptyStack();
        }
        value = stack.top();
        stack.pop();
    }

    bool empty()
    {
        std::lock_guard<std::mutex> gaurd(mtx);
        return stack.empty();
    }
};

int main()
{
    ThreadSafeStack<int> obj;
}

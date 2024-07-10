
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
        auto ptr = std::make_shared<T>(stack.top());
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

void TestThreadSafeStackTask(int id, ThreadSafeStack<int> &obj)
{
    for (int i = 0; i < 3; i++)
    {
        std::cout << "Thread Id : " << id
                  << " Poped Value : " << *obj.pop()
                  << std::endl;
    
        std::this_thread::yield();
    }
}

int main()
{
    ThreadSafeStack<int> obj;
    obj.push(1);
    obj.push(2);
    obj.push(3);
    obj.push(4);
    obj.push(5);
    obj.push(6);

    std::thread t1(TestThreadSafeStackTask, 1, std::ref(obj));
    std::thread t2(TestThreadSafeStackTask, 2, std::ref(obj));

    t1.join();
    t2.join();
}

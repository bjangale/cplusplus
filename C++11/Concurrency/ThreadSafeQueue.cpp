

#include <iostream>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>

template <typename T>
class ThreadSafeQueue
{
private:
    std::queue<T> m_queue;
    mutable std::mutex m_mtx;
    std::condition_variable m_condVar;

public:
    /** */
    ThreadSafeQueue() = default;
    /** */
    ThreadSafeQueue(ThreadSafeQueue const &other);
    /** */
    void push(T newValue);
    /** */
    void waitAndPop(T &value);
    /** */
    std::shared_ptr<T> waitAndPop();
    /** */
    bool try_pop(T &value);
    /** */
    std::shared_ptr<T> try_pop();
    /** */
    bool empty() const;
};

template <typename T>
ThreadSafeQueue<T>::ThreadSafeQueue(ThreadSafeQueue const &other)
{
    std::lock_guard<std::mutex> lock(other.m_mtx);
    this->m_queue = other.m_queue;
}

template <typename T>
void ThreadSafeQueue<T>::push(T newValue)
{
    std::lock_guard<std::mutex> lock(m_mtx);
    m_queue.push(newValue);
    m_condVar.notify_one();
}

template <typename T>
void ThreadSafeQueue<T>::waitAndPop(T &value)
{
    std::unique_lock<std::mutex> lock(m_mtx);
    m_condVar.wait(lock, [this]
                   { return !m_queue.empty(); });
    value = m_queue.front();
    m_queue.pop();
}

template <typename T>
std::shared_ptr<T> ThreadSafeQueue<T>::waitAndPop()
{
    std::unique_lock<std::mutex> lock(m_mtx);
    m_condVar.wait(lock, [this]
                   { return !m_queue.empty(); });
    auto result = std::make_shared<T>(m_queue.front());
    m_queue.pop();
    return result;
}

template <typename T>
bool ThreadSafeQueue<T>::try_pop(T &value)
{
    std::lock_guard<std::mutex> lock(m_mtx);
    if (!m_queue.empty())
        return false;

    value = m_queue.front();
    m_queue.pop();
    return true;
}

template <typename T>
std::shared_ptr<T> ThreadSafeQueue<T>::try_pop()
{
    std::lock_guard<std::mutex> lock(m_mtx);
    if (!m_queue.empty())
        return std::make_shared<T>();

    auto result = std::make_shared<T>(m_queue.front());
    m_queue.pop();
    return result;
}

template <typename T>
bool ThreadSafeQueue<T>::empty() const
{
    std::lock_guard<std::mutex> lock(m_mtx);
    return m_queue.empty();
}

void WriterThread(ThreadSafeQueue<int> &queue)
{
    for (int i = 1; i <= 40; i++)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        queue.push(i);
    }
}

void ReaderThread(ThreadSafeQueue<int> &queue)
{
    for (int i = 1; i <= 40; i++)
    {
        std::cout << "Pop value : " << *queue.waitAndPop() << std::endl;
    }
}

int main()
{
    ThreadSafeQueue<int> ts_Queue;

    std::thread t1(WriterThread, std::ref(ts_Queue));
    std::thread t2(ReaderThread, std::ref(ts_Queue));
    t1.join();
    t2.join();

    ts_Queue.push(10);
    ts_Queue.push(5);
    std::cout << "Empty : " << std::boolalpha << ts_Queue.empty() << std::endl;
    int i;
    ts_Queue.waitAndPop(i);
    std::cout << i << std::endl;
    std::cout << *ts_Queue.waitAndPop() << std::endl;
    std::cout << "Empty : " << std::boolalpha << ts_Queue.empty() << std::endl;
}
#include <iostream>
#include <atomic>
#include <thread>
#include <chrono>
#include <vector>

class SpinLock
{
    std::atomic_flag flag;

public:
    SpinLock() : flag(ATOMIC_FLAG_INIT)
    {
    }

    void lock()
    {
        std::cout << "Waiting for lock thread = " << std::this_thread::get_id() << std::endl;
        while (flag.test_and_set(std::memory_order_acquire))
            ;
        std::cout << "Locked by thread = " << std::this_thread::get_id() << std::endl;
    }

    void unlock()
    {
        std::cout << "Unlocked thread = " << std::this_thread::get_id() << std::endl;
        flag.clear(std::memory_order_release);
    }
};

int counter = 0;

void ThreadSafeCounter(SpinLock &sLock)
{
    for (int i = 0; i < 1000; i++)
    {
        sLock.lock();
        counter++;
        std::this_thread::sleep_for(std::chrono::seconds(2));
        sLock.unlock();
        std::this_thread::yield();
    }
}

int main()
{
    SpinLock sLock;
    std::vector<std::thread> vec;

    for (int i = 0; i < 10; i++)
    {
        vec.emplace_back(std::thread(ThreadSafeCounter,std::ref(sLock)));
    }

    for(auto& t : vec)
        t.join();

    std::cout<<"Counter = "<<counter<<std::endl;
}

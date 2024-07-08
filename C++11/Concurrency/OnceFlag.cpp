/**
 * The std::call_once function in C++ is used to ensure that a particular piece of
 * code is executed only once, even if multiple threads attempt to call it concurrently.
 * It is often used for initializing resources that are shared across threads.
 *
 * Components:
 *  std::once_flag: A flag used to indicate whether the function has been called.
 *  std::call_once: Ensures the function is called only once
 */

#include <iostream>
#include <mutex>
#include <thread>

/**
 * Basic implementation using once_flag for shared resorce initialization
 */
std::once_flag flag; // Declare a once_flag

void initializeSharedResource()
{
    std::cout << "Shared resource initialized." << std::endl;
}

void threadTask(int id)
{
    std::call_once(flag, initializeSharedResource); // Ensure the initialization function is called only once
    std::cout << "Thread " << id << " is running." << std::endl;
}

void TestOnceFlag()
{
    std::thread t1(threadTask, 1);
    std::thread t2(threadTask, 2);
    std::thread t3(threadTask, 3);

    t1.join();
    t2.join();
    t3.join();
}
/**
 * Singleton Pattern using once_flag
 */
class Singleton
{
public:
    // Deleted copy constructor and assignment operator to prevent copying
    Singleton(const Singleton &) = delete;
    Singleton &operator=(const Singleton &) = delete;

    // Static method to get the instance of the Singleton class
    static Singleton &getInstance()
    {
        std::call_once(initFlag, []()
                       { instance = new Singleton(); });
        return *instance;
    }

    // Public method to demonstrate functionality
    void showMessage() const
    {
        std::cout << "Hello from Singleton!" << std::endl;
        std::cout << "My address is " << std::hex << this << std::endl;
    }

private:
    // Private constructor to prevent instantiation
    Singleton()
    {
        std::cout << "Singleton instance created." << std::endl;
    }

    // Pointer to the singleton instance
    static Singleton *instance;

    // Once flag to ensure the instance is created only once
    static std::once_flag initFlag;
};

// Initialize the static members
Singleton *Singleton::instance = nullptr;
std::once_flag Singleton::initFlag;

void SingletonMultiThread()
{
    Singleton &s1 = Singleton::getInstance();
    s1.showMessage();
}

void TestOnceFlagForSingleton()
{
    std::thread t1(SingletonMultiThread);
    std::thread t2(SingletonMultiThread);

    t1.join();
    t2.join();
}

int main()
{
    TestOnceFlag();
    TestOnceFlagForSingleton();
}
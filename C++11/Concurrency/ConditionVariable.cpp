/**
 * A condition variable in C++ is a synchronization primitive that allows threads
 * to wait until a certain condition is met. Condition variables are used in
 * conjunction with a mutex to facilitate communication between threads. They are
 * part of the <condition_variable> header in the C++ Standard Library.
 *
 * Key Components
 *     std::condition_variable: The condition variable type for use with std::unique_lock<std::mutex>.
 *     std::condition_variable_any: A more general condition variable type that can work with any lock
 *     type that meets the BasicLockable requirements.
 *     wait() / wait_for() / wait_until(): Methods for waiting until notified.
 *     notify_one() / notify_all(): Methods for waking up waiting threads.
 *
 * Example demonstrating the use of std::condition_variable
 */
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <chrono>

std::mutex mtx;
std::condition_variable cv;
std::queue<int> dataQueue;
// Indicates when the producer is done
bool done = false; 

/**
 * Producer function
 * Generates data and write to queue while holding the lock.
 * Notify to the consumer thread once data is pushed to queue.
 */
void producer()
{
    for (int i = 0; i < 10; ++i)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        std::lock_guard<std::mutex> lock(mtx);
        dataQueue.push(i);
        std::cout << "Produced: " << i << std::endl;
        // Notify one waiting thread
        cv.notify_one();
    }

    // Indicate that production is done
    {
        std::lock_guard<std::mutex> lock(mtx);
        done = true;
    }
    cv.notify_all(); // Notify all waiting threads
}

/**
 * Consumer function
 * Consumer waits on condition variable until data is available.
 */
void consumer()
{
    while (true)
    {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, []
                { return !dataQueue.empty() || done; });
        // Read all packets available in queue
        while (!dataQueue.empty())
        {
            int data = dataQueue.front();
            dataQueue.pop();
            std::cout << "Consumed: " << data << std::endl;
        }

        if (done && dataQueue.empty())
        {
            // Exit the loop if done and no more data to process
            break;
        }
    }
}

int main()
{
    std::thread producerThread(producer);
    std::thread consumerThread(consumer);

    producerThread.join();
    consumerThread.join();

    return 0;
}

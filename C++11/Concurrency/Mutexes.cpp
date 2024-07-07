/**
 *
 * Mutexes : Mutexes are used to prevent data races by ensuring that only one thread can access critical
 *         section of code at a time.
 * 1. std::mutex mtx;
 *    mtx.lock()      - locks the mutex if available.if locked calling thread owns the mutex until it
 *                      calls unlock()
 *    mtx.unlock()    - unlocks the mutex.
 *    mtx.try_lock()  - try to locks the mutex, return true if locked successfully false otherwise
 *    mtx.native_handle() - return platform-specific native handle.
 *
 * 2. std::timed_mutex tmtx;
 *   tmtx.lock()     - locks the mutex, blocks if the mutex is not available
 *   tmtx.try_lock() - tries to lock the mutex, returns if the mutex is not available
 *   tmtx.try_lock_for() - tries to lock the mutex, returns if the mutex has been
 *                        unavailable for the specified timeout duration
 *   tmtx.try_lock_until() - tries to lock the mutex, returns if the mutex has been
 *                           unavailable until specified time point has been reached
 *   tmtx.unlock()       - unlocks the mutex
 *
 * 3. std::recursive_mutex - allows same thread to lock the mutex multiple times.
 *
 * 4. std::lock_guard<std::mutex> guard(mtx)
 *                   - RAII style mechanism for owning a mutex for duration of scoped block.
 *                     provides convinient way to lock and unlock mutex automatically.

 *
 * 7. std::unique_lock<std::mutex>
 *                   - unique_lock provides more flexlibility such as differed locking,
 *                     time locking, manual unlocking.
 *   lock
 *  	locks (i.e., takes ownership of) the associated mutex
 *
 *   try_lock
 *	    tries to lock (i.e., takes ownership of) the associated mutex without blocking
 *
 *   try_lock_for
 *	    attempts to lock (i.e., takes ownership of) the associated TimedLockable mutex, returns
 *	    if the mutex has been unavailable for the specified time duration
 *
 *   try_lock_until
 *	    tries to lock (i.e., takes ownership of) the associated TimedLockable mutex, returns if
 *	    the mutex has been unavailable until specified time point has been reached
 *
 *   unlock
 *	    unlocks (i.e., releases ownership of) the associated mutex
 *
 * Modifiers:
 *   swap
 *	    swaps state with another std::unique_lock
 *   release
 *	    disassociates the associated mutex without unlocking (i.e., releasing ownership of) it
 *   Observers:
 *   mutex
 *	    returns a pointer to the associated mutex
 *   owns_lock
 *	    tests whether the lock owns (i.e., has locked) its associated mutex
 *
 */

#include <iostream>
#include <thread>
#include <mutex>
#include <stdexcept>

/**
 * std::defer_lock is a tag type used with std::unique_lock and std::lock_guard to
 * indicate that the associated mutex should not be locked on construction. Instead,
 * the lock can be acquired later, manually. This is useful in scenarios where you
 * need more control over the timing of locking and unlocking, such as when you want
 * to avoid holding a lock for the entire duration of the object’s lifetime or when
 * you want to lock multiple mutexes together to avoid deadlock.
 */
std::mutex condTaskMtx;

void conditionalLockTask(bool lockRequired)
{
    std::unique_lock<std::mutex> lock(condTaskMtx, std::defer_lock);

    if (lockRequired)
    {
        lock.lock(); //Conditionally lock the mutex
        std::cout << "Mutex locked by thread " << std::this_thread::get_id() << std::endl;
    }
    else
    {
        std::cout << "Mutex not locked by thread " << std::this_thread::get_id() << std::endl;
    }

    //Perform work here
    std::cout<<"Thread "<<std::this_thread::get_id()<<" is doing work"<<std::endl;

    //mutex will be unlocked if locked when "lock" goes out of scope
}

void TestMutexDeferLock()
{
    std::thread t1(conditionalLockTask,true);
    std::thread t2(conditionalLockTask,false);

    t1.join();
    t2.join();
}

/**
 * std::adopt_lock is a tag used with std::lock_guard and std::unique_lock to indicate 
 * that the calling thread already owns the mutex and that the lock should adopt the 
 * ownership without attempting to acquire it again. This is useful in situations where 
 * the mutex has been locked manually or by another mechanism, and you want to ensure 
 * that it gets properly unlocked when the lock object goes out of scope.
 */
std::mutex mtx;

void taskAdoptLock() {
    mtx.lock(); // Manually lock the mutex

    // Use std::lock_guard with std::adopt_lock to take ownership of the mutex
    std::lock_guard<std::mutex> lock(mtx, std::adopt_lock);
    
    // Critical section
    std::cout << "Task is running with mutex locked" << std::endl;

    // The mutex will be unlocked automatically when 'lock' goes out of scope
}
void TestMutexAdoptLock()
{
    std::thread t1(taskAdoptLock);
    std::thread t2(taskAdoptLock);

    t1.join();
    t2.join();
}

/**
 * std::lock(MutexTypes&... mtxs)
 *     - function in C++ is used to lock multiple mutexes simultaneously without
 *       causing deadlock. uses deadlock avoidance algorithm. It ensures that all
 *       mutexes are locked or none are locked. If it cannot lock all mutexes, it
 *       will unlock any mutexes it has already locked and try again.
 * std::try_lock(MutexTypes&... mtxs)
 *     - attempts to lock multiple mutexes simultaneously without causing deadlock.
 */
std::mutex mtx1, mtx2;

void task1()
{
    try
    {
        // Lock both mutexes without causing deadlock
        std::lock(mtx1, mtx2);

        // Ensure mutexes are unlocked when the scope ends
        std::lock_guard<std::mutex> lock1(mtx1, std::adopt_lock);
        std::lock_guard<std::mutex> lock2(mtx2, std::adopt_lock);

        std::cout << "Task 1 has locked both mutexes" << std::endl;
        // Simulate some work
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    catch (const std::exception &e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
}

void task2()
{
    try
    {
        // Ensure mutexes are unlocked when the scope ends if locked.
        std::unique_lock<std::mutex> lock1(mtx1, std::defer_lock);
        std::unique_lock<std::mutex> lock2(mtx2, std::defer_lock);
        // Do some non-critical work here
        std::cout << "Task2: Doing some initial work before locking" << std::endl;

        // Lock both mutexes without causing deadlock
        // use std::unique_lock instace here for locking
        std::lock(lock1, lock2);
        std::cout << "Task 2 has locked both mutexes" << std::endl;
        // Simulate some work
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    catch (const std::exception &e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
}

void TestMutexStdLock()
{
    std::thread t1(task1);
    std::thread t2(task2);
    std::thread t3(task1);
    std::thread t4(task2);

    t1.join();
    t2.join();
    t3.join();
    t4.join();
}

int main()
{
    
TestMutexDeferLock();
TestMutexAdoptLock();
TestMutexStdLock();

    return 0;
}

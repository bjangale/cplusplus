/**
 *In C++, thread can be created with using callable types like function pointer, lambda,
 *fuction object, class member function. In this file we will see examples with all
 *callable types.
 *
 *Thread is launched when std::thread object is created, it ins need to be decided wheather
 *to wait for it to finish by calling join() or or leave it run to run on its own in background
 *by calling detach(). if detach() or join() is not called before std::thread is destoyed then
 *program is terminated, std::thread desctuctor calls std::terminate()
 *
 * std::terminate() defalt terminate handler is abort, behaviour can be redefined by calling
 * std::set_terminate(myTerminate)
 *
 * std::thread::hardware_concurrency() functions returns an indication of number threads that
 * can truely run concurrently for given execution program.
 *
 * std::this_thread::yield(); provides hints to implementation to reschedule/suspend execution
 * of current thread, allowing other threads to run. The exact behaviour is implementation specific.
 *
 * The function std::this_thread::get_id() return id of the thread type of thread id is std::thread::id
 *
 * std::this_thread::sleep_for(std::chrono::microseconds(10)) blocks execution of thread for at lest
 * specified sleep_duration
 * std::this_thread::sleep_until(sleep_time) blocks the execution of current thread until specified
 * sleep_time
 *
 * native_handle_type native_handle(); return underlying platfrom-specific thread handle.
 * Window returns 'HANDLE', POSIX returns 'pthread_t'
 *
 */

#include <iostream>
#include <thread>
#include <stdexcept>
#include <utility>
#include <vector>
#include <algorithm>
#include <functional>
#include <utility>
#include <pthread.h>

/**Thread using function pointer. */
void FunctionPointerTask(int x, int y)
{
    std::cout << __FUNCTION__ << " x = " << x << " y = " << y << std::endl;

    std::thread::id myThreadId = std::this_thread::get_id();
    std::cout << __FUNCTION__ << " Current threads thread id : " << myThreadId << std::endl;
}

void ThreadUsingFuctionPointer()
{
    std::thread t(FunctionPointerTask, 10, 20);

    t.join();
}

/**Thread using lambda */
void ThreadUsingLambdaFunction()
{
    int k = 15;
    auto lambdaTask = [](int k)
    {
        std::cout << __FUNCTION__ << " Lambda task: k = " << k << std::endl;
    };

    // new uniform initialization syntax with braces
    std::thread t1{lambdaTask, k};

    // Inplace lambda
    std::thread t2([](int k)
                   { std::cout << "Inplace lambda: k = " << k << std::endl; },
                   k);

    t1.join();
    t2.join();
}

/** Thread using fuction object */
struct FunctionOjectTask
{
    void operator()(std::string str) const
    {
        std::cout << "FunctionOjectTask:  " << str << std::endl;
    }
};

void ThreadUsingFunctionObject()
{
    FunctionOjectTask funcObj;

    std::thread t1(funcObj, "Using object");

    // create thread with function object temporary;
    std::thread t2{FunctionOjectTask(), "Using temporary"};

    t1.join();
    t2.join();
}

/** Thread using member function */
class MemberFunctionTask
{
    int k;

public:
    MemberFunctionTask(int k_) : k(k_){};

    void memberFuctionPlus(int n)
    {
        auto result = k + n;
        std::cout << "MemberFunctionTask : k + n = " << result << std::endl;
    }

    static void StaticMemberFunction(int n)
    {
        std::cout << "StaticMemberFunction : " << n << std::endl;
    }
};

void ThreadUsingMemberFunction()
{
    MemberFunctionTask memFuncTaskObj(10);

    // Pass address of Member funciton, address of object, arguments
    std::thread t1(&MemberFunctionTask::memberFuctionPlus, &memFuncTaskObj, 5);

    // create thead with static member function
    std::thread t2(&MemberFunctionTask::StaticMemberFunction, 20);
    t1.join();
    t2.join();
}
/**
 * lauch thread in background by calling datach
 * sometimes you may need perform one task in background another in main thread.
 * In this case you can lauch the thread in backgroud by calling detach()
 *
 * detach() breaks the associate of thread with std::thread object and ensure
 * that std::terminate() won't be called when the std::thread object is destroyed,
 * even though thread is running in backgroud.
 */
void backgroundTask(int n)
{
    std::cout << __FUNCTION__ << " called in background : " << n << std::endl;
}

void ThreadRunInBackground()
{
    std::thread t(backgroundTask, 2);
    t.detach();
    std::this_thread::sleep_for(std::chrono::microseconds(10));
}

/**
 * join in exception case - if it inteded to call join in no exeception cases
 * you also nedd to call join() in the presence of execption to avoid accidental
 * lifetime problem.
 * Ex.
 * std::thread t(my_func);
 * try
 * {
 *      do_something_in_current_thread();
 * }
 * catch (...)
 * {
 *      t.join();
 *      throw;
 * }
 * t.join();
 *
 * This issue can be avoid by using standard RAII and provide class that does join()
 * in desctructor.
 */
class thread_gaurd
{
    std::thread &t;

public:
    explicit thread_gaurd(std::thread &t_) : t(t_) {}

    ~thread_gaurd()
    {
        // check thread is joinable or not before calling join
        // because join can be called only once
        if (t.joinable())
        {
            t.join();
        }
    }
    thread_gaurd(thread_gaurd &) = delete;
    thread_gaurd &operator=(thread_gaurd &) = delete;
};

void TestThreadGuard(const std::string &str)
{
    std::thread t1(FunctionPointerTask, 2, 3);
    thread_gaurd guard(t1);

    try
    {
        int num = std::stoi(str, nullptr);
        std::cout << num << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cout << __FUNCTION__ << " Exception!!" << std::endl;
    }
}

void ThreadUsingThreadGaurdRAII()
{
    // Test success case
    TestThreadGuard("1234");
    // Test failure case exception will be thrown
    TestThreadGuard("a43");
}

/** Passing argurment to thread function by reference  */
void TestThreadArgByRef(int &k)
{
    k += 10;
    std::cout << __FUNCTION__ << " Test : " << k << std::endl;
}

void ThreadWithPaasbyReference()
{

    int k = 10;
    // arguments by reference to thread fucntion needs to
    // be wrapped using std::ref()`
    std::thread t1(TestThreadArgByRef, std::ref(k));
    t1.join();
    std::cout << __FUNCTION__ << " Test : k = " << k << std::endl;
}

/** Passing argurment to thread by r value reference, Ex.
 * unique_ptr is obj can't be copied but can be only moved.
 * The data within one object is transferred over to another,
 * leaving the orignal object empty.
 */
class BigObject
{
public:
    void execute()
    {
        std::cout << "Processing big object" << std::endl;
    }
};
void ProcessBigObject(std::unique_ptr<BigObject> obj)
{
    obj->execute();
}

void ThreadWithPassByRValueRef()
{
    std::unique_ptr<BigObject> bigObj;
    std::thread t1{ProcessBigObject, std::move(bigObj)};
    t1.join();
}

/** Transfering ownershi of thread. Thread object is movalbe not copyable
 * Ownership of a particular thread of execution can be moved between thread
 * instances.
 */
void DoSomeOperatoion()
{
    // std::this_thread::sleep_for(std::chrono::milliseconds(5));
    std::this_thread::yield();
    std::cout << "Do some operation" << std::endl;
}
void DoSomeOtherOperation()
{
    std::cout << "Do some other operation" << std::endl;
}

void ThreadTrasnferOfOwnership()
{
    std::thread t1(DoSomeOperatoion);
    // Ownership of t1 is transfered to thread t2
    // t1 is no longer associted with thread of execution
    std::thread t2 = std::move(t1);
    t1 = std::thread(DoSomeOtherOperation);
    // t3 is default constructed, it created without
    // any associated thread of execution
    std::thread t3;
    t3 = std::move(t2);
    // Here t1 already has thread associated with it.
    // This assignment will terminate the program
    // t1 = std::move(t3);

    t1.join();
    t3.join();
}

/** */
class scoped_thread
{
    std::thread t;

public:
    explicit scoped_thread(std::thread &t_) : t(std::move(t))
    {
        if (!t.joinable())
        {
            throw std::logic_error("Thread is not joinable");
        }
    }
    ~scoped_thread()
    {
        t.join();
    }

    scoped_thread(const scoped_thread &) = delete;
    scoped_thread &operator=(const scoped_thread &) = delete;
};

void TestScopedThread()
{
    std::cout << "Testing scoped_thread" << std::endl;
    std::this_thread::sleep_for(std::chrono::microseconds(50));
}

void ThreadScopedTransferOwnership()
{
    // scoped_thread t(std::thread{TestScopedThread});
}

/**
 * Spawn some thread and wait for them to finish
 * choose number of threads at runtime
 * std::thread::hardware_concurrency() functions returns an indication
 * of number threads that ca truely run concurrently for given execution
 * program.
 */
void TestSpawnThread(int n)
{
    std::cout << "Spawn thread no: " << n << std::endl;
}

void SpawnNumOfThreadsRuntimeAndWait()
{
    unsigned long int maxThreds = 5;
    const unsigned long int hardwareThread = std::thread::hardware_concurrency();
    std::vector<std::thread> threads;

    int numThread = std::min(hardwareThread, maxThreds);
    std::cout << "Number of hardware thread : " << hardwareThread
              << " Max threads to run : " << maxThreds << std::endl;

    for (int i = 1; i <= numThread; i++)
    {
        // threads.push_back(std::thread(TestSpawnThread, i));
        threads.emplace_back(TestSpawnThread, i);
    }

    std::for_each(threads.begin(), threads.end(),
                  std::mem_fn(&std::thread::join));

    // for (auto &t : threads){t.join();}
}

/**
 * native_handle_type native_handle(); return underlying platfrom-specific thread handle.
 * Window returns 'HANDLE', POSIX returns 'pthread_t'
 */
void TestNativeHandle()
{
    std::cout << "Testing native handle" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));
}

void ThreadNativeHandle()
{
    std::thread t1{TestNativeHandle};
    // std::thread::native_handle_type
    //     nativeHandle = t1.native_handle();
    pthread_t nativeHandle = t1.native_handle();

    // set thread scheduling policy to round robin
    sched_param schParam;
    schParam.sched_priority = 20;
    if (pthread_setschedparam(nativeHandle, SCHED_RR, &schParam) == 0)
    {
        std::cout << "Thread scheduling policy set to round robin" << std::endl;
    }
    else
    {
        std::cout << "Failed to set thread scheduling policy" << std::endl;
    }
    t1.join();
}

/** main function */
int main()
{
    ThreadUsingFuctionPointer();
    ThreadUsingLambdaFunction();
    ThreadUsingFunctionObject();
    ThreadUsingMemberFunction();
    ThreadRunInBackground();
    ThreadUsingThreadGaurdRAII();
    ThreadWithPaasbyReference();
    ThreadWithPassByRValueRef();
    ThreadTrasnferOfOwnership();
    ThreadScopedTransferOwnership();
    SpawnNumOfThreadsRuntimeAndWait();
    ThreadNativeHandle();
}

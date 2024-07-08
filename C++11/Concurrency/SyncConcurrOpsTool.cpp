/**
 * In C++, the standard library provides powerful tools for managing
 * asynchronous operations: std::future, std::promise, std::async, and
 * std::packaged_task. These tools help in writing concurrent and
 * parallel programs.
 *
 * std::promise and std::future: Use when you need to manually set a value
 * or an exception in the promise that can be retrieved by the future.
 * std::async: Use for a higher-level, simpler interface to run a function
 * asynchronously and get its result.
 * std::packaged_task: Use when you want to wrap a callable object and
 * manage its result with a future.
 *
 */
#include <iostream>
#include <future>
#include <thread>

/**
 * std::future and std::promise
 * std::future: An object that can retrieve the result of an asynchronous operation.
 * std::promise: An object that stores a value or an exception that can be retrieved by a std::future.
 */

void AsyncPromiseTask(std::promise<int> &&promiseObj)
{
    std::cout << "Async Promise task" << std::endl;
    // simulate a long computation
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    // result of computation
    int result = 56;
    // set the result in the promise
    promiseObj.set_value(result);
}

void TestFutureAndPromise()
{
    std::promise<int> promiseObj;
    // Get the future associated with promise
    std::future<int> futureObj = promiseObj.get_future();

    // Start a thread to perform the asynchronus task
    std::thread t1(AsyncPromiseTask, std::move(promiseObj));

    std::cout << "Waiting for the result..." << std::endl;
    // Get the result from the future
    int result = futureObj.get();
    std::cout <<__FUNCTION__<< " The Result is : " << result << std::endl;

    t1.join();
}

/**
 * std::async
 * std::async is a higher-level abstraction that runs a function asynchronously and
 * returns a std::future to retrieve the result. std::thread does not provide any
 * direct mechanism to trasfer back the result.
 * 
 * std::launch::async: The function is run asynchronously (in a new thread).
 * std::launch::deferred: The function is run when the result is needed (lazy evaluation).
 * std::launch::async | std::launch::deferred: The implementation can choose either policy.
 *
 * auto f6=std::async(std::launch::async,Y(),1.2); <- run in new thread
 * auto f7=std::async(std::launch::deferred,baz,std::ref(x)); <- run in wait() or get()
 * std::async(std::launch::async, []{ f(); }); // temporary's dtor waits for f()
 */
// Function to perform some asynchronous task
int AsyncTask()
{
    // Simulate long computation and return the result
    return 35;
}

void TestStdAsyncTask()
{
    // Launch task asynchronously
    // std::future<int> futureObj = std::async(AsyncTask);
    std::future<int> futureObj = std::async(AsyncTask);
    // Get the result from the future
    int result = futureObj.get();
    std::cout <<__FUNCTION__<<" The result is : "<< result << std::endl;
}

/**
 * std::packaged_task wraps a callable object (function, lambda, etc.) and allows its
 * result to be retrieved via a std::future.
 */
int AsyncPackagedTask()
{
    // Simulate long computation and return the result
    return 22;
}
void TestPackagedTask()
{
    //Wraps the AsyncPackagedTask in package_task
    std::packaged_task<int()> task(AsyncPackagedTask);
    //Get the futur associated with task
    std::future<int> futurObj = task.get_future();
    //start a thread to run the task
    std::thread t1(std::move(task));

    //Get the result from the future
    int result = futurObj.get();
    std::cout<<__FUNCTION__<<" The result is : "<<result<<std::endl;

    t1.join();
}

int main()
{
    TestFutureAndPromise();
    TestStdAsyncTask();
    TestPackagedTask();
}
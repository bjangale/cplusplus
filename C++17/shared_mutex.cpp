#include <iostream>
#include <thread>
#include <mutex>
#include <shared_mutex>

std::shared_mutex smtx;
int sharedResource = 0;

void readResource(int id) {
    std::shared_lock<std::shared_mutex> lock(smtx); // Shared lock
    std::cout << "Thread " << id << " reads: " << sharedResource << std::endl;
}

void writeResource(int id, int value) {
    std::unique_lock<std::shared_mutex> lock(smtx); // Exclusive lock
    sharedResource = value;
    std::cout << "Thread " << id << " writes: " << sharedResource << std::endl;
}

int main() {
    std::thread t1(readResource, 1);
    std::thread t2(writeResource, 2, 42);
    std::thread t3(readResource, 3);

    t1.join();
    t2.join();
    t3.join();

    return 0;
}

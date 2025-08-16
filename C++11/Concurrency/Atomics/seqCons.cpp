#include <iostream>
#include <atomic>
#include <thread>

std::atomic<int> a{0};
std::atomic<int> b{0};
std::atomic<int> result{0};

void thread1() {
    a.store(1, std::memory_order_seq_cst);  // Write 1 to a
    int x = b.load(std::memory_order_seq_cst); // Read value of b
    std::cout << "Thread 1: b = " << x << std::endl;
}

void thread2() {
    b.store(1, std::memory_order_seq_cst);  // Write 1 to b
    int y = a.load(std::memory_order_seq_cst); // Read value of a
    std::cout << "Thread 2: a = " << y << std::endl;
}

int main() {
    std::thread t1(thread1);
    std::thread t2(thread2);

    t1.join();
    t2.join();
}


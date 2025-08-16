#include <iostream>
#include <thread>
#include <atomic>
#include <mutex>

//#define AT

#ifndef AT
std::atomic_int var(0);
#endif

#ifdef AT
int var;
std::mutex mt;
#endif

void function()
{
    for(int i = 0; i < 1000000; i++){
        #ifdef AT
        std::lock_guard<std::mutex> mx(mt);
        #endif
        var++;
        std::this_thread::yield();
    }
}


int main(){
    std::thread t1(function);
    std::thread t2(function);
   

    t1.join();
    t2.join();
 std::cout<<"var = "<<var<<std::endl;
}
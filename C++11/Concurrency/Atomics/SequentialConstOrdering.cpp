#include <iostream>
#include <thread>
#include <atomic>
#include <assert.h>


std::atomic_int z;
std::atomic_bool x,y;


void write_x()
{
    x.store(true,std::memory_order_seq_cst); //1
}

void write_y()
{
    y.store(true,std::memory_order_seq_cst); //2
}

void read_x_then_y()
{
    while(!x.load(std::memory_order_seq_cst));
    if(y.load(std::memory_order_seq_cst))    //3
        z++;
    //    z.fetch_add(1, std::memory_order_relaxed);
}

void read_y_then_x()
{
    while(!y.load(std::memory_order_seq_cst));
    if(x.load(std::memory_order_seq_cst))    //4
        z++;
}

int main()
{
    x=false;
    y=false;
    z=0;
    std::thread t1(write_x);
    std::thread t2(write_y);
    std::thread t3(read_x_then_y);
    std::thread t4(read_y_then_x);

    t1.join();
    t2.join();
    t3.join();
    t4.join();

    int tmp = z.load();
    std::cout<<"z = "<<tmp<<std::endl;      //5
    assert(tmp != 0);
}
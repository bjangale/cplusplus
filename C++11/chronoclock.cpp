#include <iostream>
#include <vector>
#include <chrono>
#include <assert.h>
/* Introductino to chrono
* -- A prescision-nutral library for time and date
*
* clock:
* std::chrono::system_clock: current time according to the system - is not steady
* std::chrono::steady_clock: goes at a unifrom rate
* std::chrono::high_resolution_clock: provide smallest possible tick
*
* std::chrono::duration<>: represents time duration
* 2 hours ( a number and a unite)
*
*
* duration<int, ratio<1,1>> //number of seconds stored in int
* duration<double, ratio<60,1>> //number of minits stored in double
* nanoseconds, microseconds, milliseconds, seconds, minutes, hours
* system_clock::duration -- duration<T, system_clock::period>
*
* std::chrono::time_point<> : represent a point in time
* 00:00 January 1, 1970 (co-ordinated )
*
* time_pint<sytem_clock, milliseconds> : according to system_clock, the elasped time since
the epoch in milliseconds.
*
* system_clock::time_point -- time_point<system_clock, system_clock::duration>
* system_clock::time_point -- time_point<steady_clock, sytem_clock::duration>
*/
int main()
{
std::ratio<2,10> r; //1/10
std::cout<<"r.num/r.den = "<<r.num<<"/"<<r.den<<std::endl;
std::chrono::milliseconds mi(2700);
std::cout<<"milliseconds : "<<mi.count()<<std::endl; //2700
std::chrono::nanoseconds ni=mi;
std::cout<<"nanoseconds : "<<ni.count()<<std::endl; //2700000
std::chrono::milliseconds mill = std::chrono::duration_cast<std::chrono::milliseconds>(mi);
//2700 milliseconds
mi = mill + mi; // 2700 + 2700 = 5400
std::cout<<"milliseconds : "<<mi.count()<<std::endl;
std::chrono::system_clock::time_point tp = std::chrono::system_clock::now();
std::cout<< tp.time_since_epoch().count()<<std::endl;
tp = tp + std::chrono::seconds(2);
std::cout<< tp.time_since_epoch().count()<<std::endl;
//Steady clock
std::chrono::steady_clock::time_point tpSteady = std::chrono::steady_clock::now();
std::cout<< tpSteady.time_since_epoch().count()<<std::endl;
tpSteady = tpSteady + std::chrono::seconds(2);
std::cout<< tpSteady.time_since_epoch().count()<<std::endl;
//Duration
std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
std::cout<<"Test chrono time"<<std::endl;
std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
std::chrono::steady_clock::duration d = end - start;
/*
if(d == std::chrono::steady_clock::duration::zero)
{
std::cout<<"No time elapsed"<<std::endl;
}
*/
std::cout<<"Elapsed duration : "<<std::chrono::duration_cast<std::chrono::microseconds>(d).
count()<<std::endl;
//auto start = std::chrono::high_resolution_clock::now();
//std::cout<<start<<std::endl;
// std::cout<<std::chrono::duration<int,std::chrono::nanoseconds>(start).count()<<std::endl;
return true;
}
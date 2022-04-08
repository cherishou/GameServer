
#include "pch.h"
#include <iostream>

#include <thread>
#include <atomic>
#include <mutex>
using namespace std;

class SpinLock
{
public:
    void lock()
    {

        //CAS(Compare-And-Swap)

        bool expected = false; // 락을 걸기를 원하는 대상의 현재상태 그러나 현재는 false이다
        bool desired = true;  // 락이 걸리기를 기대하는 것

        while (_locked.compare_exchange_strong(expected, desired) == false)
        {
            expected = false;

            //this_thread::sleep_for(std::chrono::microseconds(100));
            this_thread::sleep_for(100ms);
            this_thread::yield(); // yield는 sleep_for의 0ms랑 동일하다.!
        }
    }
    void unlock()
    {

    }
private:
    atomic<bool> _locked = false;
};

int32 sum = 0;
mutex m;

void Add()
{
    for (int i = 0; i < 1'000'000; i++)
    {
        lock_guard<mutex> guard(m);
        sum++;
    }
}
void Sub()
{
    for (int i = 0; i < 1'000'000; i++)
    {
        lock_guard<mutex> guard(m);
        sum--;
    }
}
int main()
{
    //release 모드는 최적화가 들어간다 
    volatile int32 a = 1;
    a = 2;
    a = 3;
    a = 4;

    thread t1(Add);
    thread t2(Sub);

    cout << sum;
}
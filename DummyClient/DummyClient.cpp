
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

        bool expected = false; // ���� �ɱ⸦ ���ϴ� ����� ������� �׷��� ����� false�̴�
        bool desired = true;  // ���� �ɸ��⸦ ����ϴ� ��

        while (_locked.compare_exchange_strong(expected, desired) == false)
        {
            expected = false;

            //this_thread::sleep_for(std::chrono::microseconds(100));
            this_thread::sleep_for(100ms);
            this_thread::yield(); // yield�� sleep_for�� 0ms�� �����ϴ�.!
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
    //release ���� ����ȭ�� ���� 
    volatile int32 a = 1;
    a = 2;
    a = 3;
    a = 4;

    thread t1(Add);
    thread t2(Sub);

    cout << sum;
}
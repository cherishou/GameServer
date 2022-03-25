
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

		bool expected = false; // 락을 걸기를 원한다 그러나 현재는 false이다
		bool desired = true;  // 락이 걸리기를 기대하는 것

		////CAS 의사코드 
		//if (expected == _locked)
		//{
		//	expected = _locked; // ???
		//	_locked = desired;
		//	return true;
		//}
		////다른 누군가가 lock을 선점한 상태 !
		//else
		//{
		//	expected = _locked;
		//	return false;
		//}
		while (_locked.compare_exchanged_strong(expected, desired) == false)
		{
			expected = false;
		}
	}
	void unlock()
	{

	}
private:
	bool _locked = false;
	//volatile bool _locked = false;  volatile 키워드는 컴파일러한테 최적를 하지말아 달라라고 하는 것이다.!
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

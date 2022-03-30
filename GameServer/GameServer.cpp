
#include "pch.h"
#include <iostream>

#include <thread>
#include <atomic>
#include <mutex>
#include <Windows.h>
#include <future>
#include "ThreadManager.h"
#include "CoreMacro.h"

#include "ConcurrentQueue.h"
#include "ConcurrentStack.h"

CoreGlobal Core;

void ThreadMain()
{
	while (true)
	{
		cout << "Hello I am thread" << LThreadId << endl;
		this_thread::sleep_for(1s);
	}
}
int main()
{
	for (int i = 0; i < 5; i++)
	{
		GThreadManager->Launch(ThreadMain);
	}
	GThreadManager->Join();
}
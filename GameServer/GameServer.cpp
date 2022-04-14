#include "pch.h"
#include <iostream>
#include <atomic>
#include <mutex>
#include <future>
#include "ThreadManager.h"


#include "SocketUtils.h"
#include "Listener.h"

int main()
{
	Listener listener = MakeShared<Listener>();
	listener.StartACCEPT(NetAddress(L"127.0.0.1", 7777));
	
	for (int i = 0; i < 5; i++)
	{
		GThreadManager->Launch(
			[=]()
		{
			while (true)
			{
				GIocpCore.Dispatch();
			}
		}
		);
	}

	GThreadManager->Join();
}
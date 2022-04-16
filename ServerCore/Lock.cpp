#include "pch.h"
#include "Lock.h"
#include "DeadLockProfiler.h"

void Lock::WriteLock(const char* name)
{
#if _DEBUG
	GDeadLockProfiler->PushLock(name);
#endif
	//동일한 쓰레드가 소유하고 있다면 무조건 송겅
	const int32 lockThreadId = (_lockFlag.load() & WRITE_THREAD_MASK) >> 16;
	if (LThreadId == lockThreadId)
	{
		_writeCount++;
		return;
	}

	const int64 beginTick = ::GetTickCount64();

	//아무도 소유 및 공유하고 있지 않을 때, 경합해서 소유권을 얻는다.
	const int32 desired = ((LThreadId << 16) & WRITE_THREAD_MASK);
	
	while (true)
	{
		for (uint32 spinCount = 0; spinCount < MAX_SPIN_COUNT; spinCount++)
		{
			uint32 expected = EMPTY_FLAG;
			if (_lockFlag.compare_exchange_strong(expected, desired))
			{
				//경합에서 이긴 상태 
				_writeCount++; 
				//_writeCount를 관리하는 이유  : lock을 재귀적으로 잡을 수 
				//있도록해서 또다시 해당부분을 lock을 하게된다면 _writeCount++을 하게해준다.

				return;
			}
		}

		if (::GetTickCount64() - beginTick >= ACQUIRE_TIMEOUT_TICK)
			CRASH("Lock TimeOut");

		this_thread::yield();
	}

}

void Lock::WriteUnlock(const char* name)
{
#if _DEBUG
	GDeadLockProfiler->PopLock(name);
#endif
	//ReadLock 다 풀기전에는 WriteUnLock불가능.
	if (_lockFlag.load() & WRITE_THREAD_MASK != 0)
		CRASH("INVALID_UNLOCK_ORDER");

	const int32 lockCount = --_writeCount;
	if (lockCount == 0)
	{
		_lockFlag.store(EMPTY_FLAG);
	}
}


void Lock::ReadLock(const char* name)
{
#if _DEBUG
	GDeadLockProfiler->PushLock(name);
#endif
	const int64 beginTick = ::GetTickCount64();

	// 동일한 쓰레드가 소유하고 있다면 무조건 성공
	const int32 lockThreadId = (_lockFlag.load() & WRITE_THREAD_MASK) >> 16;
	if (LThreadId == lockThreadId)
	{
		_lockFlag.fetch_add(1);
		return;
	}

	//아무도 소유(write)하고 있지 않을 때 경합해서 공유 카운트를 올린다.

	while (true)
	{
		for (uint32 spinCount = 0; spinCount < MAX_SPIN_COUNT; spinCount++)
		{
			//WRITE부분을 0으로 쭉 미는 과정!
			uint32 expected = (_lockFlag.load() & REDA_COUNT_MASK);
			if (_lockFlag.compare_exchange_strong(expected, expected + 1))
				return;


		}
		if (::GetTickCount64() - beginTick >= ACQUIRE_TIMEOUT_TICK)
			CRASH("Lock TimeOut");

		this_thread::yield();
	}
}

void Lock::ReadUnlock(const char* name)
{
#if _DEBUG
	GDeadLockProfiler->PopLock(name);
#endif

	if ((_lockFlag.fetch_sub(1) & REDA_COUNT_MASK) == 0)
	{
		return;
	}
}

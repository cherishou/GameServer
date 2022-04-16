#include "pch.h"
#include "Lock.h"
#include "DeadLockProfiler.h"

void Lock::WriteLock(const char* name)
{
#if _DEBUG
	GDeadLockProfiler->PushLock(name);
#endif
	//������ �����尡 �����ϰ� �ִٸ� ������ �۰�
	const int32 lockThreadId = (_lockFlag.load() & WRITE_THREAD_MASK) >> 16;
	if (LThreadId == lockThreadId)
	{
		_writeCount++;
		return;
	}

	const int64 beginTick = ::GetTickCount64();

	//�ƹ��� ���� �� �����ϰ� ���� ���� ��, �����ؼ� �������� ��´�.
	const int32 desired = ((LThreadId << 16) & WRITE_THREAD_MASK);
	
	while (true)
	{
		for (uint32 spinCount = 0; spinCount < MAX_SPIN_COUNT; spinCount++)
		{
			uint32 expected = EMPTY_FLAG;
			if (_lockFlag.compare_exchange_strong(expected, desired))
			{
				//���տ��� �̱� ���� 
				_writeCount++; 
				//_writeCount�� �����ϴ� ����  : lock�� ��������� ���� �� 
				//�ֵ����ؼ� �Ǵٽ� �ش�κ��� lock�� �ϰԵȴٸ� _writeCount++�� �ϰ����ش�.

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
	//ReadLock �� Ǯ�������� WriteUnLock�Ұ���.
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

	// ������ �����尡 �����ϰ� �ִٸ� ������ ����
	const int32 lockThreadId = (_lockFlag.load() & WRITE_THREAD_MASK) >> 16;
	if (LThreadId == lockThreadId)
	{
		_lockFlag.fetch_add(1);
		return;
	}

	//�ƹ��� ����(write)�ϰ� ���� ���� �� �����ؼ� ���� ī��Ʈ�� �ø���.

	while (true)
	{
		for (uint32 spinCount = 0; spinCount < MAX_SPIN_COUNT; spinCount++)
		{
			//WRITE�κ��� 0���� �� �̴� ����!
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

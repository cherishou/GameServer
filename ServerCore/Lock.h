#pragma once
#include "Types.h"


/*
*     RW SpinLock
*/
/*
* [wwwwwww][wwwwwwww][rrrrrrr][rrrrrrr]
* w : writeFlag (Exclusive Lcok Owner ThreadId)
* r : ReadFlag (Shared Lock Count)
*/
//W -> R(O)
//R -> W(x)

class Lock
{
	enum : uint32
	{
		ACQUIRE_TIMEOUT_TICK = 10000,
		MAX_SPIN_COUNT = 5000,
		WRITE_THREAD_MASK = 0xFFFF'0000,
		REDA_COUNT_MASK = 0x0000'FFFF,
		EMPTY_FLAG = 0x0000'0000
	};
public:
	void WriteLock();
	void WriteUnlock();
	void ReadLock();
	void ReadUnlock();

private:
	Atomic<uint32> _lockFlag = EMPTY_FLAG;
	uint32 _writeCount = 0;

};

//RAII 패턴 -> 객체가 만들어질 때 LOCK을 잡는다
class ReadLockGuard
{
public:
	ReadLockGuard(Lock& lock) : _lock(lock) { _lock.ReadLock(); }
	~ReadLockGuard() { _lock.ReadUnlock(); }
private:
	Lock& _lock;

};


class WriteLockGuard
{
public:
	WriteLockGuard(Lock& lock) : _lock(lock) { _lock.WriteLock(); }
	~WriteLockGuard() { _lock.WriteUnlock(); }
private:
	Lock& _lock;

};

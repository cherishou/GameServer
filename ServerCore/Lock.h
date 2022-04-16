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
	void WriteLock(const char* name);
	void WriteUnlock(const char* name);
	void ReadLock(const char* name);
	void ReadUnlock(const char* name);

private:
	Atomic<uint32> _lockFlag = EMPTY_FLAG;
	uint32 _writeCount = 0;

};

//RAII 패턴 -> 객체가 만들어질 때 LOCK을 잡는다
class ReadLockGuard
{
public:
	ReadLockGuard(Lock& lock, const char* name) : _lock(lock), _name(name) { _lock.ReadLock(name); }
	~ReadLockGuard() { _lock.ReadUnlock(_name); }
private:
	Lock& _lock;
	const char* _name;

};


class WriteLockGuard
{
public:
	WriteLockGuard(Lock& lock, const char* name) : _lock(lock), _name(name) { _lock.WriteLock(name); }
	~WriteLockGuard() { _lock.WriteUnlock(_name); }
private:
	Lock& _lock;
	const char* _name

};

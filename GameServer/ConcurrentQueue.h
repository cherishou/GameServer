#pragma once
#include <mutex>

template<typename T>
class LockQueue
{
public:
	LockQueue() {}
	LockQueue(const LockQueue&) = delete;
	LockQueue& operator=(const LockQueue&) = delete;

	void PUsh(T value)
	{
		lock_guard<mutex> lock(_mutex);
		_queue.push(std::move(value));
		_condVar.notify_one();
	}

	//TryPop 대체용
	void WaitPop(T& value)
	{
		unique_lock<mutex> lock(_mutex);
		_condVar.wait(lock, [this] {return _queue.empty() == false; });  // 조건에 맞을때까지 wait..
		value = std::move(_queue.front());
		_queue.pop();
	}

	//데이터가 없더라도 데이가 있는지 없는지 체크를 해야서 계속해서 TryPop을 해야한다 SPINLOCK 느낌..
	bool TryPop(T& value)
	{
		lock_guard<mutex> lock(_mutex);
		if (_queue.empty()) return false;

		value = std::move(_queue.front());
		_queue.pop();
		return true;
	}
public:
	queue<T> _queue;
	mutex _mutex;
	condition_variable _condVar;
};


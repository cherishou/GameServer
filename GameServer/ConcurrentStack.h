#pragma once
#include <mutex>

template<typename T>
class LockStack
{
public:
	LockStack() { }
	LockStack(const LockStack&) = delete;
	LockStack& operator=(const LockStack&) = delete;

	void Push(T& value)
	{
		lock_guard<mutex> lock(_mutex);
		_stack.push(std::move(value)); //이동 성능 향상 
		_cv.notify_one();
	}

	//TryPop 대체용
	void WaitPop(T& value)
	{
		unique_lock<mutex> lock(_mutex);
		_cv.wait(lock, [this] {return _stack.empty() == false; });  // 조건에 맞을때까지 wait..
		value = std::move(_stack.top());
		_stack.pop();
	}

	//데이터가 없더라도 데이가 있는지 없는지 체크를 해야서 계속해서 TryPop을 해야한다 SPINLOCK 느낌..
	bool TryPop(T& value)
	{
		lock_guard<mutex> lock(_mutex);
		if (_stack.empty()) return false;

		value = std::move(_stack.top());
		_stack.pop();
		return true;
	}
	/*
	* 멀티스레드 환경에서는 엠피가 큰 효과가 없다 이유는 현재 엠티가 아니라고 체크 한후 pop을 할려고했는데
	* 그 사이에 다른 스레드가  와서 나머지를 가져간다면 엠티가 되기때문...
	*/
	bool Empty()
	{
		lock_guard<mutex> lock(_mutex);

	}
private:
	stack<T> _stack;
	mutex _mutex;
	condition_variable _cv;
};


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
		_stack.push(std::move(value)); //�̵� ���� ��� 
		_cv.notify_one();
	}

	//TryPop ��ü��
	void WaitPop(T& value)
	{
		unique_lock<mutex> lock(_mutex);
		_cv.wait(lock, [this] {return _stack.empty() == false; });  // ���ǿ� ���������� wait..
		value = std::move(_stack.top());
		_stack.pop();
	}

	//�����Ͱ� ������ ���̰� �ִ��� ������ üũ�� �ؾ߼� ����ؼ� TryPop�� �ؾ��Ѵ� SPINLOCK ����..
	bool TryPop(T& value)
	{
		lock_guard<mutex> lock(_mutex);
		if (_stack.empty()) return false;

		value = std::move(_stack.top());
		_stack.pop();
		return true;
	}
	/*
	* ��Ƽ������ ȯ�濡���� ���ǰ� ū ȿ���� ���� ������ ���� ��Ƽ�� �ƴ϶�� üũ ���� pop�� �ҷ����ߴµ�
	* �� ���̿� �ٸ� �����尡  �ͼ� �������� �������ٸ� ��Ƽ�� �Ǳ⶧��...
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


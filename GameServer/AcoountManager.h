#pragma once

#include <mutex>

class Account
{
	//TODO
};
class AcoountManager
{
public:
	//�̱������� ���� ��𼭵��� ���ٰ����ϰ� ������ ������ �� �ֵ���
	static AcoountManager* Instance()
	{
		static AcoountManager instance;
		return &instance;
	}

	Account* GetAccount(int32 id)
	{
		lock_guard<mutex> guard(_mutex);
		//������ ���� �� 
		return nullptr;
	}
private:
	mutex _mutex;
};


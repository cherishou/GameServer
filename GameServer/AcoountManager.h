#pragma once

#include <mutex>

class Account
{
	//TODO
};
class AcoountManager
{
public:
	//싱글톤으로 언제 어디서든지 접근가능하고 계정을 추출할 수 있도록
	static AcoountManager* Instance()
	{
		static AcoountManager instance;
		return &instance;
	}

	Account* GetAccount(int32 id)
	{
		lock_guard<mutex> guard(_mutex);
		//뭔가를 갖고 옴 
		return nullptr;
	}
private:
	mutex _mutex;
};


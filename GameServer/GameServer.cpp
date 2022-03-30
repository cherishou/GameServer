
#include "pch.h"
#include <iostream>

#include <thread>
#include <atomic>
#include <mutex>
#include <Windows.h>
#include <future>
#include "ThreadManager.h"
#include "CoreMacro.h"

#include "ConcurrentQueue.h"
#include "ConcurrentStack.h"

#include "RefCounting.h"

class Wraight : public RefCounting
{
public:
	Wraight() {}
public:
	int _hp = 100;
	int _posX = 0;
	int _posY = 0;
};

class Missile : public RefCounting
{
public:
	void SetTarget(Wraight* target)
	{
		_target = target;
		target->AddRef();
	}

	void Update()
	{
		int posX = _target->_posX;
		int posY = _target->_posY;

		if (_target->_hp == 0)
		{
			_target->ReleaseRef();
			_target = nullptr;
			return  true;
		}
	}
public:
	Wraight* _target = nullptr;	
};

int main()
{
	Wraight* wraight = new Wraight();
	Missile* missile = new Missile();

	missile->SetTarget(wraight);

	//미사일 맞기전에 레이스가 피격당함 
	wraight->_hp = 0;
	delete wraight; //여기서 문제가 발생
	while (true)
	{
		if (missile)
		{
			missile->Update();
		}
	}

	delete missile;
}
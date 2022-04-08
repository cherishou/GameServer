#include "pch.h"
#include <iostream>
#include "CorePch.h"
#include <atomic>
#include <mutex>
#include <windows.h>
#include <future>
#include "ThreadManager.h"

#include "RefCounting.h"

#include "Memory.h"

#pragma once

/*---------------
   RefCountable
----------------*/

class RefCountable
{
public:
	RefCountable() : _refCount(1) { }
	virtual ~RefCountable() { }

	int32 GetRefCount() { return _refCount; }

	int32 AddRef() { return ++_refCount; }
	int32 ReleaseRef()
	{
		int32 refCount = --_refCount;
		if (refCount == 0)
		{
			delete this;
		}
		return refCount;
	}

protected:
	atomic<int32> _refCount;
};

/*---------------
   SharedPtr
----------------*/

template<typename T>
class TSharedPtr
{
public:
	TSharedPtr() { }
	TSharedPtr(T* ptr) { Set(ptr); }

	// 복사
	TSharedPtr(const TSharedPtr& rhs) { Set(rhs._ptr); }
	// 이동
	TSharedPtr(TSharedPtr&& rhs) { _ptr = rhs._ptr; rhs._ptr = nullptr; }
	// 상속 관계 복사
	template<typename U>
	TSharedPtr(const TSharedPtr<U>& rhs) { Set(static_cast<T*>(rhs._ptr)); }

	~TSharedPtr() { Release(); }

public:
	// 복사 연산자
	TSharedPtr& operator=(const TSharedPtr& rhs)
	{
		if (_ptr != rhs._ptr)
		{
			Release();
			Set(rhs._ptr);
		}
		return *this;
	}

	// 이동 연산자
	TSharedPtr& operator=(TSharedPtr&& rhs)
	{
		Release();
		_ptr = rhs._ptr;
		rhs._ptr = nullptr;
		return *this;
	}

	bool		operator==(const TSharedPtr& rhs) const { return _ptr == rhs._ptr; }
	bool		operator==(T* ptr) const { return _ptr == ptr; }
	bool		operator!=(const TSharedPtr& rhs) const { return _ptr != rhs._ptr; }
	bool		operator!=(T* ptr) const { return _ptr != ptr; }
	bool		operator<(const TSharedPtr& rhs) const { return _ptr < rhs._ptr; }
	T* operator*() { return _ptr; }
	const T* operator*() const { return _ptr; }
	operator T* () const { return _ptr; }
	T* operator->() { return _ptr; }
	const T* operator->() const { return _ptr; }

	bool IsNull() { return _ptr == nullptr; }

private:
	inline void Set(T* ptr)
	{
		_ptr = ptr;
		if (ptr)
			ptr->AddRef();
	}

	inline void Release()
	{
		if (_ptr != nullptr)
		{
			_ptr->ReleaseRef();
			_ptr = nullptr;
		}
	}

private:
	T* _ptr = nullptr;
};

class Player
{
public:
	Player() {}
	virtual ~Player() {}
};
class Knight  : public Player
{
public:

	Knight()
	{
		cout << "Knight()" << endl;
	}
	Knight(int32 hp)
	{
		cout << "Knight(hp)" << endl;
	}
	~Knight()
	{
		cout << "~Knight()" << endl;
	}
public:
	int hp = 100;
	int mp = 200;
};

int main()
{
	// 가상 메모리 기본
	int* num = new int;
	*num = 100;

	// address는 진짜 주소가아니다..
	// physical 주소는 다른 곳에있다.
	// 유저레벨(메모장, 롤, 서버)
	// 커널레벨 (os code)
	// 유저레벨기리는 서로 간접하지않도록 만든다. 
	// 간접하도록하면 해킹의 위험도 많다...

	//2GB[                ]
	// 2GB / 4KB [R][W][X][X][][][][][][
	//메모리에 정책을 둘 수 있다. 
	//메모리를 관리할때 페이지단위로 관리한다.
	// 아파트 분양 모형으로 생각하는 것이 페이지 
	// PAGING

	//기본적으로 윈도우는 PAGE는 4KB는 고정되어있다. 
	SYSTEM_INFO info;
	::GetSystemInfo(&info);

	info.dwPageSize; // 4KB

	int* test = (int*)::VirtualAlloc(NULL, 4, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE); // 메모리를 줘

	::VirtualFree();


	
	int64 address = reinterpret_cast<int64>(num);
	cout << address << endl;
	delete num;

	return 0;
}
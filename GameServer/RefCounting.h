#pragma once
//RefCounting을 하는 방법은 여러가지인데, 최상위 객체에 RefCounting를 넣어주고, 밑에서 상속받아서 레프카운팅을 사용하는 방법

class RefCounting
{
public:
	RefCounting() : _refCount(1) {}
	virtual ~RefCounting() {}


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
	int32 _refCount;
};


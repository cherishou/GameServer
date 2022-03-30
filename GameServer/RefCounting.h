#pragma once
//RefCounting�� �ϴ� ����� ���������ε�, �ֻ��� ��ü�� RefCounting�� �־��ְ�, �ؿ��� ��ӹ޾Ƽ� ����ī������ ����ϴ� ���

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


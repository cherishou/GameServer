#pragma once
class RefCountable
{
public:
	RefCountable() : _refCount(1) {}
	virtual ~RefCountable() {}

	int32 GetRefCount() { return _refCount; }

	int AddRef() { return _refCount++; }
	int32 ReleaseRef()
	{
		--_refCount;
		if (_refCount == 0)
		{
			delete this;
		}
		return _refCount;
	}
protected:
	int32 _refCount;
};


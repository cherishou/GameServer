#pragma once

class IocpObject
{
public:
	virtual HANDLE GetHandle() abstract;
	virtual void Dispatch(class IocpEvent* iocpEvent, int32 numOfBytes) abstract;
	//iocpEvent는 원래 overlapped에서 read인지 write인지 체크했던것을 하는 역할
	//
};
class IocpCore
{
public:
	IocpCore();
	~IocpCore();


	HANDLE GetHandle() { return _iocpHandle; }

	//complietion port에 소켓을 등록했었는데 그 담당이 Register
	bool Register(class IocpObject* iocpObject);
	bool Dispatch(uint32 timeoutMs = INFINITE); // workerThread가 일감이 있는지 찾아 다니는 함수. 내부적으로 GETQUE~가 실행하면서
	//일감을 대기


private:
	HANDLE _iocpHandle;
};


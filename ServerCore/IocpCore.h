#pragma once

//IocpObject는 iocp에 등록을 할 수 있는 것들..
//sesssion이 될 수도 있고.. 


//enable_shared_from_this 자기자신의 wekptr을 들고있다.
//
class IocpObject : public enable_shared_from_this< IocpObject>
{
public:
	virtual HANDLE GetHandle() abstract;
	virtual void Dispatch(class IocpEvent* iocpEvent, int32 numOfBytes = 0) abstract;
	//iocpEvent가 accept인지 send인지에따라서 Dispatch가 처리한다.
	//
};
/*
* ----------------
*	IocpCore
* ----------------
*/
class IocpCore
{
public:
	IocpCore();
	~IocpCore();


	HANDLE GetHandle() { return _iocpHandle; }

	//complietion port에 소켓이나 세션을 등록했었는데 그 담당이 Register
	//등록할 iocpOjbect를 넣어준다.
	bool Register(ServerService session);
	bool Dispatch(uint32 timeoutMs = INFINITE); // workerThread가 일감이 있는지 찾아 다니는 함수. 내부적으로 GETQUE~가 실행하면서
	//일감을 대기


private:
	HANDLE _iocpHandle;
};

extern IocpCore GIocpCore;
#pragma once

//IocpObject�� iocp�� ����� �� �� �ִ� �͵�..
//sesssion�� �� ���� �ְ�.. 


//enable_shared_from_this �ڱ��ڽ��� wekptr�� ����ִ�.
//
class IocpObject : public enable_shared_from_this< IocpObject>
{
public:
	virtual HANDLE GetHandle() abstract;
	virtual void Dispatch(class IocpEvent* iocpEvent, int32 numOfBytes = 0) abstract;
	//iocpEvent�� accept���� send���������� Dispatch�� ó���Ѵ�.
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

	//complietion port�� �����̳� ������ ����߾��µ� �� ����� Register
	//����� iocpOjbect�� �־��ش�.
	bool Register(ServerService session);
	bool Dispatch(uint32 timeoutMs = INFINITE); // workerThread�� �ϰ��� �ִ��� ã�� �ٴϴ� �Լ�. ���������� GETQUE~�� �����ϸ鼭
	//�ϰ��� ���


private:
	HANDLE _iocpHandle;
};

extern IocpCore GIocpCore;
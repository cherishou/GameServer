#pragma once

class IocpObject
{
public:
	virtual HANDLE GetHandle() abstract;
	virtual void Dispatch(class IocpEvent* iocpEvent, int32 numOfBytes) abstract;
	//iocpEvent�� ���� overlapped���� read���� write���� üũ�ߴ����� �ϴ� ����
	//
};
class IocpCore
{
public:
	IocpCore();
	~IocpCore();


	HANDLE GetHandle() { return _iocpHandle; }

	//complietion port�� ������ ����߾��µ� �� ����� Register
	bool Register(class IocpObject* iocpObject);
	bool Dispatch(uint32 timeoutMs = INFINITE); // workerThread�� �ϰ��� �ִ��� ã�� �ٴϴ� �Լ�. ���������� GETQUE~�� �����ϸ鼭
	//�ϰ��� ���


private:
	HANDLE _iocpHandle;
};


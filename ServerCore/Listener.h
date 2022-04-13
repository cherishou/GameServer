#pragma once
#include "IocpCore.h"
#include "NetAddress.h"


class AcceptEvent;

//Listener�� iocp�ھ ����� �� ���̴�.
class Listener : public IocpObject
{
public:
	Listener() = default;
	~Listener();

public:
	/*�ܺο��� ���*/
	bool StartACCEPT(NetAddress netAddress);
	void CloseSocket();
public:
	/*�������̽� ���� */
	virtual HANDLE GetHandle() ;
	virtual void Dispatch(class IocpEvent* iocpEvent, int32 numOfBytes) ;

private:
	/*���Ű���*/
	void RegisterAccept(AcceptEvent* acceptEvent);
	void ProcessAccept(AcceptEvent* acceptEvent);  // register���� �� ������ ���� ��

protected:
	SOCKET _socket = INVALID_SOCKET;
	vector<AcceptEvent*> _acceptEvents;
};


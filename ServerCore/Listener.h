#pragma once
#include "IocpCore.h"
#include "NetAddress.h"


class AcceptEvent;

/*
* -------------------------
*		Listener
* -------------------------
*/

//Listener�� iocp�ھ ����� �� ���̴�.
class Listener : public IocpObject
{
public:
	Listener() = default;
	~Listener();

public:
	/*�ܺο��� ���*/
	//netAddress�� ������� ������ �����Ѵ�.
	bool StartACCEPT(NetAddress netAddress); //listener�� �������� ������ �ض�!
	void CloseSocket();
public:
	/*�������̽� ���� */
	virtual HANDLE GetHandle() override;
	virtual void Dispatch(class IocpEvent* iocpEvent, int32 numOfBytes) override;

private:
	/*���Ű���*/
	void RegisterAccept(AcceptEvent* acceptEvent);
	void ProcessAccept(AcceptEvent* acceptEvent);  // register���� �� ������ ���� ��

protected:
	SOCKET _socket = INVALID_SOCKET;
	vector<AcceptEvent*> _acceptEvents;
	ServerServiceRef _service;
};


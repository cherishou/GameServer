#pragma once
/*
* listener 와 마찬가지로 iocpCore에 등록할 것.
* 
* 클라의 정보들은 다 session에 넣는다.
*/
#include "IocpCore.h"
#include "IocpEvent.h"
#include "NetAddress.h"

class Session : public IocpObject
{
public:
	Session();
	virtual ~Session();


public:
	/*정보관련*/
	void SetNetAddress(NetAddress address) { _netAddress = address; }
	NetAddress GetAddress() { return _netAddress; }
	SOCKET GetSocket() { return _socket; }

public:
	/*인터페이스 구현*/
	virtual HANDLE GetHandle() override;
	virtual void Dispatch(class IocpEvent* iocpEvent, int32 numOfBytes) override;

public:
	char _recvBuffer[1000];
private:
	SOCKET		 _socket = INVALID_SOCKET;
	NetAddress	 _netAddress = {};
	Atomic<bool> _connected = false;
};


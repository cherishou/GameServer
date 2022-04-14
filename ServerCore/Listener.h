#pragma once
#include "IocpCore.h"
#include "NetAddress.h"


class AcceptEvent;

/*
* -------------------------
*		Listener
* -------------------------
*/

//Listener를 iocp코어에 등록을 할 것이다.
class Listener : public IocpObject
{
public:
	Listener() = default;
	~Listener();

public:
	/*외부에서 사용*/
	//netAddress를 대상으로 역할을 수행한다.
	bool StartACCEPT(NetAddress netAddress); //listener가 문지기의 역할을 해라!
	void CloseSocket();
public:
	/*인터페이스 구현 */
	virtual HANDLE GetHandle() override;
	virtual void Dispatch(class IocpEvent* iocpEvent, int32 numOfBytes) override;

private:
	/*수신관련*/
	void RegisterAccept(AcceptEvent* acceptEvent);
	void ProcessAccept(AcceptEvent* acceptEvent);  // register했을 때 실질적 동작 ㅇ

protected:
	SOCKET _socket = INVALID_SOCKET;
	vector<AcceptEvent*> _acceptEvents;
	ServerServiceRef _service;
};


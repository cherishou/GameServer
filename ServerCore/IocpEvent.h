#pragma once

class Session;
enum class EventType : uint8
{
	Connect,
	Accept,
	//PreRecv  0 byte recv
	Recv,
	Send
};
//IocpEvent는 iocpCore에 넣어줄때 사유랑 같이 넣어주는 느낌.
//iocpEventclass는 OVERLAPPEDEx랑 비슷한 느낌
//virtual을 사용하면 안되는 이유는
//virtual을 사용하면 제일 첫메모리 주소에 
//OVERLAPPED가 아닌 vttable이 끼이기때문에
class IocpEvent : public OVERLAPPED
	//상속을 받으면 제일 처음 offsetmemory에는
	//OVERLAPPED가 들어있다.
{
public:
	IocpEvent(EventType type);

	void		Init();
	EventType	GetType() { return eventType; }

public:
	EventType eventType;
	//IocpEvent를 만들 때 자신의 owner를 기억해야한다. 주신의 shared_ptr을 기억!
	IocpObjectRef owner;
};

//인자가 있을 수도 있고, 없을 수도 있어서 나눠서 관리를한다.

/*
* ConnectEvent
*/
class ConnectEvent : public IocpEvent
{
public:
	ConnectEvent() : IocpEvent(EventType::Connect) {}
};

/*
* AcceptEventd
* 여기에 session정보를 연동한다 
* why -> dispatch에서 해당 event를 가지고 왔을 때
* 어떤 session을 넘겼는지 알 수 있기때문에
*/
class AcceptEvent : public IocpEvent
{
public:
	AcceptEvent() : IocpEvent(EventType::Accept) {}

public:
	SessionRef _session = nullptr;
};

/*
* RecvEvent
*/
class RecvEvent : public IocpEvent
{
public:
	RecvEvent() : IocpEvent(EventType::Recv) {}
};

/*
* SendEvent
*/
class SendEvent : public IocpEvent
{
public:
	SendEvent() : IocpEvent(EventType::Send) {}
};
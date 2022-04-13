#pragma once

class Session;
enum class EventType : uint8
{
	Connect,
	Accept,
	//PreRecv
	Recv,
	Send
};
//iocpEventclass는 OVERLAPPEDEx랑 비슷한 느낌
//virtual을 사용하면 안되는 이유는
//virtual을 사용하면 제일 첫메모리 주소에 
//OVERLAPPED가 아닌 vttable이 끼이기때문에
class IocpEvent : public OVERLAPPED
{
public:
	IocpEvent(EventType type);

	void Init();


public:
	EventType eventType;
	//IocpEvent를 만들 때 자신의 owner를 기억해야한다.
	IocpObjectRef owner;
};


/*
* ConnectEvent
*/
class ConnectEvent : public IocpEvent
{
public:
	ConnectEvent() : IocpEvent(EventType::Connect) {}
};

/*
* AcceptEvent
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

};

/*
* SendEvent
*/
class SendEvent : public IocpEvent
{

};
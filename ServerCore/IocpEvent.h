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
//iocpEventclass�� OVERLAPPEDEx�� ����� ����
//virtual�� ����ϸ� �ȵǴ� ������
//virtual�� ����ϸ� ���� ù�޸� �ּҿ� 
//OVERLAPPED�� �ƴ� vttable�� ���̱⶧����
class IocpEvent : public OVERLAPPED
{
public:
	IocpEvent(EventType type);

	void Init();


public:
	EventType eventType;
	//IocpEvent�� ���� �� �ڽ��� owner�� ����ؾ��Ѵ�.
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
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
//IocpEvent�� iocpCore�� �־��ٶ� ������ ���� �־��ִ� ����.
//iocpEventclass�� OVERLAPPEDEx�� ����� ����
//virtual�� ����ϸ� �ȵǴ� ������
//virtual�� ����ϸ� ���� ù�޸� �ּҿ� 
//OVERLAPPED�� �ƴ� vttable�� ���̱⶧����
class IocpEvent : public OVERLAPPED
	//����� ������ ���� ó�� offsetmemory����
	//OVERLAPPED�� ����ִ�.
{
public:
	IocpEvent(EventType type);

	void		Init();
	EventType	GetType() { return eventType; }

public:
	EventType eventType;
	//IocpEvent�� ���� �� �ڽ��� owner�� ����ؾ��Ѵ�. �ֽ��� shared_ptr�� ���!
	IocpObjectRef owner;
};

//���ڰ� ���� ���� �ְ�, ���� ���� �־ ������ �������Ѵ�.

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
* ���⿡ session������ �����Ѵ� 
* why -> dispatch���� �ش� event�� ������ ���� ��
* � session�� �Ѱ���� �� �� �ֱ⶧����
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
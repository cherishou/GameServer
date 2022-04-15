#pragma once
/*
* listener �� ���������� iocpCore�� ����� ��.
* 
* Ŭ���� �������� �� session�� �ִ´�.
*/
#include "IocpCore.h"
#include "IocpEvent.h"
#include "NetAddress.h"
#include "RecvBuffer.h"
#include <vector>


class Service;

class Session : public IocpObject
{
	friend class Listener;
	friend class IocpCore;
	friend class Service;

	enum MyEnum
	{

	};
public:
	Session();
	virtual ~Session();

public:
	/*�ܺο��� ���*/
	//send�� recv�� �޸� ���� ���� �����Ͱ� ���� ���� ȣ���ؾ��Ѵ� recv�� ��å�� �ݴ��̴�.
	void		Send(SendBufferRef sendBuffer);
	bool		Connect();
	void		Disconnect(const WCHAR* cause);

public:
	//������ ������ ���������ؼ� ��� (ex : ��ŷ)
	void				Disconnect(const WCHAR* cause);
	shared_ptr<Service>	GetService() { return _service.lock(); }
	void				SetService(shared_ptr<Service> service) { _service = service; }

public:
	/*��������*/
	void			SetNetAddress(NetAddress address) { _netAddress = address; }
	NetAddress		GetAddress() { return _netAddress; }
	SOCKET			GetSocket() { return _socket; }
	//����Ǿ��ִ����� ���� ��Ŷ�� ��� ������ ������ üũ 
	bool			IsConnected() { return _connected; }
	SessionRef		GetSessionRef() { return static_pointer_cast<Session>(shared_from_this()); }
public:
	/*�������̽� ����*/
	virtual HANDLE GetHandle() override;
	virtual void Dispatch(class IocpEvent* iocpEvent, int32 numOfBytes) override;

private:
						/* ���۰���		*/
	bool			RegisterConnect();
	void			RegisterRecv();
	void			RegisterSend(SendEvent* sendEvent);

	void			ProcessConnect();
	void			ProcessRecv(int32 numOfBytes);
	void			ProcessSend(int32 numOfBytes);

	void			HandleError(int32 errorCode);
protected:
	/*������ �ڵ忡�� �����ε�*/
	virtual void	OnConnected(){}
	virtual int	OnRecv(BYTE* buffer, int32 len) { return len; }
	virtual void	OnSend(int32 len) {}
	virtual void	OnDisconnected(){}
public:
	//���� _recvBuffer�� ������ �Ǵ� ����.
	//->TCP�� 100�� ���´ٰ� �ؼ� 100�� ���� ���� �����ִ� ����ǰ����� ���⶧����.
	//��ǥ�� a�� x�������� �̵��ϰڴٴ� �ǹ��ε� x������ �߸��ٸ�..
	BYTE _recvBuffer[1000];

	//_sendBuffer�� ���������� �ε� ������ ������� �ʵ���[    /     /..    ]
	char _sendBuffer[1000];
	int32 _sendLen = 0;
private:
	//shared_ptr�� ���� ���� ������ ��ȯ�� ���̱� ���ؼ�..
	// 
	//service�� ���� ������ �˾ƾ��� �ڽ��� ������ ����ϰų� �����ϰų� �� �� �ִ�.
	weak_ptr<Service> _service;
	SOCKET		 _socket = INVALID_SOCKET;
	NetAddress	 _netAddress = {};
	Atomic<bool> _connected = false;

private:
	USE_LOCK;

	/*���� ����*/
	RecvBuffer		_recvBuffer;
	/*�۽� ����*/
	queue<SendBufferRef>	_sendQueue;
	Atomic<bool>			_sendRegistered = false;

private:
	/*IocpEvent ����*/
	RecvEvent _recvEvent; 
	SendEvent _sendEvent;
};


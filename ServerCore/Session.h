#pragma once
/*
* listener 와 마찬가지로 iocpCore에 등록할 것.
* 
* 클라의 정보들은 다 session에 넣는다.
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
	/*외부에서 사용*/
	//send는 recv와 달리 정말 보낼 데이터가 있을 때만 호출해야한다 recv랑 정책이 반대이다.
	void		Send(SendBufferRef sendBuffer);
	bool		Connect();
	void		Disconnect(const WCHAR* cause);

public:
	//연결이 끊어진 이유에대해서 출력 (ex : 해킹)
	void				Disconnect(const WCHAR* cause);
	shared_ptr<Service>	GetService() { return _service.lock(); }
	void				SetService(shared_ptr<Service> service) { _service = service; }

public:
	/*정보관련*/
	void			SetNetAddress(NetAddress address) { _netAddress = address; }
	NetAddress		GetAddress() { return _netAddress; }
	SOCKET			GetSocket() { return _socket; }
	//연결되어있는지에 따라서 패킷을 계속 전송할 것인지 체크 
	bool			IsConnected() { return _connected; }
	SessionRef		GetSessionRef() { return static_pointer_cast<Session>(shared_from_this()); }
public:
	/*인터페이스 구현*/
	virtual HANDLE GetHandle() override;
	virtual void Dispatch(class IocpEvent* iocpEvent, int32 numOfBytes) override;

private:
						/* 전송관련		*/
	bool			RegisterConnect();
	void			RegisterRecv();
	void			RegisterSend(SendEvent* sendEvent);

	void			ProcessConnect();
	void			ProcessRecv(int32 numOfBytes);
	void			ProcessSend(int32 numOfBytes);

	void			HandleError(int32 errorCode);
protected:
	/*컨텐츠 코드에서 오버로딩*/
	virtual void	OnConnected(){}
	virtual int	OnRecv(BYTE* buffer, int32 len) { return len; }
	virtual void	OnSend(int32 len) {}
	virtual void	OnDisconnected(){}
public:
	//현재 _recvBuffer가 문제가 되는 이유.
	//->TCP는 100을 보냈다고 해서 100을 받지 않을 수도있다 경계의개념이 없기때문에.
	//목표가 a가 x지점으로 이동하겠다는 의미인데 x지점이 잘린다면..
	BYTE _recvBuffer[1000];

	//_sendBuffer를 내부적으로 두되 영역이 복사되지 않도록[    /     /..    ]
	char _sendBuffer[1000];
	int32 _sendLen = 0;
private:
	//shared_ptr로 하지 않은 이유는 순환을 줄이기 위해서..
	// 
	//service에 대한 정볼르 알아야지 자신의 내용을 등록하거나 제거하거나 할 수 있다.
	weak_ptr<Service> _service;
	SOCKET		 _socket = INVALID_SOCKET;
	NetAddress	 _netAddress = {};
	Atomic<bool> _connected = false;

private:
	USE_LOCK;

	/*수신 관련*/
	RecvBuffer		_recvBuffer;
	/*송신 관련*/
	queue<SendBufferRef>	_sendQueue;
	Atomic<bool>			_sendRegistered = false;

private:
	/*IocpEvent 재사용*/
	RecvEvent _recvEvent; 
	SendEvent _sendEvent;
};


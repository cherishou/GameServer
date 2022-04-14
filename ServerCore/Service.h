#pragma once

#include "NetAddress.h"
#include "IocpCore.h"
#include "Listener.h"
#include <functional>
/*
* 서버도 분산 서버로서 하나만 있는 것이아니다, DB를 담당하는 서버
* 게임로직을 담당하는 서버. ,,
*/
enum class ServiceType : uint8
{
	Server,
	Client
};
/*
* =================
*		Service
* session listen iocpCore를 묶어서 관리하도록 했다.
* 
* =================
*/
using SessionFactory = function<SessionRef(void)>;

class Service  : enable_shared_from_this<Service>
{
public:
	// 서버라면 자기 자신의 주소, 클라라면 상대방의 주소를(ADDREDSS)
	// 해당 서비스가 어떤 타입인지 정의를 해야한다. 클라인지 서버인지 
	Service(ServiceType type, NetAddress address, IocpCoreRef core, SessionFactory factory, int32 maxSessionCount = 1);
	virtual ~Service();

	virtual bool Start() abstract;
	bool		 CanStart() { return _sessionFactory != nullptr; }

	virtual void CloseService();
	void		 SetSessionFactory(SessionFactory func) { _sessionFactory; }
	

	//session을 만들고 iocpCore에 연결
	SessionRef CreateSession();
	void		AddSession(SessionRef session);
	void		ReleaseSession(SessionRef session);
	int32		GetCurrentSessionCount() { return _sessionCount; }
	int32       GetMaxSessionCount() { return _maxSessionCount; }

public:
	ServiceType	GetSerServiceType() { return _type; }
	NetAddress	GetNetAddress() { return _netAddress; }
	IocpCoreRef& GetIocpCore() { return _iocpCore; }

protected:
	USE_LOCK;
	ServiceType _type;
	NetAddress _netAddress = {};
	IocpCoreRef _iocpCore;

	set<SessionRef> _sessions;
	int32			_sessionCount = 0;
	int32			_maxSessionCount = 0;
	SessionFactory	_sessionFactory;
};

class ClientService : public Service
{
public:
	ClientService(NetAddress targetAddresss, IocpCoreRef core, SessionFactory factory,
		int32 maxSessionCount = 1);
	virtual ~ClientService() {}
	virtual bool Start() override; // Start하면 클라이언트 서비스를 상대방 targetAddress에 연결
};

class ServerService : public Service
{
public:

};
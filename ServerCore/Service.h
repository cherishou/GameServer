#pragma once

#include "NetAddress.h"
#include "IocpCore.h"
#include "Listener.h"
#include <functional>
/*
* ������ �л� �����μ� �ϳ��� �ִ� ���̾ƴϴ�, DB�� ����ϴ� ����
* ���ӷ����� ����ϴ� ����. ,,
*/
enum class ServiceType : uint8
{
	Server,
	Client
};
/*
* =================
*		Service
* session listen iocpCore�� ��� �����ϵ��� �ߴ�.
* 
* =================
*/
using SessionFactory = function<SessionRef(void)>;

class Service  : enable_shared_from_this<Service>
{
public:
	// ������� �ڱ� �ڽ��� �ּ�, Ŭ���� ������ �ּҸ�(ADDREDSS)
	// �ش� ���񽺰� � Ÿ������ ���Ǹ� �ؾ��Ѵ�. Ŭ������ �������� 
	Service(ServiceType type, NetAddress address, IocpCoreRef core, SessionFactory factory, int32 maxSessionCount = 1);
	virtual ~Service();

	virtual bool Start() abstract;
	bool		 CanStart() { return _sessionFactory != nullptr; }

	virtual void CloseService();
	void		 SetSessionFactory(SessionFactory func) { _sessionFactory; }
	

	//session�� ����� iocpCore�� ����
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
	virtual bool Start() override; // Start�ϸ� Ŭ���̾�Ʈ ���񽺸� ���� targetAddress�� ����
};

class ServerService : public Service
{
public:

};
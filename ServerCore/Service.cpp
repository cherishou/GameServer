#include "pch.h"
#include "Service.h"

Service::Service(ServiceType type, NetAddress address, IocpCoreRef core, SessionFactory factory, int32 maxSessionCount)
	: _type(type), _netAddress(address), _iocpCore(core), _sessionFactory(factory), _maxSessionCount(maxSessionCount)
{

}

Service::~Service()
{
}
//CreateSession���� iocpCore���� ����Ѵ�.
SessionRef Service::CreateSession()
{
	SessionRef session = _sessionFactory();

	if (_iocpCore->Register(session) == false) return nullptr;

	return session;
}

void Service::SetSessionFactory()
{
}

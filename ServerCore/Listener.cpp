#include "pch.h"
#include "Listener.h"
#include "IocpEvent.h"
#include "SocketUtils.h"
#include "Session.h"
#include "Service.h"

/*
* -------------------------
*		Listener
* -------------------------
*/



Listener::~Listener()
{
	//SocketUtils::Close(_socket);
}

bool Listener::StartACCEPT(ServerServiceRef service)
{
	_service = service;
	if (_service == nullptr) return false;

	_socket = SocketUtils::CreateSocket();
	
	if (_service->GetIocpCore()->Register(shared_from_this()) == false)
		return false;

	if (SocketUtils::SetReuseAddress(_socket, true) == false)
		return false;

	if (SocketUtils::SetLinger(_socket, 0, 0) == false)
		return false;

	if (SocketUtils::Bind(_socket, _service->GetNetAddress()) == false)
		return false;

	if (SocketUtils::Listen(_socket) == false)
		return false;
	//acceptCount��ŭ �ؾ��ϴ� ������ client�� ������ ���
	//�ش��ϴ� ������ŭ �� ó���غ��� ���ؼ�.
	//ó���� RegisterAccept�� ����Ǹ� ������.
	//�׷��������� iocpCore���� �ϰ��� �Ϸ�� ���� ó���Ѵ�.
	//
	const int32 acceptCount = 1;
	for (int i = 0; i < acceptCount; i++)
	{
		AcceptEvent* acceptEvent = new AcceptEvent();
		//acceptEvent->owner = this;
		//�ٱ��� owner�� ����Ʈ�������ε� �װ��� �־��ٶ� ����ұ�?
		//�Ǽ�1
		//acceptEvent->owner = shared_ptr<IocpObject>(this)
		// shared_ptr<IocpObject>(this)�� refcount��1�� ���ο� ���� ������ ��.
		//�̰��� �ذ��ϱ� ���ؼ��� IocpObject�� �ٸ� ���� ��� �޾ƾ��Ѵ�.
		acceptEvent->owner = shared_from_this();
		_acceptEvents.push_back(acceptEvent);
		RegisterAccept(acceptEvent);
	}
	
	return true;
}

void Listener::CloseSocket()
{
	SocketUtils::Close(_socket);
}

HANDLE Listener::GetHandle()
{
	return reinterpret_cast<HANDLE>(_socket);
}

//worker��������� Ȱ���ϴ� ��.
void Listener::Dispatch(IocpEvent* iocpEvent, int32 numOfBytes)
{
	AcceptEvent* acceptEvent = static_cast<AcceptEvent*>(iocpEvent);
	ProcessAccept(acceptEvent);
}

//iocp���� ó���� �� �ֵ��� �����ϴ� ����.
void Listener::RegisterAccept(AcceptEvent* acceptEvent)
{
	//session ������ AcceptEvent�� ������ ��ų ���̴�.
	//���߿� Dispatch���� �ش� iocpEvent �̺�Ʈ�� ������ ���� �� 
	//� session�� �Ѱ���� �� �� �ֱ⿡.
	SessionRef session = MakeShared<Session>();
	acceptEvent->Init();
	acceptEvent->_session = session;

	DWORD bytesReceived = 0;
	//AcceptEx �񵿱� �����..
	if (false == SocketUtils::AcceptEx(_socket, session->GetSocket(),
		session->_recvBuffer/*ó�������� �ξ����� ���۰� �ʿ�*/, 0
		, sizeof(SOCKADDR_IN) + 16,
		sizeof(SOCKADDR_IN) + 16,
		&bytesReceived, static_cast<LPOVERLAPPED>(acceptEvent)))
	{
		const int32 errorCode = ::WSAGetLastError();

		if (errorCode != WSA_IO_PENDING)
		{
			//�ƹ��͵� ���ϰ� ������ 
			//�ٽô� RegisterAccept�� ������ �� �� ����.
			//�׷��� �ٽ� RegisterAccept�� �ҷ�����Ѵ�.
			RegisterAccept(acceptEvent);
		}
		
	}

}

//���ô븦 �ٽ� ������ ��Ȳ -> Dispatch�� �ٽ� ���� ��Ȳ
//��⸦ ����� �� ProcessAccept
//acceptEvent��  ��� �����Ѵ�.

void Listener::ProcessAccept(AcceptEvent* acceptEvent)
{
	SessionRef session = acceptEvent->_session;

	//.�� ���ϰ� liseten�� ��������ϳ�.
	if (false == SocketUtils::SetUpdateAcceptSocket(session->GetSocket(),
		_socket))
	{
		RegisterAccept(acceptEvent);
		return;
	}
	SOCKADDR_IN sockAddress;
	int32 sizeofSockeAddress = sizeof(sockAddress);
	if (SOCKET_ERROR == ::getpeername(session->GetSocket()
		, reinterpret_cast<SOCKADDR*>(&sockAddress)
		, &sizeofSockeAddress))
	{
		RegisterAccept(acceptEvent);
		return;
	}
	session->SetNetAddress(NetAddress(sockAddress));
	RegisterAccept(acceptEvent);
}

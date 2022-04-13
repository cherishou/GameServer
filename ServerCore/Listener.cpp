#include "pch.h"
#include "Listener.h"
#include "IocpEvent.h"
#include "SocketUtils.h"
#include "Session.h"

/*
* -------------------------
*		Listener
* -------------------------
*/

Listener::~Listener()
{
	//SocketUtils::Close(_socket);
}

bool Listener::StartACCEPT(NetAddress netAddress)
{
	_socket = SocketUtils::CreateSocket();
	return false;
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

//�����ϴ� ����.
void Listener::RegisterAccept(AcceptEvent* acceptEvent)
{
	//session ������ AcceptEvent�� ������ ��ų ���̴�.
	//���߿� Dispatch���� �ش� iocpEvent �̺�Ʈ�� ������ ���� �� 
	//� session�� �Ѱ���� �� �� �ֱ⿡.
	Session* session = xnew<Session>();
	acceptEvent->Init();
	acceptEvent->SetSession(session);

	DWORD bytesReceived = 0;
	if (false == SocketUtils::AcceptEx(_socket, session->GetSocket(),
		session->_recvBuffer, 0
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
void Listener::ProcessAccept(AcceptEvent* acceptEvent)
{
	SessionRef session = acceptEvent->_session();

	if (false == SocketUtils::SetUpdateAcceptSocket(session->GetSocket(),
		_socket))
	{
		RegisterAccept(acceptEvent);
		return;
	}


	RegisterAccept(acceptEvent);
}

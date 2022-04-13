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

//worker스레드들이 활동하는 곳.
void Listener::Dispatch(IocpEvent* iocpEvent, int32 numOfBytes)
{
	AcceptEvent* acceptEvent = static_cast<AcceptEvent*>(iocpEvent);
	ProcessAccept(acceptEvent);
}

//예약하는 느낌.
void Listener::RegisterAccept(AcceptEvent* acceptEvent)
{
	//session 정보를 AcceptEvent에 연동을 시킬 것이다.
	//나중에 Dispatch한후 해당 iocpEvent 이벤트를 가지고 왔을 때 
	//어떤 session을 넘겼는지 알 수 있기에.
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
			//아무것도 안하고 끝나면 
			//다시는 RegisterAccept를 실행할 수 가 없다.
			//그래서 다시 RegisterAccept를 불러줘야한다.
			RegisterAccept(acceptEvent);
		}
		
	}

}

//낚시대를 다시 던지는 상황 -> Dispatch로 다시 들어가는 상황
//고기를 잡았을 때 ProcessAccept
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

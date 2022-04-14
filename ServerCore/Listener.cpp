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
	//acceptCount만큼 해야하는 이유는 client가 많아질 경우
	//해당하는 갯수만큼 다 처리해보기 위해서.
	//처음에 RegisterAccept가 실행되면 좋지만.
	//그렇지않으면 iocpCore에서 일감이 완료된 것을 처리한다.
	//
	const int32 acceptCount = 1;
	for (int i = 0; i < acceptCount; i++)
	{
		AcceptEvent* acceptEvent = new AcceptEvent();
		//acceptEvent->owner = this;
		//바깥의 owner가 스마트포인터인데 그것을 넣어줄때 어떻게할까?
		//실수1
		//acceptEvent->owner = shared_ptr<IocpObject>(this)
		// shared_ptr<IocpObject>(this)은 refcount가1인 새로운 것을 생성한 것.
		//이것을 해결하기 위해서는 IocpObject가 다른 것을 상속 받아야한다.
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

//worker스레드들이 활동하는 곳.
void Listener::Dispatch(IocpEvent* iocpEvent, int32 numOfBytes)
{
	AcceptEvent* acceptEvent = static_cast<AcceptEvent*>(iocpEvent);
	ProcessAccept(acceptEvent);
}

//iocp에서 처리할 수 있도록 예약하는 느낌.
void Listener::RegisterAccept(AcceptEvent* acceptEvent)
{
	//session 정보를 AcceptEvent에 연동을 시킬 것이다.
	//나중에 Dispatch한후 해당 iocpEvent 이벤트를 가지고 왔을 때 
	//어떤 session을 넘겼는지 알 수 있기에.
	SessionRef session = MakeShared<Session>();
	acceptEvent->Init();
	acceptEvent->_session = session;

	DWORD bytesReceived = 0;
	//AcceptEx 비동기 입출력..
	if (false == SocketUtils::AcceptEx(_socket, session->GetSocket(),
		session->_recvBuffer/*처음연결이 맺어질때 버퍼가 필요*/, 0
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
//acceptEvent를  계속 재사용한다.

void Listener::ProcessAccept(AcceptEvent* acceptEvent)
{
	SessionRef session = acceptEvent->_session;

	//.왜 소켓과 liseten을 맞춰줘야하나.
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

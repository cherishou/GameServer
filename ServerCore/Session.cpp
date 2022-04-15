#include "pch.h"
#include "Session.h"
#include "SocketUtils.h"
#include "Service.h"
#include "CoreMacro.h"

Session::Session() : _recvBuffer(BufferSize)
{
	_socket = SocketUtils::CreateSocket();
}

Session::~Session()
{
	SocketUtils::Close(_socket);
}

void Session::Send(SendBufferRef sendBuffer)
{
	//생각할 문제
	// 1) 버퍼관리? 덮어쓰면 안된다, 해당 버퍼가 wsaSend가 될때까지
	// 유지되어야한다
	// sendEvent 관리? 단일? 여러 개? WSASend 중첩할 것인지..

	// 현재 RegisterSend가 걸리지 않은 상태라면 , 걸어준다.
	WRITE_LOCK;

	_sendQueue.push(sendBuffer);

	//
	if (_sendRegistered.exchange(true) == false)
		RegisterSend();

}

bool Session::Connect()
{
	return RegisterConnect();
}

void Session::Disconnect(const WCHAR* cause)
{
	//_connected가 원래 true였다면 exchange로 인해 false가 
	//되어서 return이 된다.
	if (_connected.exchange(false) == false)
		return;
	
	//temp cout은 wchar로 동작하지 않는다.
	wcout << "Disconnect : " << cause << endl;

	OnDisconnected(); // 컨텐츠 코드에서 오버로딩
	SocketUtils::Close(_socket);
	//ReleaseSession해주어야지 RefCount가 1감소해서 지워질 수 있다.
	GetService()->ReleaseSession(GetSessionRef());
}

HANDLE Session::GetHandle()
{
	return reinterpret_cast<HANDLE>(_socket);
}

void Session::Dispatch(IocpEvent* iocpEvent, int32 numOfBytes)
{
	//iocpEvent가 recv, send를 처리할 경우 이부분에서 처리한다.
	switch (iocpEvent->eventType)
	{
	case EventType::Connect:
		ProcessConnect();
		break;
	case EventType::Recv:
		ProcessRecv(numOfBytes);
		break;
	case EventType::Send:
		ProcessSend(numOfBytes);
		break;
	default:
		break;
	}
}

bool Session::RegisterConnect()
{
	if (IsConnected()) return false;

	//서비스는 클라이언트 타입이어야한다.  그래야 내가 클라에게 붙으니..
	//서버타입은 상대방이 나에게 붙은 느낌 

}

void Session::RegisterRecv()
{
	if (IsConnected() == false) return;

	_recvEvent.Init();
	_recvEvent.owner = shared_from_this();

	WSABUF wsaBuf;
	wsaBuf.buf = reinterpret_cast<char*>(_recvBuffer.writePos());
	wsaBuf.len = _recvBuffer.FreeSize();
}

void Session::RegisterSend()
{
	if (IsConnected() == false) return;

	//나중에 pageLock도 필요.. 언제 어디서든 사용가능하도록?
	WSABUF wsaBuf;
	wsaBuf.buf = (char*)sendEvent->buffer.data();
	wsaBuf.len = (ULONG)sendEvent->buffer.size();

	//Scatter-Gather (흩어져 있는 데이터들을 모아서 한 방에 보낸다)
	vector<WSABUF> wsaBufs;
	wsaBuf.reserve(_sendEvent.sendBuffer.size());
	for (auto sendBuffer : _sendBuffer.sendBuffers)
	{
		WSABUF wsaBuf;
		wsaBuf.buf = reinterpret_cast<char*>(sendBuffer->Buffer());
		wsaBuf.len = static_cast<LONG>(SendBuffer->WriteSize());
		wsaBufs.push_back(wsaBuf);
	}


	DWORD numOfBytes = 0;
	//
	//send는 스레드 safe하지않아서 순서를 보장해줘야한다.
	if (SOCKET_ERROR == ::WSASend(_socket, wsaBufs.data(), static_cast<DWORD>(wsaBufs.size()),
		&numOfBytes, 0, sendEvent, nullptr))
	{
		int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			HandleError(errorCode);
			sendEvent->owner = nullptr; //Release_ref
			xdelete(sendEvent);
		}
	}
}

//한번에 한번의 ProcessRecv가 발생, 멀티스레드환경이라도 여러개 동시에 실행되지않는다.
void Session::ProcessRecv(int32 numOfBytes)
{
	_recvEvent.owner = nullptr; // RELEASE_REF

	if (numOfBytes == 0)
	{
		Disconnect(L"Recv0");
		return;
	}

	if (_recvBuffer.OnWrite(numOfBytes) == false)
	{
		Disconnect(L"OnWrite Overflow");
		return;
	}
	
	int32 dataSize = _recvBuffer.DataSize();

	//processLen를 보이는 이유는 모든 길이를 처리했다는 의미로 보면된다.
	//processLen는 처리한 데이터의 갯수. 그후 해당크기만큼 readPos를 옮기자.
	int32 processLen =  OnRecv(_recvBuffer.readPos(), dataSize);

	if (processLen < 0 || dataSize < 0 || _recvBuffer.OnRead(processLen) == false)
	{
		Disconnect(L"OnRead Overflow");
		return;
	}

	//커서 정리
	_recvBuffer.Clean();

	//수신 등록  (ProcessRecv로 처리한 후 , 그후에 또다시 받은 데이터가 있으면 처리하러가야한다.
	RegisterRecv();

}

//ProcessSend 과정에서는 dispath가 일어나서 completionportqueue에서 일감들이 실행되는데
//그곳에서 순서가 보장되지않는다.
void Session::ProcessSend(int32 numOfBytes)
{
	_sendEvent.owner = nullptr; //release_ref
	_sendEvent.sendBuffers.clear();

	if (numOfBytes == 0)
	{
		Disconnect(L"Send 0");
		return;
	}

	//컨텐츠 코드에서 재정의
	OnSend(numOfBytes);

	WRITE_LOCK;
	//보낼 데이터를 다 보냈다.
	if (_sendQueue.empty()) _sendRegistered.store(false);
	else RegisterSend();  //완료하기전에 새로운 데이터들이 들어와있다.
}

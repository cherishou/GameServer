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
	//������ ����
	// 1) ���۰���? ����� �ȵȴ�, �ش� ���۰� wsaSend�� �ɶ�����
	// �����Ǿ���Ѵ�
	// sendEvent ����? ����? ���� ��? WSASend ��ø�� ������..

	// ���� RegisterSend�� �ɸ��� ���� ���¶�� , �ɾ��ش�.
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
	//_connected�� ���� true���ٸ� exchange�� ���� false�� 
	//�Ǿ return�� �ȴ�.
	if (_connected.exchange(false) == false)
		return;
	
	//temp cout�� wchar�� �������� �ʴ´�.
	wcout << "Disconnect : " << cause << endl;

	OnDisconnected(); // ������ �ڵ忡�� �����ε�
	SocketUtils::Close(_socket);
	//ReleaseSession���־���� RefCount�� 1�����ؼ� ������ �� �ִ�.
	GetService()->ReleaseSession(GetSessionRef());
}

HANDLE Session::GetHandle()
{
	return reinterpret_cast<HANDLE>(_socket);
}

void Session::Dispatch(IocpEvent* iocpEvent, int32 numOfBytes)
{
	//iocpEvent�� recv, send�� ó���� ��� �̺κп��� ó���Ѵ�.
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

	//���񽺴� Ŭ���̾�Ʈ Ÿ���̾���Ѵ�.  �׷��� ���� Ŭ�󿡰� ������..
	//����Ÿ���� ������ ������ ���� ���� 

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

	//���߿� pageLock�� �ʿ�.. ���� ��𼭵� ��밡���ϵ���?
	WSABUF wsaBuf;
	wsaBuf.buf = (char*)sendEvent->buffer.data();
	wsaBuf.len = (ULONG)sendEvent->buffer.size();

	//Scatter-Gather (����� �ִ� �����͵��� ��Ƽ� �� �濡 ������)
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
	//send�� ������ safe�����ʾƼ� ������ ����������Ѵ�.
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

//�ѹ��� �ѹ��� ProcessRecv�� �߻�, ��Ƽ������ȯ���̶� ������ ���ÿ� ��������ʴ´�.
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

	//processLen�� ���̴� ������ ��� ���̸� ó���ߴٴ� �ǹ̷� ����ȴ�.
	//processLen�� ó���� �������� ����. ���� �ش�ũ�⸸ŭ readPos�� �ű���.
	int32 processLen =  OnRecv(_recvBuffer.readPos(), dataSize);

	if (processLen < 0 || dataSize < 0 || _recvBuffer.OnRead(processLen) == false)
	{
		Disconnect(L"OnRead Overflow");
		return;
	}

	//Ŀ�� ����
	_recvBuffer.Clean();

	//���� ���  (ProcessRecv�� ó���� �� , ���Ŀ� �Ǵٽ� ���� �����Ͱ� ������ ó���Ϸ������Ѵ�.
	RegisterRecv();

}

//ProcessSend ���������� dispath�� �Ͼ�� completionportqueue���� �ϰ����� ����Ǵµ�
//�װ����� ������ ��������ʴ´�.
void Session::ProcessSend(int32 numOfBytes)
{
	_sendEvent.owner = nullptr; //release_ref
	_sendEvent.sendBuffers.clear();

	if (numOfBytes == 0)
	{
		Disconnect(L"Send 0");
		return;
	}

	//������ �ڵ忡�� ������
	OnSend(numOfBytes);

	WRITE_LOCK;
	//���� �����͸� �� ���´�.
	if (_sendQueue.empty()) _sendRegistered.store(false);
	else RegisterSend();  //�Ϸ��ϱ����� ���ο� �����͵��� �����ִ�.
}

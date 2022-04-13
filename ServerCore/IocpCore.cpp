#include "pch.h"
#include "IocpCore.h"
#include "IocpEvent.h"
IocpCore::IocpCore()
{
	_iocpHandle = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);


}

IocpCore::~IocpCore()
{
	//::CloseHanlde();
}

bool IocpCore::Register(IocpObject* iocpObject)
{
	return ::CreateIoCompletionPort(iocpObject->GetHandle(), _iocpHandle,
		reinterpret_cast<ULONG_PTR>(iocpObject), 0);
}

bool IocpCore::Dispatch(uint32 timeoutMs)
{
	DWORD numOfBytes = 0;
	ULONG_PTR key = 0;
	IocpEvent* iocpEvent = nullptr;

	if (::GetQueuedCompletionStatus(_iocpHandle, &numOfBytes,
		&key, reinterpret_cast<LPOVERLAPPED*>(&iocpEvent)
		, timeoutMs))
	{
		IocpObjectRef iocpObject = iocpEvent->owner;
		iocpObject->Dispatch(iocpEvent, numOfBytes);
	}
	else
	{
		//GetQueuedCompletionStatus의 time이 무한이 아니라면 시간이 다 되었을 경우
		//else 문으로 들어온다
		int32 errorCode = ::WSAGetLastError();
		switch (errorCode)
		{
		case WAIT_TIMEOUT:
			return false;
		default:
			IocpObjectRef iocpObject = iocpEvent->owner;
			iocpObject->Dispatch(iocpEvent, numOfBytes);
			break;
		}
	}


	return true;
}

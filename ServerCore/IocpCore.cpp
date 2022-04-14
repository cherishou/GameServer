#include "pch.h"
#include "IocpCore.h"
#include "IocpEvent.h"


IocpCore GIocpCore;

IocpCore::IocpCore()
{
	_iocpHandle = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
}

IocpCore::~IocpCore()
{
	::CloseHandle(_iocpHandle);
}

bool IocpCore::Register(IocpObjectRef iocpObject)
{
	//일반적인 상황에서 iocpObject->GetHandle()은 소켓이다.
	//세션을 넣어줄 수도 있고, 소켓을 넣어줄 수도있고, 
	//네트워크도 아니어도 사용할 수 있도록 광범위하게 만들어주기 위해서
	//iocpObject로 관리한다
	return ::CreateIoCompletionPort(iocpObject->GetHandle(), _iocpHandle,
		/*key*/0, 0);
}
//workerThread가 dispach를 실행해서 일감이 있는지 확인한다.
// 등록을할 때 레퍼런스 카운팅이 들어가서 ,날아가면안된다.
bool IocpCore::Dispatch(uint32 timeoutMs)
{
	DWORD numOfBytes = 0; //전송된 바이트
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

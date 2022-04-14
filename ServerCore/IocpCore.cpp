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
	//�Ϲ����� ��Ȳ���� iocpObject->GetHandle()�� �����̴�.
	//������ �־��� ���� �ְ�, ������ �־��� �����ְ�, 
	//��Ʈ��ũ�� �ƴϾ ����� �� �ֵ��� �������ϰ� ������ֱ� ���ؼ�
	//iocpObject�� �����Ѵ�
	return ::CreateIoCompletionPort(iocpObject->GetHandle(), _iocpHandle,
		/*key*/0, 0);
}
//workerThread�� dispach�� �����ؼ� �ϰ��� �ִ��� Ȯ���Ѵ�.
// ������� �� ���۷��� ī������ ���� ,���ư���ȵȴ�.
bool IocpCore::Dispatch(uint32 timeoutMs)
{
	DWORD numOfBytes = 0; //���۵� ����Ʈ
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
		//GetQueuedCompletionStatus�� time�� ������ �ƴ϶�� �ð��� �� �Ǿ��� ���
		//else ������ ���´�
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

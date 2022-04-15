#include "pch.h"
#include "RecvBuffer.h"

RecvBuffer::RecvBuffer(int32 bufferSize)
{
}

RecvBuffer::~RecvBuffer()
{
}

void RecvBuffer::Clean()
{
	int32 dataSize = DataSize();
	if (dataSize == 0)
	{
		//현재 읽기 쓰기 위치가 동일하다면 둘 다 리셋
		_readPos = _writePos = 0;
	}
	else
	{
		//여유공간이 버퍼 1개 크기 미만이면 데이터를 앞으로 당긴다.
		if (FreeSize() < _bufferSize)
		{
			::memcpy(&_buffer[0], &_buffer[_readPos], dataSize);
			_readPos = 0;
			_writePos = dataSize;
		}
	}
}

bool RecvBuffer::OnRead(int32 numOfBytes)
{
	return false;
}

bool RecvBuffer::OnWrite(int32 numOfBytes)
{
	return false;
}

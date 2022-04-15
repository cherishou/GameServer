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
		//���� �б� ���� ��ġ�� �����ϴٸ� �� �� ����
		_readPos = _writePos = 0;
	}
	else
	{
		//���������� ���� 1�� ũ�� �̸��̸� �����͸� ������ ����.
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

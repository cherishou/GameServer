#pragma once
#include <vector>

/*
* TCP���� ���ۻ���� ������ ũ��� �س����� �ȵǴ� ������
* TCP���� ����� ������ ��� ���ϴ� �����Ͱ� ����� �� �� �� ����
* �ֱ⶧���̴�.
*/
/*
* read, write�� ������ �������� ���� read�� ��ġ�� write�� ��ġ�� ����Ű��
* ���̴�.
* ť�� �����̴� ó����ġ�� read write �����ϴ�.
* [r][][][][][][w][][][][]
* w�� r���̰� ���� �� �� �ִ� ���̶u���̴�.
* w���Ŀ��� w�� �� �ִ� �����̴�.
*/
class RecvBuffer
{
	///BUFFER_COUNT�� 10�踦 �÷��شٸ� �������� �� �� �� �ִ�.
	enum { BUFFER_COUNT = 10 };
public:
	RecvBuffer(int32 bufferSize);
	~RecvBuffer();

	//rw�� ���� ��ġ�� �ִٸ� �ƹ��� �����Ͱ� ���ٴ� ���̶�
	//��Ŀ���� ù��ġ�� �ű��.
	//���ڶ��̶� �� ������ ���ٸ� ���� �����͸� �����ϸ鼭 
	//Ŀ���� �̵��Ѵ� ù�κ�����
	void		Clean();
	bool		OnRead(int32 numOfBytes);
	bool		OnWrite(int32 numOfBytes);


	BYTE* readPos() { return &_buffer[_readPos]; }
	BYTE* writePos() { return &_buffer[_writePos]; }
	int32 DataSize() { return _writePos - _readPos; }
	//��밡���� ����
	int32 FreeSize() { return _bufferSize - _writePos; }

private:
	int32 _bufferSize = 0;
	int32 _writePos = 0;
	int32 _readPos = 0;
	vector<BYTE> _buffer;
};


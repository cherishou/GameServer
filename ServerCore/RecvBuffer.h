#pragma once
#include <vector>

/*
* TCP에서 버퍼사이즈를 고정된 크기로 해놓으면 안되는 이유가
* TCP에는 경계의 개념이 없어서 원하는 데이터가 제대로 다 안 올 수도
* 있기때문이다.
*/
/*
* read, write는 포인터 느낌으로 현재 read할 위치랑 write할 위치를 가리키는
* 것이다.
* 큐의 느낌이다 처음위치는 read write 동일하다.
* [r][][][][][][w][][][][]
* w과 r차이가 현재 쓸 수 있는 데이턷르이다.
* w이후에는 w할 수 있는 공간이다.
*/
class RecvBuffer
{
	///BUFFER_COUNT로 10배를 늘려준다면 복사비용을 줄 일 수 있다.
	enum { BUFFER_COUNT = 10 };
public:
	RecvBuffer(int32 bufferSize);
	~RecvBuffer();

	//rw가 같은 위치에 있다면 아무런 데이터가 없다는 것이라서
	//두커서를 첫위치로 옮긴다.
	//끝자락이라서 쓸 공간이 없다면 현재 데이터를 유지하면서 
	//커서를 이동한다 첫부분으로
	void		Clean();
	bool		OnRead(int32 numOfBytes);
	bool		OnWrite(int32 numOfBytes);


	BYTE* readPos() { return &_buffer[_readPos]; }
	BYTE* writePos() { return &_buffer[_writePos]; }
	int32 DataSize() { return _writePos - _readPos; }
	//사용가능한 범위
	int32 FreeSize() { return _bufferSize - _writePos; }

private:
	int32 _bufferSize = 0;
	int32 _writePos = 0;
	int32 _readPos = 0;
	vector<BYTE> _buffer;
};


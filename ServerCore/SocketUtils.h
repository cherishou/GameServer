#pragma once
#include "NetAddress.h"

//소켓프로그래밍을 할때 윈솟 초기화랑
//비동기에 connect disconnect는 런타임에 주소를 얻어와햐ㅏ는데 그부분을 처리

/*
* 소켓설정관련 함수들을 랩핑해놓고 편하게 사용도록한다.
* ------------------------
*		SocketUtils
* WSADATA?
* ------------------------
*/
class SocketUtils
{
public:
	static LPFN_CONNECTEX		ConnectEx;
	static LPFN_DISCONNECTEX	DisconnectEx;
	static LPFN_ACCEPTEX		AcceptEx;
public:
	static void Init();
	static void clear();


	static bool BindWindowsFunction(SOCKET socket, GUID guid, LPVOID* fn);
	static SOCKET CreateSocket();


	static bool SetLinger(SOCKET socket, uint16 onoff, uint16 linger);
	static bool SetReuseAddress(SOCKET socket, bool flag);
	static bool SetRecvBufferSize(SOCKET socket, int32 size);
	static bool SetSendBufferSize(SOCKET socket, int32 size);
	static bool SetTcpNoDelay(SOCKET socket, bool flag);
	static bool SetUpdateAcceptSocket(SOCKET socket, SOCKET listenSocket);


	// BOLL로하는 이유는 계속해서 성공여부체크를 위해서
	static bool Bind(SOCKET socket, NetAddress netAddr);
	
	//socket에 임의의 주소를 맵핑
	static bool BindAnyAddress(SOCKET socket, uint16 port);
	static bool Listen(SOCKET socket, int32 backlog = SOMAXCONN);
	//소켓을닫아주는 함수 
	static void Close(SOCKET& socket);
};

//setSocketOption을 반복하게 사용하는 것을 위해
template<typename T>
static inline bool SetSockOpt(SOCKET socket, int32 level, int32 optName, T optVal)
{
	return SOCKET_ERROR !=::setsockopt(socket, level, optName, reinterpret_cast<char*>(&optVal), sizeof(optVal));

}


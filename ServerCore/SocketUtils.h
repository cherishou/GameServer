#pragma once
#include "NetAddress.h"

//�������α׷����� �Ҷ� ���� �ʱ�ȭ��
//�񵿱⿡ connect disconnect�� ��Ÿ�ӿ� �ּҸ� �����᤿�µ� �׺κ��� ó��

/*
* ���ϼ������� �Լ����� �����س��� ���ϰ� ��뵵���Ѵ�.
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


	// BOLL���ϴ� ������ ����ؼ� ��������üũ�� ���ؼ�
	static bool Bind(SOCKET socket, NetAddress netAddr);
	
	//socket�� ������ �ּҸ� ����
	static bool BindAnyAddress(SOCKET socket, uint16 port);
	static bool Listen(SOCKET socket, int32 backlog = SOMAXCONN);
	//�������ݾ��ִ� �Լ� 
	static void Close(SOCKET& socket);
};

//setSocketOption�� �ݺ��ϰ� ����ϴ� ���� ����
template<typename T>
static inline bool SetSockOpt(SOCKET socket, int32 level, int32 optName, T optVal)
{
	return SOCKET_ERROR !=::setsockopt(socket, level, optName, reinterpret_cast<char*>(&optVal), sizeof(optVal));

}


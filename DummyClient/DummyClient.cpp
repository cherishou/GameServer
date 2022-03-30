#include "pch.h"
#include <iostream>

#include <WinSock2.h>
#include <MSWSock.h>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32.lib")
int main()
{
	WSADATA wsaData;
	if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		return 0;

	// ad : Address Family (AF_INET = IPv4, AF_INET6 = IPv6)
	// type : TCP(SOCK_STREAM) VS UDP(SOCK_DGRAM)
	// protocol : 0
	// return : descriptor
	SOCKET clientSocket = ::socket(AF_INET, SOCK_DGRAM, 0);
	if (clientSocket == INVALID_SOCKET)
	{
		int32 errorCode = ::WSAGetLastError();

		cout << "Socket ErrorCode : " << errorCode << endl;
		return 0;
	}

	SOCKADDR_IN serverAddr;
	::memset(&serverAddr, 0, sizeof(serverAddr));

	serverAddr.sin_family = AF_INET;
	::inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);

	serverAddr.sin_port = ::htons(7777);   // port 포트 번호 
	

	if (::connect(clientSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
	{
		int32 errorCode = ::WSAGetLastError();

		cout << "Connect ErrorCode : " << errorCode << endl;
	}

	cout << "Connected To Server" << endl;

	while (true)
	{
		char sendBuffer[100] = "Hello world";
		
		for (int i = 0; i < 10; i++)
		{
			int32 resultCode = ::sendto(clientSocket, sendBuffer, sizeof(sendBuffer), 0, (sockaddr*)&serverAddr, sizeof(serverAddr));
			cout << "Send Data Len : " << sizeof(sendBuffer) << endl;
		}
		
		

		//-------------------------server에서 보낸 것을 recv

		//char recvBuffer[1000];

		//int recvLen = ::recv(clientSocket, recvBuffer, sizeof(recvBuffer), 0);

		////SOCKET_ERROR가 -1인데 0보다 작으면 에러라고 보자!
		//if (recvLen <= 0)
		//{
		//	int32 errCode = ::WSAGetLastError();
		//	cout << "Rcv ErrorCode : " << errCode << endl;
		//	return 0;
		//}
		//cout << "Rcv Data : " << recvBuffer << endl;

		//this_thread::sleep_for(1s);

	}
	::closesocket(clientSocket);

	::WSACleanup();
}
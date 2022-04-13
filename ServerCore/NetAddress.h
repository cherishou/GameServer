#pragma once
/*
* ----------------------
*		NetAddress
* ----------------------
*/
// Ŭ���̾�Ʈ�� ip�ּҸ� �����ؾ��ϴ� ��찡 �ִ� �Ź� �׷����ϸ� ���ŷӴ�.
// ��� �ּҸ� NetAddress�� �����ϸ鼭 ���

class NetAddress
{
public:
	NetAddress() = default;
	NetAddress(SOCKADDR_IN sockAddr);
	NetAddress(wstring ip, uint16 port);

	SOCKADDR_IN& GetSockAddr() { return _sockAddr; }
	wstring		GetIpAddress();
	uint16		GetPort() { return ::ntohs(_sockAddr.sin_port); }
public:
	//ip�ּҸ� �̿��ؼ� IN_ADDR�� ��ȯ  (helper ���)
	static IN_ADDR ip2Address(const WCHAR* ip);
private:
	SOCKADDR_IN _sockAddr = {};
};


#pragma once
/*
* ----------------------
*		NetAddress
* ----------------------
*/
// 클라이언트의 ip주소를 추출해야하는 경우가 있다 매번 그렇게하면 번거롭다.
// 모든 주소를 NetAddress로 관리하면서 사용

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
	//ip주소를 이용해서 IN_ADDR로 반환  (helper 기능)
	static IN_ADDR ip2Address(const WCHAR* ip);
private:
	SOCKADDR_IN _sockAddr = {};
};


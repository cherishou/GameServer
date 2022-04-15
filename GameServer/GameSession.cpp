#include "pch.h"
#include "GameSession.h"
#include "GameSessionManager.h"

void GameSession::OnConnected()
{
	GSessionManager.Add(static_pointer_cast<GameSession>(shared_from_this()));
}

void GameSession::OnDisconnected()
{
	GSessionManager.Remove(static_pointer_cast<GameSession>(shared_from_this()));
}

int GameSession::OnRecv(BYTE* buffer, int32 len)
{
	cout << "OnRecv Len = " << len << endl;
	SendBufferRef sendBuffer = MakeShared<SendBuffer>(4096);
	sendBuffer->CopyData(buffer, len);
	/*
	* Broadcast를 하는 것은 현재 상황을 주변으로 다 퍼뜨리는 것이다.
	* 
	*/
	GSessionManager.Broadcast(sendBuffer);

	return len;
}

void GameSession::OnSend(int32 len)
{
}

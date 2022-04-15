#include "pch.h"
#include "GameSessionManager.h"
#include "GameSession.h"

void GameSessionManager::Add(GameSessionRef session)
{
	WRITE_LOCK;
	_sessions.insert(session);
}

void GameSessionManager::Remove(GameSessionRef session)
{
	WRITE_LOCK;
	_sessions.erase(session);
}

//세션들에게 동일한 데이터들을 보내겠다는 의미.
//sendBuffer를 받아서, 브로드캐스트를 걸어준다.
void GameSessionManager::Broadcast(SendBufferRef sendBuffer)
{
	WRITE_LOCK;
	for (auto session : _sessions)
	{
		session->Send(sendBuffer);
	}
}

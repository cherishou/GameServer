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

//���ǵ鿡�� ������ �����͵��� �����ڴٴ� �ǹ�.
//sendBuffer�� �޾Ƽ�, ��ε�ĳ��Ʈ�� �ɾ��ش�.
void GameSessionManager::Broadcast(SendBufferRef sendBuffer)
{
	WRITE_LOCK;
	for (auto session : _sessions)
	{
		session->Send(sendBuffer);
	}
}

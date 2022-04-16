#include "pch.h"
#include "DeadLockProfiler.h"

void DeadLockProfiler::PushLock(const char* name)
{
	LockGuard guard(_lock);

	//아이디를 찾거나 발급한다.
	int32	lockId = 0;
	auto findIt = _nameToId.find(name);
	if (findIt == _nameToId.end())
	{
		lockId = static_cast<int32>(_nameToId.size());
		_nameToId[name] = lockId;
		_IdToName[lockId] = name;
	}
	else
	{
		lockId = findIt->second;
	}

	//잡고 있는 락이 있었다면.
	if (!_lockStack.empty())
	{
		//기존에 발견되지 않은 케이스라면 데드락 여부 다시 확인한다.
		const int32 prevId = _lockStack.top();
		if (lockId != prevId)
		{
			set<int32>& history = _lockHistory[prevId];
			if (history.find(lockId) == history.end())
			{
				history.insert(lockId);
				//새로운 길을 발견하면 사이클을 체크해본다.
				CheckCycle();
			}
		}
	}

	_lockStack.push(lockId);
}

void DeadLockProfiler::PopLock(const char* name)
{
	LockGuard guard(_lock);

	_lockStack.pop();
}

void DeadLockProfiler::CheckCycle()
{
	const int32 lockCount = static_cast<int32>(_nameToId.size());
	_discoveredOrder = vector<int32>(lockCount, -1);
	_finished = vector<bool>(lockCount, -1);
	_parent = vector<int32>(lockCount, -1);

	for (int32 lockId = 0; lockId < lockCount; lockId++)
		Dfs(lockId);

	_discoveredOrder.clear();
	_finished.clear();
	_parent.clear();
}

void DeadLockProfiler::Dfs(int32 here)
{
	if (_discoveredOrder[here] != -1) return;

	_discoveredOrder[here] = _discoveredCount++;

	//모든 인접한 정점을 순회 
	auto findIt = _lockHistory.find(here);
	if (findIt == _lockHistory.end())
	{
		//인접한 정점이 없다면 끝
		_finished[here] = true;
		return;
	}

	//here가 다른 lock을 잡은 적이 있다.
	set<int32>& nextSet = findIt->second;
	for (int32 there : nextSet)
	{
		//아직 방문한적이 없다면 방문한다.
		if (_discoveredOrder[there] == -1)
		{
			_parent[there] = here;
			Dfs(there);
			continue;
		}

		//here가 there보다 먼저 발견되었으면 there는 here의 후손이다. 순방향
		if (_discoveredOrder[here] < _discoveredOrder[there]) continue;

		//순방향 간선이 아니고 dfs가 종료되지않았다면 역방향이다.
		if (_finished[there] == false)
		{
			CRASH("DEADLOCK_DETECTED");
		}
	}
}

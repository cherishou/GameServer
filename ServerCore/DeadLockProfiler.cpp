#include "pch.h"
#include "DeadLockProfiler.h"

void DeadLockProfiler::PushLock(const char* name)
{
	LockGuard guard(_lock);

	//���̵� ã�ų� �߱��Ѵ�.
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

	//��� �ִ� ���� �־��ٸ�.
	if (!_lockStack.empty())
	{
		//������ �߰ߵ��� ���� ���̽���� ����� ���� �ٽ� Ȯ���Ѵ�.
		const int32 prevId = _lockStack.top();
		if (lockId != prevId)
		{
			set<int32>& history = _lockHistory[prevId];
			if (history.find(lockId) == history.end())
			{
				history.insert(lockId);
				//���ο� ���� �߰��ϸ� ����Ŭ�� üũ�غ���.
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

	//��� ������ ������ ��ȸ 
	auto findIt = _lockHistory.find(here);
	if (findIt == _lockHistory.end())
	{
		//������ ������ ���ٸ� ��
		_finished[here] = true;
		return;
	}

	//here�� �ٸ� lock�� ���� ���� �ִ�.
	set<int32>& nextSet = findIt->second;
	for (int32 there : nextSet)
	{
		//���� �湮������ ���ٸ� �湮�Ѵ�.
		if (_discoveredOrder[there] == -1)
		{
			_parent[there] = here;
			Dfs(there);
			continue;
		}

		//here�� there���� ���� �߰ߵǾ����� there�� here�� �ļ��̴�. ������
		if (_discoveredOrder[here] < _discoveredOrder[there]) continue;

		//������ ������ �ƴϰ� dfs�� ��������ʾҴٸ� �������̴�.
		if (_finished[there] == false)
		{
			CRASH("DEADLOCK_DETECTED");
		}
	}
}

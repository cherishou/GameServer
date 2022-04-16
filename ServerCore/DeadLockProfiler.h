#pragma once
#include <stack>
#include <map>
#include <vector>

class DeadLockProfiler
{
public:
	void PushLock(const char* name);
	void PopLock(const char* name);
	void CheckCycle();

private:
	void Dfs(int32 here);

private:
	unordered_map<const char* , int32 > _nameToId;
	unordered_map<int32 , const char*> _IdToName;
	stack<int>							_lockStack; //락의 상황을 판별.
	//어떤 lock이 몇번 몇번 lock을 잡았는지 다 판별하는 것.
	//ex : 0이 1번을 lock을 잡았따는 것을 정보를 담아 두는 것.
	map<int32, set<int32>>				_lockHistory;

	Mutex	_lock;

private:
	vector<int32>	_discoveredOrder; //노드가 발견된 순서를 기록
	int32			_discoveredCount = 0;// 노드가 발견된 순서
	vector<bool>	_finished; //dfs(i)가 종료되었는지 여부
	vector<int32>	_parent; //역 추적 배열 느낌 
};


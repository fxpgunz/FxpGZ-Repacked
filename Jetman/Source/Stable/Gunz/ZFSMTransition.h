#pragma once
#include "ZFSMCondition.h"

class ZFSMState;

// 상태전이의 조건,다음상태의 쌍을 나타내는 클래스

class ZFSMTransition
{
protected:
	typedef vector<ZFSMCondition*>	VecCondition;
	typedef VecCondition::iterator	ItorCondition;

	VecCondition m_vecCondition;

	ZFSMState* m_pNextState;
	string m_strNextState;

public:
	ZFSMTransition();
	~ZFSMTransition();

	void AddCondition(ZFSMCondition* pCond) { m_vecCondition.push_back(pCond); }
	ZFSMCondition* GetCondition(int i) { return m_vecCondition[i]; }
	int CountCondition() { return (int)m_vecCondition.size(); }
	ZFSMCondition* FindCondition(E_FSMCOND_ID condId);

	void SetNextState(ZFSMState* pState) { m_pNextState = pState; }
	ZFSMState* GetNextState() { return m_pNextState; }

	void SetNextStateName(const char* sz) { m_strNextState=sz; }
	const char* GetNextStateName() { return m_strNextState.c_str(); }

	bool Check(IFSMConditionSource* pSrc);
};
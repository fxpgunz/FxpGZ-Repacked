#include "stdafx.h"
#include "ZFSMTransition.h"
#include "ZFSMState.h"


ZFSMTransition::ZFSMTransition()
: m_pNextState(NULL)
{

}

ZFSMTransition::~ZFSMTransition()
{
	int n = (int)m_vecCondition.size();
	for (int i=0; i<n; ++i)
		delete m_vecCondition[i];
}

ZFSMCondition* ZFSMTransition::FindCondition(E_FSMCOND_ID condId)
{
	int n = (int)m_vecCondition.size();
	for (int i=0; i<n; ++i)
		if (condId == m_vecCondition[i]->GetConditionId())
			return m_vecCondition[i];
	
	return NULL;
}

bool ZFSMTransition::Check(IFSMConditionSource* pSrc)
{
	if (!m_pNextState)
		return false;

	DWORD dwCurrTime = timeGetTime();

	// 타겟 상태의 쿨타임부터 확인한다
	if (!pSrc->Cond_CheckStateCooltime(m_pNextState, dwCurrTime))
		return false;

	int n = (int)m_vecCondition.size();
	for (int i=0; i<n; ++i)
	{
		if (!m_vecCondition[i]->Check(pSrc))
			return false;
	}
	return true;
}
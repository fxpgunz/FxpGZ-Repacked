#include "stdafx.h"
#include "ZFSMStateEnteredTimeStamper.h"
#include "ZFSMState.h"


bool ZFSMStateEnteredTimeStamper::CheckCooltime(ZFSMState* pState, DWORD dwCurrTime)
{
	ItorStateLastEnteredTime it = m_mapStateLastEnteredTime.find(pState);
	
	// 한번도 진입한 적이 없는 상태인 경우
	if (it == m_mapStateLastEnteredTime.end())
		return true;

	DWORD dwElapsedSinceLastEnterd = dwCurrTime - it->second;
	if (dwElapsedSinceLastEnterd > (DWORD)pState->GetCooltime())
		return true;

	return false;
}

void ZFSMStateEnteredTimeStamper::SetEnteredTime(ZFSMState* pState, DWORD dwCurrTime)
{
	m_mapStateLastEnteredTime[pState] = dwCurrTime;
}

DWORD ZFSMStateEnteredTimeStamper::GetEnteredTime( ZFSMState* pState )
{
	ItorStateLastEnteredTime it = m_mapStateLastEnteredTime.find(pState);
	if (it != m_mapStateLastEnteredTime.end())
		return it->second;

	// 만약 없는 경우 어떻게 하는 것이 올바른 것일까? return 0는 위험해보인다..
	_ASSERT(0);
	return 0;
}
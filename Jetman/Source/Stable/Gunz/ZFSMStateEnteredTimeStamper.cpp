#include "stdafx.h"
#include "ZFSMStateEnteredTimeStamper.h"
#include "ZFSMState.h"


bool ZFSMStateEnteredTimeStamper::CheckCooltime(ZFSMState* pState, DWORD dwCurrTime)
{
	ItorStateLastEnteredTime it = m_mapStateLastEnteredTime.find(pState);
	
	// �ѹ��� ������ ���� ���� ������ ���
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

	// ���� ���� ��� ��� �ϴ� ���� �ùٸ� ���ϱ�? return 0�� �����غ��δ�..
	_ASSERT(0);
	return 0;
}
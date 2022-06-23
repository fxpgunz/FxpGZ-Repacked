#include "stdafx.h"
#include "ZFSM.h"
#include "ZFSMState.h"
#include "ZFSMTransition.h"


ZFSM::~ZFSM()
{
	for (ItorState it=m_mapState.begin(); it!=m_mapState.end(); ++it)
		delete it->second;
}

void ZFSM::AddState( ZFSMState* pState )
{
	if (m_mapState.end() != m_mapState.find(pState->GetName()))
	{
		_ASSERT(0);
		return;
	}
	m_mapState[pState->GetName()] = pState;
}

ZFSMState* ZFSM::GetState( const char* szName )
{
	ItorState it = m_mapState.find(szName);
	if (it != m_mapState.end())
		return it->second;
	
	return NULL;
}

// ZFSMTransition 에는 nextState의 이름만 들어있다. 실제로 FSM을 구동하려면
// nextState의 포인터를 찾아 연결해 주어야 한다. 그것을 해주는 함수
void ZFSM::PrepareTransNextState()
{
	ZFSMState* pState;
	ZFSMTransition* pTrans;

	for (ItorState it=m_mapState.begin(); it!=m_mapState.end(); ++it)
	{
		pState = it->second;
		
		int numTrans = pState->GetTransitionCount();
		for (int i=0; i<numTrans; ++i)
		{
			pTrans = pState->GetTransition(i);
			
			ZFSMState* pNextState = GetState( pTrans->GetNextStateName());
			if (!pNextState)
			{
				mlog("ERROR: PrepareTransNextState cant find next state. \"%s\"\n", pTrans->GetNextStateName());
				_ASSERT(0);
				continue;
			}
			
			pTrans->SetNextState(pNextState);
		}
	}
}
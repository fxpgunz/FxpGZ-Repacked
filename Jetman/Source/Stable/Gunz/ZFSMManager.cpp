#include "stdafx.h"
#include "ZFSMManager.h"
#include "ZFSM.h"


ZFSMManager::~ZFSMManager()
{
	for (ItorFsm it=m_mapFSM.begin(); it!=m_mapFSM.end(); ++it)
		delete it->second;
}

void ZFSMManager::AddFsm(ZFSM** arrayFsm, unsigned int count)
{
	ItorFsm it;
	for (unsigned int i=0; i<count; ++i)
	{
		it = m_mapFSM.find(arrayFsm[i]->GetName());
		if (it == m_mapFSM.end())
		{
			m_mapFSM[arrayFsm[i]->GetName()] = arrayFsm[i];
		}
		else
		{
			mlog("ERROR : fsm manager add fsm name duplicated.\n");
			_ASSERT(0);
		}
	}
}

ZFSM* ZFSMManager::GetFsm( const char* szName )
{
	ItorFsm it = m_mapFSM.find( szName );
	if (it != m_mapFSM.end())
		return it->second;
	return NULL;
}
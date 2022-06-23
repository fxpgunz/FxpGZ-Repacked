#include "stdafx.h"
#include "MNewQuestPlayerManager.h"

void MNewQuestPlayerManager::AddPlayer(MMatchObject* pObject)
{
	if (!pObject) { _ASSERT(0); return; }
	if (pObject->GetUID().IsInvalid())  { _ASSERT(0); return; }

	m_mapPlayer[pObject->GetUID()] = new MNewQuestPlayer(pObject);
}

void MNewQuestPlayerManager::RemovePlayer(MUID uid)
{
	ItorPlayer it = m_mapPlayer.find(uid);
	if (it == m_mapPlayer.end())
	{
		_ASSERT(0); return;
	}

	delete it->second;
	m_mapPlayer.erase(it);
}

bool MNewQuestPlayerManager::FindPlayer(MUID uid)
{
	auto itor = m_mapPlayer.find(uid);
	if (itor != m_mapPlayer.end())
		return true;
	
	return false;
}

void MNewQuestPlayerManager::IncreaseNpcControl( MUID uidController, MUID uidNpc )
{
	ItorPlayer it = m_mapPlayer.find(uidController);
	if (it == m_mapPlayer.end())
		return;

	it->second->IncreaseNpcControl();
}

void MNewQuestPlayerManager::DecreaseNpcControl( MUID uidController )
{
	ItorPlayer it = m_mapPlayer.find(uidController);
	if (it == m_mapPlayer.end())
		return;

	it->second->DecreaseNpcControl();
}

MUID MNewQuestPlayerManager::FindSuitableController()
{
	int numLeast = 1000;
	MUID uidLeast;
	uidLeast.SetInvalid();
	for (ItorPlayer it=m_mapPlayer.begin(); it!=m_mapPlayer.end(); ++it)
	{
		if (numLeast > it->second->GetNumNpcControl())
		{
			uidLeast = it->second->GetUid();
			numLeast = it->second->GetNumNpcControl();
		}
	}
	return uidLeast;
}
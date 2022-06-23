#include "stdafx.h"
#include "MNewQuestNpcManager.h"


void MNewQuestNpcManager::AddNpcObject(MUID uidNpc, MUID uidController, int nDropItemId, const char* actorDef)
{
	if (uidNpc.IsInvalid() || uidController.IsInvalid()) { _ASSERT(0); return; }

	MNewQuestNpcObject* pNewNPC = new MNewQuestNpcObject(uidNpc);
	pNewNPC->AssignControl(uidController);
	pNewNPC->SetDropItemId(nDropItemId);
	pNewNPC->SetActorDef(actorDef);
	m_mapNpc[uidNpc] = pNewNPC;
}

void MNewQuestNpcManager::DeleteNpcObject(MUID uidNpc)
{
	if (uidNpc.IsInvalid()) { _ASSERT(0); return; }

	ItorNpc it = m_mapNpc.find(uidNpc);
	if (it == m_mapNpc.end())
		return;

	m_mapNpc.erase(it);
	delete it->second;

}

MNewQuestNpcObject* MNewQuestNpcManager::GetNpc( MUID uid )
{
	if (uid.IsInvalid()) { _ASSERT(0); return NULL; }

	ItorNpc it = m_mapNpc.find(uid);
	if (it == m_mapNpc.end())
		return NULL;

	return it->second;
}

bool MNewQuestNpcManager::GetNpcByName(const char* npcName)
{
	for (auto itor = m_mapNpc.begin(); itor != m_mapNpc.end(); ++itor)
	{
		string currNpc = (*itor).second->GetActorDef();

		if (_stricmp(currNpc.c_str(), npcName)==0)
			return true;
	}
	return false;
}
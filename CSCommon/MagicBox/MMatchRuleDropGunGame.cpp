#include "stdafx.h"
#include "MMatchRule.h"
#include "MMatchRuleDropGunGame.h"
#include "MMatchServer.h"
#include "MBlobArray.h"
#include "MMatchTransDataType.h"

//////////////////////////////////////////////////////////////////////////////////
// MMatchRuleDropGunGame ///////////////////////////////////////////////////////////
MDropGunGameXMLManager	MMatchRuleDropGunGame::m_DropGunGameXMLManager;
MDropGunGameStageManager MMatchRuleDropGunGame::m_DropGunGameStageManager;

MMatchRuleDropGunGame::MMatchRuleDropGunGame(MMatchStage* pStage) : MMatchRule(pStage)
{
	SetGunGameListID(0);
}

bool MMatchRuleDropGunGame::InitDropGunGameListMgr()
{
	if (!m_DropGunGameXMLManager.ReadXml(NULL, "xml/DropGunGameList.xml"))
	{
		mlog("Load XML from memory : %s- FAILED\n", "xml/DropGunGameList.xml");
		return false;
	}
	else
	{
		mlog("Load XML from memory : %s- SUCCESS\n", "xml/DropGunGameList.xml");
		return true;
	}
}

void MMatchRuleDropGunGame::OnBegin()
{
	MMatchStage* pStage = GetStage();
	if (pStage == NULL)
		return;

	// Add Map Index 
	SetGunGameListID(pStage->GetStageSetting()->GetMapIndex());

	// Add Stage Information
	m_DropGunGameStageManager.AddStage(pStage->GetUID().Low);
}
void MMatchRuleDropGunGame::OnEnd()
{
	MMatchStage* pStage = GetStage();
	if (pStage == NULL)
		return;

	if (m_DropGunGameStageManager.GetStorageByStageID(pStage->GetUID().Low) == NULL)
		return;

	// Remove Stage Information
	m_DropGunGameStageManager.GetStorageByStageID(pStage->GetUID().Low)->clear();
}

void MMatchRuleDropGunGame::OnRoundBegin()
{
	MMatchServer* pServer = MMatchServer::GetInstance();
	MMatchStage* pStage = GetStage();
	if (pServer == NULL || pStage == NULL) return;

	// Tell Everyone The New Information
	for (MUIDRefCache::iterator itor = pStage->GetObjBegin(); itor != pStage->GetObjEnd(); itor++)
	{
		MMatchObject* pObj = (MMatchObject*)(*itor).second;
		if (pObj == NULL)
			continue;

		if (pObj->GetEnterBattle() == false)
			continue;

		// Get Player Storage Info
		MDropGunGameBattleStageStorage* pDropGunGameBattleStageStorage = m_DropGunGameStageManager.GetStorageByStageID(pStage->GetUID().Low)->GetPlayerByCID(pObj->GetCharInfo()->m_nCID);
		if (pDropGunGameBattleStageStorage == NULL)
			continue;

		// Tell Player Information
		MCommand* pEquipInfo = MMatchServer::GetInstance()->CreateCommand(MC_DROPGUNGAME_RESPONSE_ENTERGAME, MUID(0, 0));
		pEquipInfo->AddParameter(new MCmdParamUID(pObj->GetUID()));
		pEquipInfo->AddParameter(new MCmdParamInt(pDropGunGameBattleStageStorage->GetItemID1()));
		pEquipInfo->AddParameter(new MCmdParamInt(pDropGunGameBattleStageStorage->GetItemID2()));
		pEquipInfo->AddParameter(new MCmdParamInt(pDropGunGameBattleStageStorage->GetItemID3()));
		pEquipInfo->AddParameter(new MCmdParamInt(pDropGunGameBattleStageStorage->GetItemID4()));
		pEquipInfo->AddParameter(new MCmdParamInt(pDropGunGameBattleStageStorage->GetItemID5()));
		pEquipInfo->AddParameter(new MCmdParamBool(false));
		MMatchServer::GetInstance()->RouteToBattle(pObj->GetStageUID(), pEquipInfo);

		/////////////////
		//Spawn World Weapon Items
		MDropGunGameWorldItemMgr* ItemSet = m_DropGunGameXMLManager.GetDropGunGameWorldItemByID(GetGunGameListID());
		for (int i = 0; i != 100; i++)
		{
			MDropGunGameWorldItem* item = ItemSet->GetDropGunGameWorldItemByListID(i);
			if (item == NULL)
				continue;

			MCommand* pEquipInfo = MMatchServer::GetInstance()->CreateCommand(MC_DROPGUNGAME_RESPONSE_WORLDITEMS, MUID(0, 0));
			pEquipInfo->AddParameter(new MCmdParamUID(pObj->GetUID()));
			pEquipInfo->AddParameter(new MCmdParamInt(item->GetItemID()));
			pEquipInfo->AddParameter(new MCmdParamFloat(item->GetAssX()));
			pEquipInfo->AddParameter(new MCmdParamFloat(item->GetAssY()));
			pEquipInfo->AddParameter(new MCmdParamFloat(item->GetAssZ()));
			MMatchServer::GetInstance()->RouteToBattle(pObj->GetStageUID(), pEquipInfo);
		}
	}
}

bool MMatchRuleDropGunGame::RoundCount()
{
	if (++m_nRoundCount < 1) return true;
	return false;
}

bool MMatchRuleDropGunGame::CheckKillCount(MMatchObject* pOutObject)
{
	MMatchStage* pStage = GetStage();
	for (MUIDRefCache::iterator i = pStage->GetObjBegin(); i != pStage->GetObjEnd(); i++)
	{
		MMatchObject* pObj = (MMatchObject*)(*i).second;
		if (pObj->GetEnterBattle() == false) continue;

		if (pObj->GetKillCount() >= (unsigned int)pStage->GetStageSetting()->GetRoundMax())
		{
			pOutObject = pObj;
			return true;
		}
	}
	return false;
}

void MMatchRuleDropGunGame::OnGameKill(const MUID& uidAttacker, const MUID& uidVictim)
{
	// Victim Info
	MMatchObject* pVictim = MMatchServer::GetInstance()->GetObject(uidVictim);
	if (pVictim == NULL) return;

	// Attacker CharInfo
	MMatchCharInfo* pCharInfo = pVictim->GetCharInfo();
	if (pCharInfo == NULL) return;

	// Stage Info
	MMatchStage* pStage = MMatchServer::GetInstance()->FindStage(pVictim->GetStageUID());
	if (pStage == NULL) return;

	// Get Player Storage Info
	MDropGunGameBattleStageStorage* pDropGunGameBattleStageStorage = m_DropGunGameStageManager.GetStorageByStageID(pStage->GetUID().Low)->GetPlayerByCID(pCharInfo->m_nCID);
	if (pDropGunGameBattleStageStorage == NULL) return;

	// Reset Weapons
	pDropGunGameBattleStageStorage->SetItemID1(0);
	pDropGunGameBattleStageStorage->SetItemID2(0);
	pDropGunGameBattleStageStorage->SetItemID3(0);
	pDropGunGameBattleStageStorage->SetItemID4(0);
	pDropGunGameBattleStageStorage->SetItemID5(0);
}

void MMatchRuleDropGunGame::OnObtainWorldItem(MMatchObject* pObj)
{
	// Object Check
	if (pObj == NULL) return;

	// Stage Info
	MMatchStage* pStage = MMatchServer::GetInstance()->FindStage(pObj->GetStageUID());
	if (pStage == NULL) return;

	// Get Player Storage Info
	MDropGunGameBattleStageStorage* pDropGunGameBattleStageStorage = m_DropGunGameStageManager.GetStorageByStageID(pStage->GetUID().Low)->GetPlayerByCID(pObj->GetCharInfo()->m_nCID);
	if (pDropGunGameBattleStageStorage == NULL) return;

	/////////////////
	//Select Weapon Set
	MDropGunGameItemSetMgr* ItemSet = m_DropGunGameXMLManager.GetDropGunGameItemSetByID(GetGunGameListID());

	int rand = std::rand() % 10 + 1;

	for (int i = 0; i != 11; i++)
	{
		MDropGunGameItemSet* item = ItemSet->GetDropGunGameItemSetByListID(i);
		if (item == NULL)
			continue;

		if (rand != i)
			continue;

		MCommand* pEquipInfo = MMatchServer::GetInstance()->CreateCommand(MC_DROPGUNGAME_RESPONSE_ITEM, MUID(0, 0));
		pEquipInfo->AddParameter(new MCmdParamUID(pObj->GetUID()));
		pEquipInfo->AddParameter(new MCmdParamInt(item->GetItemID1()));
		pEquipInfo->AddParameter(new MCmdParamInt(item->GetItemID2()));
		pEquipInfo->AddParameter(new MCmdParamInt(item->GetItemID3()));
		pEquipInfo->AddParameter(new MCmdParamInt(item->GetItemID4()));
		pEquipInfo->AddParameter(new MCmdParamInt(item->GetItemID5()));
		pEquipInfo->AddParameter(new MCmdParamBool(false));
		MMatchServer::GetInstance()->RouteToBattle(pObj->GetStageUID(), pEquipInfo);

		//Insert Weapons in storages
		pDropGunGameBattleStageStorage->SetItemID1(item->GetItemID1());
		pDropGunGameBattleStageStorage->SetItemID2(item->GetItemID2());
		pDropGunGameBattleStageStorage->SetItemID3(item->GetItemID3());
		pDropGunGameBattleStageStorage->SetItemID4(item->GetItemID4());
		pDropGunGameBattleStageStorage->SetItemID5(item->GetItemID5());
	}
}

void MMatchRuleDropGunGame::OnEnterBattle(MUID& uidChar)
{
	// EnterBattle Info
	MMatchObject* pEnterBattle = MMatchServer::GetInstance()->GetObject(uidChar);
	if (pEnterBattle == NULL) return;

	// Attacker CharInfo
	MMatchCharInfo* pCharInfo = pEnterBattle->GetCharInfo();
	if (pCharInfo == NULL) return;

	// Stage Info
	MMatchStage* pStage = MMatchServer::GetInstance()->FindStage(pEnterBattle->GetStageUID());
	if (pStage == NULL) return;

	// Add Player In Stage Storage
	m_DropGunGameStageManager.GetStorageByStageID(pStage->GetUID().Low)->AddPlayer(pEnterBattle, pCharInfo->m_nCID, 0, 0, 0, 0, 0);

	// Install Player Information
	for (MUIDRefCache::iterator itor = pStage->GetObjBegin(); itor != pStage->GetObjEnd(); itor++)
	{
		MMatchObject* pObj = (MMatchObject*)(*itor).second;
		if (pObj == NULL)
			continue;

		if (pObj->GetEnterBattle() == false)
			continue;

		// Get Player Storage Info
		MDropGunGameBattleStageStorage* pDropGunGameBattleStageStorage = m_DropGunGameStageManager.GetStorageByStageID(pStage->GetUID().Low)->GetPlayerByCID(pObj->GetCharInfo()->m_nCID);
		if (pDropGunGameBattleStageStorage == NULL)
			continue;

		// Tell Player Information
		MCommand* pEquipInfo = MMatchServer::GetInstance()->CreateCommand(MC_DROPGUNGAME_RESPONSE_ENTERGAME, MUID(0, 0));
		pEquipInfo->AddParameter(new MCmdParamUID(pObj->GetUID()));
		pEquipInfo->AddParameter(new MCmdParamInt(pDropGunGameBattleStageStorage->GetItemID1()));
		pEquipInfo->AddParameter(new MCmdParamInt(pDropGunGameBattleStageStorage->GetItemID2()));
		pEquipInfo->AddParameter(new MCmdParamInt(pDropGunGameBattleStageStorage->GetItemID3()));
		pEquipInfo->AddParameter(new MCmdParamInt(pDropGunGameBattleStageStorage->GetItemID4()));
		pEquipInfo->AddParameter(new MCmdParamInt(pDropGunGameBattleStageStorage->GetItemID5()));
		if (pEnterBattle->GetUID() == pObj->GetUID())
			pEquipInfo->AddParameter(new MCmdParamBool(true));
		else
			pEquipInfo->AddParameter(new MCmdParamBool(false));
		MMatchServer::GetInstance()->RouteToBattle(pEnterBattle->GetStageUID(), pEquipInfo);

		if (pEnterBattle->GetUID() == pObj->GetUID())
		{
			/////////////////
			//Spawn World Weapon Items
			MDropGunGameWorldItemMgr* ItemSet = m_DropGunGameXMLManager.GetDropGunGameWorldItemByID(GetGunGameListID());
			for (int i = 0; i != 100; i++)
			{
				MDropGunGameWorldItem* item = ItemSet->GetDropGunGameWorldItemByListID(i);
				if (item == NULL)
					continue;

				MCommand* pEquipInfo = MMatchServer::GetInstance()->CreateCommand(MC_DROPGUNGAME_RESPONSE_WORLDITEMS, MUID(0, 0));
				pEquipInfo->AddParameter(new MCmdParamUID(pObj->GetUID()));
				pEquipInfo->AddParameter(new MCmdParamInt(item->GetItemID()));
				pEquipInfo->AddParameter(new MCmdParamFloat(item->GetAssX()));
				pEquipInfo->AddParameter(new MCmdParamFloat(item->GetAssY()));
				pEquipInfo->AddParameter(new MCmdParamFloat(item->GetAssZ()));
				MMatchServer::GetInstance()->RouteToBattle(pObj->GetStageUID(), pEquipInfo);
			}
		}
	}
}

void MMatchRuleDropGunGame::OnLeaveBattle(MUID& uidChar)
{
	MMatchObject* pChar = MMatchServer::GetInstance()->GetObject(uidChar);
	if (pChar == NULL) return;

	// Stage Info
	MMatchStage* pStage = MMatchServer::GetInstance()->FindStage(pChar->GetStageUID());
	if (pStage == NULL) return;

	// Remove Player to game storage
	m_DropGunGameStageManager.GetStorageByStageID(pStage->GetUID().Low)->RemovePlayer(pChar, pChar->GetCharInfo()->m_nCID);
}

bool MMatchRuleDropGunGame::OnCheckRoundFinish()
{
	MMatchObject* pObject = NULL;

	if (CheckKillCount(pObject))
	{
		return true;
	}
	return false;
}

void MMatchRuleDropGunGame::OnRoundTimeOut()
{
	SetRoundArg(MMATCH_ROUNDRESULT_DRAW);
}
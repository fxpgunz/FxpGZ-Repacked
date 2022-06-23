#include "stdafx.h"
#include "IMatchRuleNewQuest.h"
#include "MNewQuestNpcManager.h"
#include "MNewQuestPlayerManager.h"
#include "MSharedCommandTable.h"
#include "MMatchWorldItemDesc.h"

MActorDefManager IMatchRuleNewQuest::ms_actorDefMgr;

IMatchRuleNewQuest::IMatchRuleNewQuest(MMatchStage* pStage) : MMatchRule(pStage)
, m_pPlayerMgr(NULL)
, m_pNpcMgr(NULL)
{

}

IMatchRuleNewQuest::~IMatchRuleNewQuest()
{

}

void IMatchRuleNewQuest::InitActorDefMgr()
{
	if (!ms_actorDefMgr.ReadXml(NULL, "npc2.xml"))
	{
		mlog("Read new quest file %s Failed\n", "npc2.xml");
		_ASSERT(0); return;
	}
}

void IMatchRuleNewQuest::OnBegin()
{
	m_pPlayerMgr = new MNewQuestPlayerManager;
	m_pNpcMgr = new MNewQuestNpcManager;

	for (MUIDRefCache::iterator it = m_pStage->GetObjBegin(); it != m_pStage->GetObjEnd(); ++it)
	{
		MUID uidChar = it->first;

		MMatchObject* pObj = MMatchServer::GetInstance()->GetObject(uidChar);
		if (IsAdminGrade(pObj) && pObj->CheckPlayerFlags(MTD_PlayerFlags_AdminHide)) continue;

		m_pPlayerMgr->AddPlayer(pObj);
	}
}

void IMatchRuleNewQuest::OnEnd()
{
	delete m_pPlayerMgr;
	delete m_pNpcMgr;
}

void IMatchRuleNewQuest::OnRoundBegin()
{
}

void IMatchRuleNewQuest::OnRoundEnd()
{

}

void IMatchRuleNewQuest::OnEnterBattle(MUID& uidChar)
{

	MMatchObject* pObj = MGetMatchServer()->GetObject(uidChar);

	///only add if they aren't in hide mode
	if (IsAdminGrade(pObj)) {
		if (!pObj->CheckPlayerFlags(MTD_PlayerFlags::MTD_PlayerFlags_AdminHide))
			m_pPlayerMgr->AddPlayer(pObj);
	}
	else
		m_pPlayerMgr->AddPlayer(pObj);

	for (auto itor = m_pNpcMgr->GetNpcs()->begin(); itor != m_pNpcMgr->GetNpcs()->end(); ++itor)
	{
		RouteSpawnLateJoinNpc(uidChar, (*itor).first, (*itor).second->GetActorDef(), 0, 0);
	}
}

void IMatchRuleNewQuest::OnLeaveBattle(MUID& uidChar)
{
	MGetMatchServer()->ResponseCharacterItemList(uidChar);

	m_pPlayerMgr->RemovePlayer(uidChar);
	MUID newController = m_pPlayerMgr->FindSuitableController();
	if (newController.IsInvalid())
		return;
	for (auto itor = m_pNpcMgr->GetNpcs()->begin(); itor != m_pNpcMgr->GetNpcs()->end(); ++itor)
	{
		if (itor->second->GetController() == uidChar)
		{
			itor->second->AssignControl(newController);
			MCommand* pCmd = MGetMatchServer()->CreateCommand(MC_NEWQUEST_NEW_CONTROLLER, MUID(0, 0));
			pCmd->AddParameter(new MCmdParamUID(newController));//controller;
			pCmd->AddParameter(new MCmdParamUID(itor->first));//npc uid;

			MGetMatchServer()->RouteToBattle(GetStage()->GetUID(), pCmd);
		}
	}
}

void IMatchRuleNewQuest::OnGameKill(const MUID& uidAttacker, const MUID& uidVictim)
{

}

void IMatchRuleNewQuest::OnCommand(MCommand* pCommand)
{
	switch (pCommand->GetID())
	{
		case MC_NEWQUEST_REQUEST_NPC_DEAD:
		{
			MUID uidSender = pCommand->GetSenderUID();
			MUID uidKiller, uidNPC;
			MShortVector s_pos;
			pCommand->GetParameter(&uidKiller, 0, MPT_UID);
			pCommand->GetParameter(&uidNPC, 1, MPT_UID);
			pCommand->GetParameter(&s_pos, 2, MPT_SVECTOR);
			MVector pos = MVector((float)s_pos.x, (float)s_pos.y, (float)s_pos.z);

			MNewQuestNpcObject* pNpc = m_pNpcMgr->GetNpc(uidNPC);
			if (!pNpc)
			{
				_ASSERT(0); break;
			}

			// 커맨드 보낸이는 반드시 해당 npc의 컨트롤러여야 한다. 아니라면 해킹이나 버그 상황
			if (pNpc->GetController() != uidSender)
			{
				_ASSERT(0); break;
			}

			// 드롭 아이템 생성
			DropItemByNpcDead(uidKiller, pNpc->GetDropItemId(), pos);

			m_pNpcMgr->DeleteNpcObject(uidNPC);
			m_pPlayerMgr->DecreaseNpcControl(uidSender);

			RouteNpcDead(uidNPC, uidKiller);
		}
		break;
		case MC_NEWQUEST_REQUEST_NPC_SPAWN:
		{
			MUID uidController;
			MUID uidNpc;
			char szActorDefName[128];
			int num, route;
			MShortVector s_pos, s_dir;

			if (!m_pNpcMgr || !m_pPlayerMgr)
				break;

			if (GetRoundState() != MMATCH_ROUNDSTATE_PLAY)
				break;

			pCommand->GetParameter(&uidController, 0, MPT_UID);
			pCommand->GetParameter(&uidNpc, 1, MPT_UID);
			pCommand->GetParameter(&num, 2, MPT_INT);
			pCommand->GetParameter(szActorDefName, 3, MPT_STR, sizeof(szActorDefName));
			pCommand->GetParameter(&s_pos, 4, MPT_SVECTOR);
			pCommand->GetParameter(&s_dir, 5, MPT_SVECTOR);
			pCommand->GetParameter(&route, 6, MPT_INT);

			MNewQuestNpcObject* pNpc = m_pNpcMgr->GetNpc(uidNpc);
			if (!pNpc)
			{
				mlog("not npc uid\n");
				break;
				//_ASSERT(0); break;
			}

			// 커맨드 보낸이는 반드시 해당 npc의 컨트롤러여야 한다. 아니라면 해킹이나 버그 상황
			if (pNpc->GetController() != pCommand->GetSenderUID())
				break;

			SpawnNpcSummon(szActorDefName, num, s_pos, s_dir,route);
			break;
		}
	}
}

void IMatchRuleNewQuest::DropItemByNpcDead(const MUID& uidKiller, int nWorldItemId, const MVector& pos)
{
	// 월드아이템인지 검사하자
	if (!MGetMatchWorldItemDescMgr()->GetItemDesc(nWorldItemId))
		return;

	MMatchObject* pPlayer = MMatchServer::GetInstance()->GetObject(uidKiller);
	if (!pPlayer) return;

	int nWorldItemExtraValues[WORLDITEM_EXTRAVALUE_NUM];
	for (int i = 0; i < WORLDITEM_EXTRAVALUE_NUM; i++) nWorldItemExtraValues[i] = 0;

	m_pStage->SpawnServerSideWorldItem(pPlayer, nWorldItemId, pos.x, pos.y, pos.z,
		QUEST_DYNAMIC_WORLDITEM_LIFETIME, nWorldItemExtraValues);
}

bool IMatchRuleNewQuest::OnCheckRoundFinish()
{
	///TODO: move this to zrulequestchallenge, blitz is different from this
	if (m_pNpcMgr->GetNumNpc() == 0)
		return true;

	return false;
}

MUID IMatchRuleNewQuest::SpawnNpc(const char* szActorDef, int nCustomSpawnTypeIndex, int nSpawnIndex, int nDropItemId)
{
	if (!m_pNpcMgr || !m_pPlayerMgr)
		return MUID(0,0);

	// 적당한 컨트롤러를 찾는다. 컨트롤러할 사람이 없으면 스폰을 취소
	MUID uidController = m_pPlayerMgr->FindSuitableController();
	if (uidController.IsInvalid())
		return MUID(0,0);


	// npc 매니저에 실제 npc 생성 요청. 아까 찾은 컨트롤러 uid를 넘겨준다
	MUID uidNpc = MMatchServer::GetInstance()->UseUID();
	m_pNpcMgr->AddNpcObject(uidNpc, uidController, nDropItemId,szActorDef);
	m_pPlayerMgr->IncreaseNpcControl(uidController, uidNpc);

	//todok szActorDef를 이름이 아닌 인덱스로 보내도록 수정하자
	RouteSpawnNpc(uidNpc, uidController, szActorDef, nCustomSpawnTypeIndex, nSpawnIndex);

	return uidNpc;
}

void IMatchRuleNewQuest::RouteSpawnNpc(MUID uidNPC, MUID uidController, const char* szNpcDefName, int nCustomSpawnTypeIndex, int nSpawnIndex)
{
	MCommand* pCmd = MMatchServer::GetInstance()->CreateCommand(MC_NEWQUEST_NPC_SPAWN, MUID(0, 0));
	pCmd->AddParameter(new MCmdParamUID(uidController));
	pCmd->AddParameter(new MCmdParamUID(uidNPC));
	pCmd->AddParameter(new MCmdParamStr(szNpcDefName));
	pCmd->AddParameter(new MCmdParamUChar(nCustomSpawnTypeIndex));
	pCmd->AddParameter(new MCmdParamUChar(nSpawnIndex));
	MMatchServer::GetInstance()->RouteToBattle(m_pStage->GetUID(), pCmd);
}

void IMatchRuleNewQuest::SpawnNpcSummon(const char* szActorDef, int num, MShortVector pos, MShortVector dir,const int route)
{
	if (!m_pNpcMgr || !m_pPlayerMgr)
		return;

	// 적당한 컨트롤러를 찾는다. 컨트롤러할 사람이 없으면 스폰을 취소
	MUID uidController = m_pPlayerMgr->FindSuitableController();
	if (uidController.IsInvalid())
		return;

	// npc 매니저에 실제 npc 생성 요청. 아까 찾은 컨트롤러 uid를 넘겨준다
	MUID uidNpc = MMatchServer::GetInstance()->UseUID();
	m_pNpcMgr->AddNpcObject(uidNpc, uidController, 0,szActorDef); //change this back later
	m_pPlayerMgr->IncreaseNpcControl(uidController, uidNpc);

	//todok szActorDef를 이름이 아닌 인덱스로 보내도록 수정하자
	RouteSpawnNpcSummon(uidNpc, uidController, num, szActorDef, pos, dir, route);
}

void IMatchRuleNewQuest::RouteSpawnNpcSummon(MUID uidNpc, MUID uidController, int Num, const char* szNpcDefName, MShortVector pos, MShortVector dir,const int route)
{
	MCommand* pCmd = MMatchServer::GetInstance()->CreateCommand(MC_NEWQUEST_REQUEST_NPC_SPAWN, MUID(0, 0));
	pCmd->AddParameter(new MCmdParamUID(uidController));
	pCmd->AddParameter(new MCmdParamUID(uidNpc));
	pCmd->AddParameter(new MCmdParamInt(Num));
	pCmd->AddParameter(new MCmdParamStr(szNpcDefName));
	pCmd->AddParameter(new MCmdParamShortVector(pos.x, pos.y, pos.z));
	pCmd->AddParameter(new MCmdParamShortVector(dir.x, dir.y, dir.z));
	pCmd->AddParameter(new MCmdParamInt(route));
	MMatchServer::GetInstance()->RouteToBattle(m_pStage->GetUID(), pCmd);
}

void IMatchRuleNewQuest::RouteNpcDead(MUID uidNPC, MUID uidKiller)
{
	MCommand* pNew = MMatchServer::GetInstance()->CreateCommand(MC_NEWQUEST_NPC_DEAD, MUID(0, 0));
	pNew->AddParameter(new MCmdParamUID(uidKiller));
	pNew->AddParameter(new MCmdParamUID(uidNPC));
	MMatchServer::GetInstance()->RouteToBattle(m_pStage->GetUID(), pNew);
}

void IMatchRuleNewQuest::OnRequestPlayerDead(const MUID& uidVictim)
{
	if (m_pPlayerMgr->FindPlayer(uidVictim)) {
		MCommand* pCmd = MGetMatchServer()->CreateCommand(MC_MATCH_QUEST_PLAYER_DEAD, MUID(0, 0));
		pCmd->AddParameter(new MCmdParamUID(uidVictim));
		MGetMatchServer()->RouteToBattle(GetStage()->GetUID(), pCmd);
	}
}

void IMatchRuleNewQuest::RouteSpawnLateJoinNpc(MUID uidLateJoiner,MUID uidNpc, const char* szNpcDefName, int nCustomSpawnTypeIndex, int nSpawnIndex)
{
	MMatchObject* pObj = MGetMatchServer()->GetObject(uidLateJoiner);
	if (pObj == nullptr)
		return;
	MCommand* pCmd = MMatchServer::GetInstance()->CreateCommand(MC_NEWQUEST_LATE_NPC_SPAWN, MUID(0, 0));
	pCmd->AddParameter(new MCmdParamUID(uidLateJoiner));
	pCmd->AddParameter(new MCmdParamUID(uidNpc));
	pCmd->AddParameter(new MCmdParamStr(szNpcDefName));
	MMatchServer::GetInstance()->RouteToBattle(GetStage()->GetUID(), pCmd);
}

bool IMatchRuleNewQuest::OnCheckEnableBattleCondition()
{
	if (!GetStage()->CheckBattleEntry())
		return false;

	return true;
}
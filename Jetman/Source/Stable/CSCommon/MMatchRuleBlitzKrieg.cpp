#include "stdafx.h"
#include "MMatchRuleBlitzKrieg.h"
#include "Blitz.h"
#include "MNewQuestNpcManager.h"
MMatchRuleBlitzKrieg::MMatchRuleBlitzKrieg(MMatchStage* pStage) : IMatchRuleNewQuest(pStage)
{

}

MMatchRuleBlitzKrieg::~MMatchRuleBlitzKrieg()
{

}

void MMatchRuleBlitzKrieg::OnBegin()
{
	///TODO: apply redboss/blueboss muids
	IMatchRuleNewQuest::OnBegin();
	redBoss.SetZero();
	blueBoss.SetZero();
	//todo:
	//MGetMatchServer()->OnStartBlitz();
}

void MMatchRuleBlitzKrieg::OnEnd()
{
	redBoss.SetZero();
	blueBoss.SetZero();
	IMatchRuleNewQuest::OnRoundEnd();
}

///no latejoin in blitz, doesnt' work with the concept of the game
void MMatchRuleBlitzKrieg::OnEnterBattle(MUID& uidChar)
{
	MMatchObject* Obj = MGetMatchServer()->GetObject(uidChar);
	if (Obj == nullptr)
		return;

	if (!IsAdminGrade(Obj) && !Obj->CheckPlayerFlags(MTD_PlayerFlags::MTD_PlayerFlags_AdminHide))
		return;

	IMatchRuleNewQuest::OnEnterBattle(uidChar);

	////enables latejoin i hope?
	//IMatchRuleNewQuest::OnEnterBattle(uidChar);
	//if(!m_pPlayerMgr.)
	MCommand* pCmd = MGetMatchServer()->CreateCommand(MC_BLITZ_START_HONOR, MUID(0, 0));
	pCmd->AddParameter(new MCmdParamUID(uidChar));
	pCmd->AddParameter(new MCmdParamInt(MGetBlitz()->GetPlayerHonorInfo().startHonor));

	MGetMatchServer()->RouteToListener(Obj, pCmd);
}

void MMatchRuleBlitzKrieg::OnLeaveBattle(MUID& uidChar)
{
	IMatchRuleNewQuest::OnLeaveBattle(uidChar);
}

void MMatchRuleBlitzKrieg::OnGameKill(const MUID& uidAttacker, const MUID& uidVictim)
{
	///TODO: route playerkill honor :)
	MMatchObject* pAttacker = MMatchServer::GetInstance()->GetObject(uidAttacker);
	MMatchObject* pVictim = MMatchServer::GetInstance()->GetObject(uidVictim);

	if (pAttacker == nullptr || pVictim == nullptr)
		return;
	if (pAttacker == pVictim) return;

	if (pAttacker->GetTeam() == pVictim->GetTeam())
		return;//shouldn't be possible, but just in case :)


	SendPlayerKillHonor(pAttacker->GetUID(),pVictim->GetUID());
}

void MMatchRuleBlitzKrieg::OnRoundBegin()
{
	m_tresureSpawnTime = MGetMatchServer()->GetGlobalClockCount();
	SendBaseHonor();
}

void MMatchRuleBlitzKrieg::OnRoundEnd()
{

}

void MMatchRuleBlitzKrieg::OnRoundCountDown()
{
	ProcessNpcSpawning();
}

void MMatchRuleBlitzKrieg::ProcessNpcSpawning()
{
	for (int i = 0; i < (int)MGetBlitz()->GetSpawnList().size(); ++i)
	{
		SpawnList spawnList = MGetBlitz()->GetSpawnList().at(i);
		for (int j = 0; j < spawnList.npcCount; ++j)
		{
			SpawnNpc(spawnList.actorName, 0, j, 0);
		}
	}
}

MUID MMatchRuleBlitzKrieg::SpawnNpc(const char* name, int customSpawnIndex, int spawnIndex, int dropitemID)
{
	MUID uidNpc = IMatchRuleNewQuest::SpawnNpc(name, customSpawnIndex, spawnIndex, dropitemID);
	if (_stricmp(name, "radar_red") == 0)
		redBoss = uidNpc;
	else if (_stricmp(name, "radar_blue") == 0)
		blueBoss = uidNpc;

	return MUID(0,0);
}

void MMatchRuleBlitzKrieg::RouteSpawnNpc(MUID uidNPC, MUID uidController, const char* szNpcDefName, int nCustomSpawnTypeIndex, int nSpawnIndex)
{
	IMatchRuleNewQuest::RouteSpawnNpc(uidNPC, uidController, szNpcDefName, nCustomSpawnTypeIndex, nSpawnIndex);
}

void MMatchRuleBlitzKrieg::SpawnNpcSummon(const char* szActorDef, int num, MShortVector pos, MShortVector dir,const int route)
{
	IMatchRuleNewQuest::SpawnNpcSummon(szActorDef, num, pos, dir,route);
}

void MMatchRuleBlitzKrieg::RouteSpawnNpcSummon(MUID uidNpc, MUID uidController, int Num, const char* szNpcDefName, MShortVector pos, MShortVector dir,const int route)
{
	IMatchRuleNewQuest::RouteSpawnNpcSummon(uidNpc, uidController, Num, szNpcDefName, pos, dir,route);
}

void MMatchRuleBlitzKrieg::RouteNpcDead(MUID uidNpc, MUID uidKiller)
{
	IMatchRuleNewQuest::RouteNpcDead(uidNpc, uidKiller);

	if (uidNpc == redBoss)
		redBoss.SetInvalid();
	else if(uidNpc == blueBoss)
		blueBoss.SetInvalid();
}

void MMatchRuleBlitzKrieg::RouteSpawnLateJoinNpc(MUID uidLateJoiner, MUID uidNpc, const char* szNpcDefName, int nCustomSpawnTypeIndex, int nSpawnIndex)
{
	IMatchRuleNewQuest::RouteSpawnLateJoinNpc(uidLateJoiner, uidNpc, szNpcDefName, nCustomSpawnTypeIndex, nSpawnIndex);
}

void MMatchRuleBlitzKrieg::OnCommand(MCommand* pCommand)
{
	if (pCommand->GetID() == 6302 || pCommand->GetID() == 6304)
		IMatchRuleNewQuest::OnCommand(pCommand);
	else
	{
		switch (pCommand->GetID())
		{
			case MC_BLITZ_REQUEST_UPGRADE:
			{
				int upgradIndex;
				unsigned int currhonorPoints;
				pCommand->GetParameter(&upgradIndex, 0, MPT_INT);
				pCommand->GetParameter(&currhonorPoints, 1, MPT_UINT);

				OnRequestUpgrade(pCommand->GetSenderUID(), upgradIndex, currhonorPoints);
			}break;
			case MC_BLITZ_REQUEST_CLASS:
			{
				///TODO: handle this
				int classID;
				pCommand->GetParameter(&classID, 0, MPT_INT);
				OnRequestSelectClass(pCommand->GetSenderUID(), classID);
			}break;
		}
	}

}

void MMatchRuleBlitzKrieg::OnRequestPlayerDead(const MUID& uidPlayer)
{
	IMatchRuleNewQuest::OnRequestPlayerDead(uidPlayer);
}

void MMatchRuleBlitzKrieg::SpawnTresure()
{
	for (int i = 0; i < static_cast<int>(MGetBlitz()->GetHonorNpcInfo().npcInfo.size()); ++i)
	{
		string npcName = MGetBlitz()->GetHonorNpcInfo().npcInfo.at(i);
		if (!m_pNpcMgr->GetNpcByName(npcName.c_str()))
			SpawnNpc(npcName.c_str(), 0, i, 0);
	}
}

void MMatchRuleBlitzKrieg::SendBaseHonor()
{
	for (auto itor = GetStage()->GetObjBegin(); itor != GetStage()->GetObjEnd(); ++itor)
	{
		MMatchObject* pObj = (MMatchObject*)(*itor).second;
		if (pObj->GetPlace() != MMP_BATTLE)
			continue;
		if (pObj->CheckPlayerFlags(MTD_PlayerFlags::MTD_PlayerFlags_AdminHide))
			continue;
		MCommand* pCmd = MGetMatchServer()->CreateCommand(MC_BLITZ_START_HONOR, MUID(0, 0));
		pCmd->AddParameter(new MCmdParamUID(pObj->GetUID()));
		pCmd->AddParameter(new MCmdParamInt(MGetBlitz()->GetPlayerHonorInfo().startHonor));

		MGetMatchServer()->RouteToBattle(GetStage()->GetUID(), pCmd);
	}

}

void MMatchRuleBlitzKrieg::SendPlayerKillHonor(const MUID& uidAttacker,const MUID& uidVictim)
{
	MCommand* pCmd = MGetMatchServer()->CreateCommand(MC_BLITZ_PLAYER_KILL, MUID(0, 0));
	pCmd->AddParameter(new MCmdParamUID(uidAttacker));
	pCmd->AddParameter(new MCmdParamUID(uidVictim));

	MGetMatchServer()->RouteToBattle(GetStage()->GetUID(),pCmd);
}

bool MMatchRuleBlitzKrieg::OnRun()
{
	DWORD nClock = MMatchServer::GetInstance()->GetGlobalClockCount();

	switch (GetRoundState())
	{
		case MMATCH_ROUNDSTATE_PREPARE:
		{
			if (GetStage()->CheckBattleEntry())
			{
				SetRoundState(MMATCH_ROUNDSTATE_COUNTDOWN);
				return true;
			}
			return true;
		}
		case MMATCH_ROUNDSTATE_COUNTDOWN:
		{
			if (nClock - GetRoundStateTimer() > 30000)
			{
				SetRoundState(MMATCH_ROUNDSTATE_PLAY);
				return true;
			}
			return true;
		}
		case MMATCH_ROUNDSTATE_PLAY:
		{
			if (OnCheckRoundFinish())
			{
				SetRoundState(MMATCH_ROUNDSTATE_FINISH);
				return true;
			}
			if (OnCheckTresureSpawnTime())
			{
				SpawnTresure();
			}
			return true;
		}
		case MMATCH_ROUNDSTATE_FINISH:
		{
			if (nClock - GetRoundStateTimer() > 8000)
			{
				SetRoundState(MMATCH_ROUNDSTATE_EXIT);
				return true;
			}
			return true;
		}
		case MMATCH_ROUNDSTATE_FREE:
		{
			//nothing
		}
		case MMATCH_ROUNDSTATE_EXIT:
		{
			return false;
		}
	}
	return false;
}

bool MMatchRuleBlitzKrieg::OnCheckTresureSpawnTime()
{
	if (m_tresureSpawnTime + MGetBlitz()->GetHonorNpcInfo().spawnTime * 1000  < MGetMatchServer()->GetGlobalClockCount())
	{
		m_tresureSpawnTime = MGetMatchServer()->GetGlobalClockCount();
		return true;
	}
	return false;
}

bool MMatchRuleBlitzKrieg::OnCheckRoundFinish() {

	if (redBoss.IsInvalid()) {
		SetRoundArg(MMATCH_ROUNDRESULT_BLUEWON);
		return true;
	}
	else if (blueBoss.IsInvalid()) {
		SetRoundArg(MMATCH_ROUNDRESULT_REDWON);
		return true;
	}
	else if (blueBoss.IsInvalid() && redBoss.IsInvalid()) {
		SetRoundArg(MMATCH_ROUNDRESULT_DRAW);
		return true;
	}
	return false;
}

bool MMatchRuleBlitzKrieg::OnRequestUpgrade(const MUID& player, const int upgradIndex, const int currHonorPoints)
{
	///todo: handle this logic
	const Upgrade* upgrade = MGetBlitz()->GetUpgrade(upgradIndex, currHonorPoints,0);
	if (upgrade == nullptr)
		return false;

	/*MCommand* pCmd = MGetMatchServer()->CreateCommand(MC_BLITZ_RESPONSE_UPGRADE, MUID(0,0));
	pCmd->AddParameter(new MCmdParam)*/
	return false;
}

bool MMatchRuleBlitzKrieg::OnRequestSelectClass(const MUID& player, const int classID)
{
	MMatchObject* requester = MGetMatchServer()->GetObject(player);
	for (auto itor = GetStage()->GetObjBegin(); itor != GetStage()->GetObjEnd(); ++itor)
	{
		MMatchObject* player = static_cast<MMatchObject*>((*itor).second);
		if (player->GetTeam() == requester->GetTeam()) {
			if (player->GetBlitzClass() == classID)
				return false;
		}
	}
	requester->SetBlitzClass((MMatchObjectClass)classID);

	MCommand* classCommand = MGetMatchServer()->CreateCommand(MC_BLITZ_RESPONSE_CLASS, MUID(0, 0));
	classCommand->AddParameter(new MCmdParamUID(player));
	classCommand->AddParameter(new MCmdParamInt(classID));

	MGetMatchServer()->RouteToBattle(GetStage()->GetUID(), classCommand);

	return true;
}
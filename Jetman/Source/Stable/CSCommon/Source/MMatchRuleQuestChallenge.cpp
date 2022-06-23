#include "stdafx.h"
#include "MMatchRuleQuestChallenge.h"
#include "MNewQuestNpcManager.h"
#include "MNewQuestPlayerManager.h"
#include "MSharedCommandTable.h"
#include "MMatchWorldItemDesc.h"
#include "MMatchFormula.h"

#define FILENAME_SCENARIO2_XML	"scenario2.xml"
MNewQuestScenarioManager MMatchRuleQuestChallenge::ms_scenarioMgr;

void MMatchRuleQuestChallenge::InitScenarioMgr()
{
	if (!ms_scenarioMgr.ReadXml(NULL, FILENAME_SCENARIO2_XML, MMatchServer::GetInstance()->GetQuest()->GetDropTable()))
	{
		mlog("Read new quest file %s Failed\n", FILENAME_SCENARIO2_XML);
		_ASSERT(0); return;
	}
}

MNewQuestScenarioManager* MMatchRuleQuestChallenge::GetScenarioMgr()
{
	return &ms_scenarioMgr;
}


MMatchRuleQuestChallenge::MMatchRuleQuestChallenge(MMatchStage* pStage)
: IMatchRuleNewQuest(pStage)
, m_pScenario(NULL)
, m_nCurrSector(0)
{
	//LoadTempINI();
}

MMatchRuleQuestChallenge::~MMatchRuleQuestChallenge()
{
}

// 모든 라운드가 끝나면 false 반환
bool MMatchRuleQuestChallenge::RoundCount()
{
	if (!m_pScenario)
	{
		_ASSERT(0);
		return false;
	}

	if (m_nCurrSector < m_pScenario->GetNumSector())
	{
		++m_nCurrSector;
		return true;
	}

	return false;
}

void MMatchRuleQuestChallenge::OnBegin()
{

	IMatchRuleNewQuest::OnBegin();

	m_nCurrSector = 0;

	m_pScenario = ms_scenarioMgr.GetScenario( m_pStage->GetMapName());
	if (!m_pScenario)
		m_pScenario = ms_scenarioMgr.GetScenario( ms_scenarioMgr.GetDefaultScenarioName());

	MGetMatchServer()->OnStartCQ(m_pStage, m_pScenario->GetNumSector());
}

void MMatchRuleQuestChallenge::OnEnd()
{
	IMatchRuleNewQuest::OnRoundEnd();
}

void MMatchRuleQuestChallenge::OnRoundBegin()
{
	// PLAY 상태로 넘어갈때 전부 스폰 시킨다
	ProcessNpcSpawning();

	MMatchRule::OnRoundBegin();
}

void MMatchRuleQuestChallenge::OnRoundEnd()
{
	RouteXpBpBonus();
	if (m_nCurrSector < m_pScenario->GetNumSector())
	{

		++m_nCurrSector;
		RouteMoveToNextSector();
	}


	MMatchRule::OnRoundEnd();
}

void MMatchRuleQuestChallenge::OnSectorBonus(MMatchObject* pObj, const unsigned int nAddedXP, const unsigned int nAddedBP)
{
	MCommand* pCmd = MGetMatchServer()->CreateCommand(MC_QUEST_SECTOR_BONUS, MUID(0, 0));
	pCmd->AddParameter(new MCmdParamUID(pObj->GetUID()));
	pCmd->AddParameter(new MCmdParamUInt(nAddedXP));
	pCmd->AddParameter(new MCmdParamUInt(nAddedBP));

	MGetMatchServer()->RouteToListener(pObj, pCmd);
}

void MMatchRuleQuestChallenge::RouteXpBpBonus()
{
	for (MUIDRefCache::iterator it = m_pStage->GetObjBegin(); it != m_pStage->GetObjEnd(); ++it)
	{
		MUID uidChar = it->first;

		MMatchObject* pObj = MMatchServer::GetInstance()->GetObject(uidChar);
		if (pObj->CheckPlayerFlags(MTD_PlayerFlags_AdminHide)) continue;

		if ((!IsEnabledObject(pObj)) || (!pObj->CheckAlive())) continue;

		//EXP
		int nXP = 0;
		int nBP = 0;

		MNewQuestSector* pCurrSector = NULL;
		if (m_pScenario)
		{
			MNewQuestSector* pCurrSector = m_pScenario->GetSector(m_nCurrSector);
			if (pCurrSector)
			{
				nXP = pCurrSector->GetRewardXP();
				nBP = pCurrSector->GetRewardBP();
			}
		}

		const float fXPBonusRatio = MMatchFormula::CalcXPBonusRatio(pObj, MIBT_QUEST);
		const float fBPBonusRatio = MMatchFormula::CalcBPBounsRatio(pObj, MIBT_QUEST);


		int nExpBonus = (int)(nXP * fXPBonusRatio);
		nXP += nExpBonus;

		int nBPBonus = nBP * fBPBonusRatio;
		nBP += nBPBonus;

		MGetMatchServer()->ProcessPlayerXPBP(m_pStage, pObj, nXP, nBP);


		int nExpPercent = MMatchFormula::GetLevelPercent(pObj->GetCharInfo()->m_nXP,
			pObj->GetCharInfo()->m_nLevel);

		MCommand* pNewCmd = MMatchServer::GetInstance()->CreateCommand(MC_QUEST_SECTOR_BONUS, MUID(0, 0));
		pNewCmd->AddParameter(new MCmdParamUID(pObj->GetUID()));
		pNewCmd->AddParameter(new MCmdParamUInt(nXP));
		pNewCmd->AddParameter(new MCmdParamUInt(nBP));
		MMatchServer::GetInstance()->RouteToListener(pObj, pNewCmd);
	}
}

bool MMatchRuleQuestChallenge::OnRun()
{
	DWORD nClock = MMatchServer::GetInstance()->GetGlobalClockCount();

	switch (GetRoundState())
	{
	case MMATCH_ROUNDSTATE_PREPARE:
	{
		if (GetStage()->CheckBattleEntry() == true)
		{
			SetRoundState(MMATCH_ROUNDSTATE_COUNTDOWN);
			return true;
		}
		return true;
	}
	case MMATCH_ROUNDSTATE_COUNTDOWN:
	{
		if (nClock - GetRoundStateTimer() > 3 * 1000)
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
		if (!CheckPlayersAlive())
		{
			SetRoundState(MMATCH_ROUNDSTATE_EXIT);
			return true;
		}
		return true;
	}
	case MMATCH_ROUNDSTATE_FINISH:
	{
		if (nClock - GetRoundStateTimer() > 1000)
		{
			if (m_nCurrSector < m_pScenario->GetNumSector())
			{
				if (nClock - GetRoundStateTimer() > 6000)
				{
					SetRoundState(MMATCH_ROUNDSTATE_PREPARE);
				}
				return true;
			}
			else
			{
				SetRoundState(MMATCH_ROUNDSTATE_EXIT);
				return true;
			}
		}
		return true;
	}
/*	case MMATCH_ROUNDSTATE_FREE:
	{
		if (OnCheckEnableBattleCondition())
		{
			SetRoundState(MMATCH_ROUNDSTATE_PREPARE);
		}
		return true;
	}*/
	case MMATCH_ROUNDSTATE_EXIT:
	{
		return false;
	}
	}
	return false;
}

bool MMatchRuleQuestChallenge::OnCheckRoundFinish()
{
	return IMatchRuleNewQuest::OnCheckRoundFinish();
}

bool MMatchRuleQuestChallenge::CheckPlayersAlive()
{
	int nAliveCount = 0;
	MMatchObject* pObj;
	for (MUIDRefCache::iterator i = m_pStage->GetObjBegin(); i != m_pStage->GetObjEnd(); i++)
	{
		pObj = (MMatchObject*)(*i).second;
		if (pObj->GetEnterBattle() == false) continue;	// 배틀참가하고 있는 플레이어만 체크
		if (IsAdminGrade(pObj) && pObj->CheckPlayerFlags(MTD_PlayerFlags_AdminHide)) continue;

		if (pObj->CheckAlive() == true)
		{
			++nAliveCount;
		}
	}

	// 모두 죽었으면 리셋
	if (nAliveCount == 0) return false;

	return true;
}


void MMatchRuleQuestChallenge::ProcessNpcSpawning()
{
	// todok 요건 모든 스폰포인트에서 액터를 스폰하는 것 // 기획 나오고 구현을 픽스할 때는 코드 정리 좀 하자 if문 대박이네
	MNewQuestSector* pCurrSector = NULL;
	if (m_pScenario)
	{
		MNewQuestSector* pCurrSector = m_pScenario->GetSector(m_nCurrSector);
		if (pCurrSector)
		{
			int numSpawnType = pCurrSector->GetNumSpawnType();
			for (int i=0; i<numSpawnType; ++i)
			{
				MNewQuestSpawn* pSpawn = pCurrSector->GetSpawnByIndex(i);
				if (pSpawn)
				{
					for (int k=0; k<pSpawn->GetNum(); ++k)
					{
						// drop item 결정
						MQuestDropItem item;
						MMatchServer::GetInstance()->GetQuest()->GetDropTable()->Roll(item, pSpawn->GetDropTableId(), 0);

						SpawnNpc(pSpawn->GetActor(), i, k, item.nID);
					}
				}
			}
		}
	}
}


MUID MMatchRuleQuestChallenge::SpawnNpc(const char* szActorDef, int nCustomSpawnTypeIndex, int nSpawnIndex, int nDropItemId)
{
	return IMatchRuleNewQuest::SpawnNpc(szActorDef, nCustomSpawnTypeIndex, nSpawnIndex, nDropItemId);
}

void MMatchRuleQuestChallenge::RouteSpawnNpc(MUID uidNPC, MUID uidController, const char* szNpcDefName, int nCustomSpawnTypeIndex, int nSpawnIndex)
{
	IMatchRuleNewQuest::RouteSpawnNpc(uidNPC,uidController,szNpcDefName,nCustomSpawnTypeIndex,nSpawnIndex);
}

void MMatchRuleQuestChallenge::SpawnNpcSummon(const char* szActorDef, int num, MShortVector nSpawnPos, MShortVector nSpawnDir,const int route)
{
	IMatchRuleNewQuest::SpawnNpcSummon(szActorDef, num, nSpawnPos, nSpawnDir,route);
}

void MMatchRuleQuestChallenge::RouteSpawnNpcSummon(MUID uidNpc, MUID uidController, int Num, const char* szNpcDefName, MShortVector pos, MShortVector dir,
	const int route)
{
	IMatchRuleNewQuest::RouteSpawnNpcSummon(uidNpc, uidController, Num, szNpcDefName, pos, dir,route);
}

void MMatchRuleQuestChallenge::RouteNpcDead(MUID uidNPC, MUID uidKiller)
{
	IMatchRuleNewQuest::RouteNpcDead(uidNPC, uidKiller);
}

void MMatchRuleQuestChallenge::RouteMoveToNextSector()
{
	MCommand* pNew = MMatchServer::GetInstance()->CreateCommand(MC_NEWQUEST_MOVE_TO_NEXT_SECTOR, MUID(0,0));
	MMatchServer::GetInstance()->RouteToBattle(m_pStage->GetUID(), pNew);
}

void MMatchRuleQuestChallenge::OnCommand(MCommand* pCommand)
{
	IMatchRuleNewQuest::OnCommand(pCommand);
}

void MMatchRuleQuestChallenge::DropItemByNpcDead(const MUID& uidKiller, int nWorldItemId, const MVector& pos)
{
	IMatchRuleNewQuest::DropItemByNpcDead(uidKiller, nWorldItemId, pos);
}

void MMatchRuleQuestChallenge::OnEnterBattle(MUID& uidChar)
{
	//latejoin
	MMatchObject* pObj = MGetMatchServer()->GetObject(uidChar);
	if (!pObj)
		return;

	//only use latejoin info is player isn't in the ptr
	if (!m_pPlayerMgr->FindPlayer(uidChar))
	{
		IMatchRuleNewQuest::OnEnterBattle(uidChar);

		///Added timesync to get the times close to matching when player latejoins
		UINT currTime = MMatchServer::GetInstance()->GetGlobalClockCount() - GetRoundStateTimer();

		MCommand* pCmd = MGetMatchServer()->CreateCommand(MC_MATCH_LATEJOIN_CQ, MUID(0, 0));
		pCmd->AddParameter(new MCmdParamUID(uidChar));
		pCmd->AddParameter(new MCmdParamInt(m_nCurrSector));
		pCmd->AddParameter(new MCmdParamUInt(currTime));

		MGetMatchServer()->RouteToListener(pObj, pCmd);
	}
}

void MMatchRuleQuestChallenge::OnLeaveBattle(MUID& uidChar)
{
	IMatchRuleNewQuest::OnLeaveBattle(uidChar);
}

void MMatchRuleQuestChallenge::OnGameKill(const MUID& uidAttacker, const MUID& uidVictim)
{

}

void MMatchRuleQuestChallenge::OnRequestPlayerDead(const MUID& uidPlayer)
{
	IMatchRuleNewQuest::OnRequestPlayerDead(uidPlayer);
}

void MMatchRuleQuestChallenge::RouteSpawnLateJoinNpc(MUID uidLateJoiner, MUID uidNpc, const char* szNpcDefName, int nCustomSpawnTypeIndex, int nSpawnIndex)
{
	IMatchRuleNewQuest::RouteSpawnLateJoinNpc(uidLateJoiner, uidNpc, szNpcDefName, nCustomSpawnTypeIndex, nSpawnIndex);
}

//Refresh best time????right now ust for min/maxplayers
void MMatchRuleQuestChallenge::RefreshStageGameInfo()
{
	//MakeStageGameInfo();
	//RouteStageGameInfo();
}

void MMatchRuleQuestChallenge::MakeStageGameInfo()
{

}

void MMatchRuleQuestChallenge::RouteStageGameInfo(int const& maxPlayers, int const& minLevel)
{

}
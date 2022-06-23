#include "stdafx.h"
#include "MMatchRuleSkillMap.h"
#include "MMatchWorldItemDesc.h"

MMatchRuleSkillMap::MMatchRuleSkillMap(MMatchStage* pStage) : MMatchRule(pStage)
{

}

MMatchRuleSkillMap::~MMatchRuleSkillMap()
{

}

void MMatchRuleSkillMap::OnBegin()
{

}

void MMatchRuleSkillMap::OnEnd()
{

}

void MMatchRuleSkillMap::OnRoundBegin()
{
	m_roundFinish = false;
}

void MMatchRuleSkillMap::OnRoundEnd()
{

}

void MMatchRuleSkillMap::OnEnterBattle(MUID& player)
{
	MMatchObject* playerObj = MGetMatchServer()->GetObject(player);
	if (playerObj == nullptr)
		return;

	if (playerObj->GetTeam() != MMT_BLUE)
		GetStage()->PlayerTeam(playerObj->GetUID(), MMT_BLUE);
}

void MMatchRuleSkillMap::OnObtainWorldItem(MMatchObject* pObj, int nItemID, int* pnExtraValues)
{
	if (pObj == nullptr)
		return;

	int bonusXP = 0, bonusBP = 0;;

	if (nItemID == 52) {
		bonusXP = 5000;
		bonusBP = 5000;
	}
	else if (nItemID == 53) {
		bonusXP = 10000;
		bonusBP = 10000;
	}
	else if (nItemID == 54) {
		bonusXP = 15000;
		bonusBP = 15000;
	}
	else
		return;

	MGetMatchServer()->ProcessPlayerXPBP(m_pStage, pObj, bonusXP, bonusBP);

	MCommand* pCmd = MGetMatchServer()->CreateCommand(MC_SKILLMAP_OBTAIN_ITEM, MUID(0, 0));
	pCmd->AddParameter(new MCmdParamUID(pObj->GetUID()));
	pCmd->AddParameter(new MCmdParamInt(bonusXP));
	pCmd->AddParameter(new MCmdParamInt(bonusBP));

	MGetMatchServer()->RouteToBattle(GetStage()->GetUID(), pCmd);

	unsigned int mapTime = 0;
	DWORD currTime = MGetMatchServer()->GetGlobalClockCount() - GetRoundStateTimer();


	bool updateSkillMapTime = MGetMatchServer()->GetDBMgr()->GetSkillMapBestTime(pObj->GetCharInfo()->m_nCID, MGetMapDescMgr()->GetMapID(GetStage()->GetMapName()), &mapTime);
	if (mapTime == 0)
	{
		MGetMatchServer()->GetDBMgr()->UpdateSkillMapBestTIme(pObj->GetCharInfo()->m_nCID, MGetMapDescMgr()->GetMapID(GetStage()->GetMapName()), currTime);

		pCmd = MGetMatchServer()->CreateCommand(MC_MATCH_RESPONSE_SKILLMAP_BESTTIME, MUID(0, 0));
		pCmd->AddParameter(new MCmdParamUID(pObj->GetUID()));
		pCmd->AddParameter(new MCmdParamUInt(currTime));
		MGetMatchServer()->RouteToBattle(GetStage()->GetUID(), pCmd);
	}
	else
	{
		if (currTime < mapTime)
		{
			MGetMatchServer()->GetDBMgr()->UpdateSkillMapBestTIme(pObj->GetCharInfo()->m_nCID, MGetMapDescMgr()->GetMapID(GetStage()->GetMapName()), currTime);

			pCmd = MGetMatchServer()->CreateCommand(MC_MATCH_RESPONSE_SKILLMAP_BESTTIME, MUID(0, 0));
			pCmd->AddParameter(new MCmdParamUID(pObj->GetUID()));
			pCmd->AddParameter(new MCmdParamUInt(currTime));
			MGetMatchServer()->RouteToBattle(GetStage()->GetUID(), pCmd);
		}
	}


	m_roundFinish = true;
}

bool MMatchRuleSkillMap::OnRun()
{
	DWORD nClock = MGetMatchServer()->GetGlobalClockCount();
	switch (GetRoundState())
	{
		case MMATCH_ROUNDSTATE_PREPARE:
		{
			if (GetStage()->CheckBattleEntry() == true)
			{
				if (OnCheckEnableBattleCondition())
				{
					SetRoundState(MMATCH_ROUNDSTATE_COUNTDOWN);
					return true;
				}
				else
				{
					SetRoundState(MMATCH_ROUNDSTATE_FREE);
					return true;
				}
				return true;
			}
			return true;
		}
		case MMATCH_ROUNDSTATE_COUNTDOWN:
		{
			if (nClock - GetRoundStateTimer() >= 15000) {
				SetRoundState(MMATCH_ROUNDSTATE_PLAY);
				return true;
			}
			return true;
		}
		case MMATCH_ROUNDSTATE_PLAY:
		{
			if (!OnCheckEnableBattleCondition())
			{
				SetRoundState(MMATCH_ROUNDSTATE_FREE);
				return true;
			}
			if (OnCheckRoundFinish())
			{
				SetRoundState(MMATCH_ROUNDSTATE_FINISH);
				return true;
			}
			return true;
		}
		case MMATCH_ROUNDSTATE_FINISH:
		{
			if (nClock - GetRoundStateTimer() > 4000)
			{
				if (RoundCount())
				{
					SetRoundState(MMATCH_ROUNDSTATE_PREPARE);
					return true;
				}
				else
				{
					SetRoundState(MMATCH_ROUNDSTATE_END);
					return true;
				}
				return true;
			}
			return true;
		}
		case MMATCH_ROUNDSTATE_FREE:
		{
			if (OnCheckEnableBattleCondition())
			{
				SetRoundState(MMATCH_ROUNDSTATE_PREPARE);
				return true;
			}
			if (GetStage()->GetObjInBattleCount() == 0)
				return false;
			return true;
		}
		case MMATCH_ROUNDSTATE_END:
		{
			return false;
		}
		default:
			break;
	}
	return false;
}

bool MMatchRuleSkillMap::OnCheckEnableBattleCondition()
{
	return true;
	//return GetStage()->GetObjInBattleCount() >= 4;
}

bool MMatchRuleSkillMap::OnCheckRoundFinish()
{
	return m_roundFinish;
}

bool MMatchRuleSkillMap::RoundCount()
{
	if (m_pStage == NULL) return false;

	int nTotalRound = m_pStage->GetStageSetting()->GetRoundMax();
	m_nRoundCount++;

	if (m_nRoundCount < nTotalRound) return true;

	return false;
}
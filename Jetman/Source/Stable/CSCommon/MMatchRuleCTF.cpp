#include "stdafx.h"
#include "MMatchRuleCTF.h"
#include "MMatchFormula.h"

// 무한 팀데스매치 - 추가 by 동섭
//////////////////////////////////////////////////////////////////////////
MMatchRuleTeamCTF::MMatchRuleTeamCTF(MMatchStage* pStage) : MMatchRule(pStage)
{
	SetBlueFlagObtained(false);
	SetRedFlagObtained(false);
	SetBlueCarrier(MUID(0, 0));
	SetRedCarrier(MUID(0, 0));
}

void MMatchRuleTeamCTF::OnBegin()
{
	m_pStage->InitTeamKills();
}

void MMatchRuleTeamCTF::OnEnd()
{
	SetBlueFlagObtained(false);
	SetRedFlagObtained(false);
	SetBlueCarrier(MUID(0, 0));
	SetRedCarrier(MUID(0, 0));
}

bool MMatchRuleTeamCTF::OnRun()
{
	DWORD nClock = MMatchServer::GetInstance()->GetGlobalClockCount();
	bool ret = true;

	switch (GetRoundState())
	{
		case MMATCH_ROUNDSTATE_PREPARE:
		{
			if(GetStage()->CheckBattleEntry())
			{
				if (OnCheckEnableBattleCondition())
				{
					SetRoundState(MMATCH_ROUNDSTATE_COUNTDOWN);
				}
				else
				{
					SetRoundState(MMATCH_ROUNDSTATE_FREE);
				}
			}
			ret = true;
		}break;
		case MMATCH_ROUNDSTATE_COUNTDOWN:
		{
			if (OnCheckEnableBattleCondition())
			{
				if (nClock - GetRoundStateTimer() >= 2000)
				{
					SetRoundState(MMATCH_ROUNDSTATE_PLAY);
				}
			}
			else
			{
				SetRoundState(MMATCH_ROUNDSTATE_FREE);
			}
			ret = true;
		}break;
		case MMATCH_ROUNDSTATE_PLAY:
		{
			if (!OnCheckEnableBattleCondition())
			{
				SetRoundState(MMATCH_ROUNDSTATE_FREE);
			}
			if (OnCheckBattleTimeOut(nClock - GetRoundStateTimer()))
			{
				OnRoundTimeOut();
				SetRoundState(MMATCH_ROUNDSTATE_FINISH);
			}
			ret = true;
		}break;
		case MMATCH_ROUNDSTATE_FINISH:
		{
			if (nClock - GetRoundStateTimer() >= 2000)
			{
				SetRoundState(MMATCH_ROUNDSTATE_EXIT);
			}
			ret = true;
		}break;
		case MMATCH_ROUNDSTATE_FREE:
		{	
			if (OnCheckEnableBattleCondition())
			{
				SetRoundState(MMATCH_ROUNDSTATE_PREPARE);
			}
			ret = true;
		}break;
		case MMATCH_ROUNDSTATE_EXIT:
		{
			ret = false;
		}break;
	}
	return ret;
}

void MMatchRuleTeamCTF::OnRoundBegin()
{
	return MMatchRule::OnRoundBegin();
}

void MMatchRuleTeamCTF::OnRoundEnd()
{
	if (m_pStage != NULL)
	{
		if (m_nRoundArg == MMATCH_ROUNDRESULT_REDWON)
		{
			m_pStage->OnRoundEnd_FromTeamGame(MMT_RED);
		}
		else if (m_nRoundArg == MMATCH_ROUNDRESULT_BLUEWON)
		{
			m_pStage->OnRoundEnd_FromTeamGame(MMT_BLUE);
		}
		else if (m_nRoundArg == MMATCH_ROUNDRESULT_DRAW)
		{
			// Do Nothing
		}
	}

	MMatchRule::OnRoundEnd();
}

// 만약 레드팀이나 블루팀에서 팀원이 0명일 경우는 false 반환 , true,false 모두 AliveCount 반환
void MMatchRuleTeamCTF::GetTeamScore(int* pRedTeamScore, int* pBlueTeamScore)
{
	(*pRedTeamScore) = 0;
	(*pBlueTeamScore) = 0;

	MMatchStage* pStage = GetStage();
	if (pStage == NULL) return;

	(*pRedTeamScore) = pStage->GetTeamKills(MMT_RED);
	(*pBlueTeamScore) = pStage->GetTeamKills(MMT_BLUE);

	return;
}

bool MMatchRuleTeamCTF::OnCheckRoundFinish()
{
	int nRedScore, nBlueScore;
	GetTeamScore(&nRedScore, &nBlueScore);

	MMatchStage* pStage = GetStage();

	if (nRedScore >= pStage->GetStageSetting()->GetRoundMax())
	{
		SetRoundArg(MMATCH_ROUNDRESULT_REDWON);
		return true;
	}
	else if (nBlueScore >= pStage->GetStageSetting()->GetRoundMax())
	{
		SetRoundArg(MMATCH_ROUNDRESULT_BLUEWON);
		return true;
	}

	return false;
}

void MMatchRuleTeamCTF::OnRoundTimeOut()
{
	if (!OnCheckRoundFinish())
		SetRoundArg(MMATCH_ROUNDRESULT_DRAW);
}

// 반환값이 false이면 게임이 끝난다.
bool MMatchRuleTeamCTF::RoundCount()
{
	if(++m_nRoundCount < 1) return true;
	return false;
}

void MMatchRuleTeamCTF::CalcTeamBonus(MMatchObject* pAttacker, MMatchObject* pVictim,
	int nSrcExp, int* poutAttackerExp, int* poutTeamExp)
{
	if (m_pStage == NULL)
	{
		*poutAttackerExp = nSrcExp;
		*poutTeamExp = 0;
		return;
	}

	*poutTeamExp = (int)(nSrcExp * m_pStage->GetStageSetting()->GetCurrGameTypeInfo()->fTeamBonusExpRatio);
	*poutAttackerExp = (int)(nSrcExp * m_pStage->GetStageSetting()->GetCurrGameTypeInfo()->fTeamMyExpRatio);
}

void MMatchRuleTeamCTF::OnObtainWorldItem(MMatchObject* pObj, int nItemID, int* pnExtraValues)
{
	if (0 == pObj)
		return;

	if (m_pStage == NULL)
	{
		SetBlueFlagObtained(false);
		SetRedFlagObtained(false);
		SetBlueCarrier(MUID(0, 0));
		SetRedCarrier(MUID(0, 0));
		return;
	}

	MMatchObject* pObtainer = pObj;
	MMatchTeam nTeam = pObj->GetTeam();

	int nBlueTeamCount = 0;
	int nRedTeamCount = 0;
	for (MUIDRefCache::iterator i = m_pStage->GetObjBegin(); i != m_pStage->GetObjEnd(); i++) {
		MMatchObject* pTeamObj = (MMatchObject*)(*i).second;
		if (pTeamObj->GetEnterBattle() == true)
		{
			if (pTeamObj->GetTeam() == MMT_RED)
			{
				nRedTeamCount++;
			}
			else if (pTeamObj->GetTeam() == MMT_BLUE)
			{
				nBlueTeamCount++;
			}
		}
	}

	if (MMT_BLUE == nTeam)
	{
		if (IsRedFlagTaken() == false && nItemID == CTF_RED_ITEM_ID)
		{
			MUID obtainerUID = pObtainer->GetUID();
			// this is a grab
			SetRedFlagObtained(true);
			m_pStage->m_WorldItemManager.ChangeFlagState(false, MMT_RED);
			SetBlueCarrier(obtainerUID);
			RouteAssignFlag(obtainerUID, nTeam);
			SendAssignState();
		}
		else if (IsBlueFlagTaken() == false && nItemID == CTF_BLUE_ITEM_ID && IsRedFlagTaken() == true)
		{
			SetBlueFlagObtained(false);
			SetRedFlagObtained(false);
			SetBlueCarrier(MUID(0, 0));
			SetRedCarrier(MUID(0, 0));
			m_pStage->AddTeamKills(nTeam);
			m_pStage->m_WorldItemManager.ChangeFlagState(true, MMT_RED);
			if (pObj->GetCharInfo() && nBlueTeamCount > NUM_APPLYED_TEAMBONUS_TEAM_PLAYERS && nRedTeamCount > NUM_APPLYED_TEAMBONUS_TEAM_PLAYERS)
			{
				unsigned long int nGettingExp = (MMatchFormula::GetGettingExp(pObj->GetCharInfo()->m_nLevel, pObj->GetCharInfo()->m_nLevel) * 3);
				MMatchServer::GetInstance()->ApplyObjectTeamBonus(pObj, nGettingExp * 3);
				//			m_pStage->OnApplyTeamBonus(nTeam);
			}
			m_pStage->ResetTeamBonus();
			RouteAssignCap(nTeam);
			SendAssignState();
			//this is a cap
		}
		else
		{
			return;
		}
	}
	else if (MMT_RED == nTeam) //reverse logic
	{
		if (IsBlueFlagTaken() == false && nItemID == CTF_BLUE_ITEM_ID)
		{
			MUID obtainerUID = pObtainer->GetUID();
			// this is a grab
			SetBlueFlagObtained(true);
			m_pStage->m_WorldItemManager.ChangeFlagState(false, MMT_BLUE);
			SetRedCarrier(obtainerUID);
			RouteAssignFlag(obtainerUID, nTeam);
			SendAssignState();
		}
		else if (IsBlueFlagTaken() == true && nItemID == CTF_RED_ITEM_ID && IsRedFlagTaken() == false)
		{
			SetBlueFlagObtained(false);
			SetRedFlagObtained(false);
			SetBlueCarrier(MUID(0, 0));
			SetRedCarrier(MUID(0, 0));
			m_pStage->AddTeamKills(nTeam);
			m_pStage->m_WorldItemManager.ChangeFlagState(true, MMT_BLUE);
			if (pObj->GetCharInfo() && nBlueTeamCount > NUM_APPLYED_TEAMBONUS_TEAM_PLAYERS && nRedTeamCount > NUM_APPLYED_TEAMBONUS_TEAM_PLAYERS)
			{
				unsigned long int nGettingExp = (MMatchFormula::GetGettingExp(pObj->GetCharInfo()->m_nLevel, pObj->GetCharInfo()->m_nLevel) * 3);
				MMatchServer::GetInstance()->ApplyObjectTeamBonus(pObj, nGettingExp * 3);
				//			m_pStage->OnApplyTeamBonus(nTeam);
			}
			m_pStage->ResetTeamBonus();
			RouteAssignCap(nTeam);
			SendAssignState();
			//this is a cap
		}
		else
		{
			return;
		}
	}
	else
	{
		return; //haxxors!
	}

}


void MMatchRuleTeamCTF::RouteAssignFlag(MUID& uidFlagBearer, int nTeam)
{
	MCommand* pNew = MMatchServer::GetInstance()->CreateCommand(MC_MATCH_FLAG_EFFECT, MUID(0, 0));
	pNew->AddParameter(new MCmdParamUID(uidFlagBearer));
	pNew->AddParameter(new MCmdParamInt(nTeam));
	MMatchServer::GetInstance()->RouteToBattle(m_pStage->GetUID(), pNew);
}

void MMatchRuleTeamCTF::RouteAssignFlagToJoiner(MUID& uidFlagBearer, MUID& uidSendTo, int nTeam)
{
	MCommand* pNew = MMatchServer::GetInstance()->CreateCommand(MC_MATCH_FLAG_EFFECT, MUID(0, 0));
	pNew->AddParameter(new MCmdParamUID(uidFlagBearer));
	pNew->AddParameter(new MCmdParamInt(nTeam));
	MMatchServer::GetInstance()->RouteToObjInStage(m_pStage->GetUID(), uidSendTo, pNew);
}

void MMatchRuleTeamCTF::RouteAssignCap(int nTeam)
{
	MCommand* pNew = MMatchServer::GetInstance()->CreateCommand(MC_MATCH_FLAG_CAP, MUID(0, 0));
	pNew->AddParameter(new MCmdParamInt(nTeam));
	MMatchServer::GetInstance()->RouteToBattle(m_pStage->GetUID(), pNew);
}

void MMatchRuleTeamCTF::RouteAssignState(MUID uidSendTo)
{
	MMatchRuleTeamCTF* pTeamCTF = (MMatchRuleTeamCTF*)this;

	//Route Blue Flag
	MCommand* pCmdBlue = MMatchServer::GetInstance()->CreateCommand(MC_MATCH_FLAG_STATE, MUID(0, 0));
	pCmdBlue->AddParameter(new MCmdParamInt(CTF_BLUE_ITEM_ID));
	pCmdBlue->AddParameter(new MCmdParamShortVector(pTeamCTF->GetBlueFlagPosition().x, pTeamCTF->GetBlueFlagPosition().y, pTeamCTF->GetBlueFlagPosition().z));
	pCmdBlue->AddParameter(new MCmdParamInt(pTeamCTF->IsBlueFlagTaken()));
	pCmdBlue->AddParameter(new MCmdParamUID(pTeamCTF->GetBlueCarrier()));
	MMatchServer::GetInstance()->RouteToObjInStage(m_pStage->GetUID(), uidSendTo, pCmdBlue);

	//Route Red Flag
	MCommand* pCmdRed = MMatchServer::GetInstance()->CreateCommand(MC_MATCH_FLAG_STATE, MUID(0, 0));
	pCmdRed->AddParameter(new MCmdParamInt(CTF_RED_ITEM_ID));
	pCmdRed->AddParameter(new MCmdParamShortVector(pTeamCTF->GetRedFlagPosition().x, pTeamCTF->GetRedFlagPosition().y, pTeamCTF->GetRedFlagPosition().z));
	pCmdRed->AddParameter(new MCmdParamInt(pTeamCTF->IsRedFlagTaken()));
	pCmdRed->AddParameter(new MCmdParamUID(pTeamCTF->GetRedCarrier()));
	MMatchServer::GetInstance()->RouteToObjInStage(m_pStage->GetUID(), uidSendTo, pCmdRed);
}

void MMatchRuleTeamCTF::SendAssignState()
{
	MMatchRuleTeamCTF* pTeamCTF = (MMatchRuleTeamCTF*)this;

	//Route Blue Flag
	MCommand* pCmdBlue = MMatchServer::GetInstance()->CreateCommand(MC_MATCH_FLAG_STATE, MUID(0, 0));
	pCmdBlue->AddParameter(new MCmdParamInt(CTF_BLUE_ITEM_ID));
	pCmdBlue->AddParameter(new MCmdParamShortVector(pTeamCTF->GetBlueFlagPosition().x, pTeamCTF->GetBlueFlagPosition().y, pTeamCTF->GetBlueFlagPosition().z));
	pCmdBlue->AddParameter(new MCmdParamInt(pTeamCTF->IsBlueFlagTaken()));
	pCmdBlue->AddParameter(new MCmdParamUID(pTeamCTF->GetBlueCarrier()));
	MMatchServer::GetInstance()->RouteToBattle(m_pStage->GetUID(), pCmdBlue);

	//Route Red Flag
	MCommand* pCmdRed = MMatchServer::GetInstance()->CreateCommand(MC_MATCH_FLAG_STATE, MUID(0, 0));
	pCmdRed->AddParameter(new MCmdParamInt(CTF_RED_ITEM_ID));
	pCmdRed->AddParameter(new MCmdParamShortVector(pTeamCTF->GetRedFlagPosition().x, pTeamCTF->GetRedFlagPosition().y, pTeamCTF->GetRedFlagPosition().z));
	pCmdRed->AddParameter(new MCmdParamInt(pTeamCTF->IsRedFlagTaken()));
	pCmdRed->AddParameter(new MCmdParamUID(pTeamCTF->GetRedCarrier()));
	MMatchServer::GetInstance()->RouteToBattle(m_pStage->GetUID(), pCmdRed);
}

void MMatchRuleTeamCTF::OnLeaveBattle(MUID& uidChar)
{
	if (uidChar == GetBlueCarrier())
	{
		MUID m_uidBearer = MUID(0, 0);
		SetBlueCarrier(m_uidBearer);
		SetRedFlagObtained(false);
		m_pStage->m_WorldItemManager.ChangeFlagState(true, MMT_RED);
		RouteAssignFlag(m_uidBearer, MMT_BLUE);
	}

	else if (uidChar == GetRedCarrier())
	{
		MUID m_uidBearer = MUID(0, 0);
		SetRedCarrier(m_uidBearer);
		SetBlueFlagObtained(false);
		m_pStage->m_WorldItemManager.ChangeFlagState(true, MMT_BLUE);
		RouteAssignFlag(m_uidBearer, MMT_RED);
	}

	SendAssignState();
}

void MMatchRuleTeamCTF::OnEnterBattle(MUID& uidChar)
{
	RouteAssignFlagToJoiner(GetBlueCarrier(), uidChar, MMT_ALL);
	RouteAssignFlagToJoiner(GetRedCarrier(), uidChar, MMT_END);
	RouteAssignState(uidChar);
}


void MMatchRuleTeamCTF::OnGameKill(const MUID& uidAttacker, const MUID& uidVictim)
{
	MMatchObject* pVictim = MMatchServer::GetInstance()->GetObject(uidVictim);

	if (m_pStage != NULL)
	{
		MUID uidChar = pVictim->GetUID();

		if (uidChar == GetBlueCarrier())
		{
			MUID m_uidBearer = MUID(0, 0);
			SetBlueCarrier(m_uidBearer);
			SetRedFlagObtained(false);
			m_pStage->m_WorldItemManager.ChangeFlagState(true, MMT_RED);
			RouteAssignFlag(m_uidBearer, MMT_BLUE);
			SendAssignState();
		}

		else if (uidChar == GetRedCarrier())
		{
			MUID m_uidBearer = MUID(0, 0);
			SetRedCarrier(m_uidBearer);
			SetBlueFlagObtained(false);
			m_pStage->m_WorldItemManager.ChangeFlagState(true, MMT_BLUE);
			RouteAssignFlag(m_uidBearer, MMT_RED);
			SendAssignState();
		}
	}
}

bool MMatchRuleTeamCTF::OnCheckEnableBattleCondition()
{
	if (GetStage()->GetObjInBattleCount() >= 1)
		return true;

	return false;
}
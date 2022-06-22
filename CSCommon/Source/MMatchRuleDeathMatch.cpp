#include "stdafx.h"
#include "MMatchRuleDeathMatch.h"
#include "MMatchTransDataType.h"
#include "MBlobArray.h"
#include "MMatchFormula.h"
#include "MMatchServer.h"

// TEAM DEATH RULE ///////////////////////////////////////////////////////////////
MMatchRuleTeamDeath::MMatchRuleTeamDeath(MMatchStage* pStage) : MMatchRule(pStage)
{
}

void MMatchRuleTeamDeath::OnBegin()
{
}

void MMatchRuleTeamDeath::OnEnd()
{
}

bool MMatchRuleTeamDeath::OnRun()
{
	bool ret = MMatchRule::OnRun();


	return ret;
}

void MMatchRuleTeamDeath::OnRoundBegin()
{
	MMatchRule::OnRoundBegin();
}

void MMatchRuleTeamDeath::OnRoundEnd()
{
	if (m_pStage != NULL)
	{
		switch(m_nRoundArg)
		{
			case MMATCH_ROUNDRESULT_BLUE_ALL_OUT: m_pStage->OnRoundEnd_FromTeamGame(MMT_RED);break;
			case MMATCH_ROUNDRESULT_RED_ALL_OUT: m_pStage->OnRoundEnd_FromTeamGame(MMT_BLUE); break;
			case MMATCH_ROUNDRESULT_REDWON: m_pStage->OnRoundEnd_FromTeamGame(MMT_RED); break;
			case MMATCH_ROUNDRESULT_BLUEWON: m_pStage->OnRoundEnd_FromTeamGame(MMT_BLUE); break;
			case MMATCH_ROUNDRESULT_DRAW: break;
		}
	}

	MMatchRule::OnRoundEnd();
}

bool MMatchRuleTeamDeath::OnCheckEnableBattleCondition()
{
	// 선승제일 경우는 Free상태가 안된다.
	if (m_pStage->GetStageSetting()->IsTeamWinThePoint() == true)
	{
		return true;
	}

	int nRedTeam = 0, nBlueTeam = 0;
	int nPreRedTeam = 0, nPreBlueTeam = 0;
	int nStageObjects = 0;		// 게임안에 없고 스테이지에 있는 사람

	MMatchStage* pStage = GetStage();
	if (pStage == NULL) return false;

	for (MUIDRefCache::iterator i=pStage->GetObjBegin(); i!=pStage->GetObjEnd(); i++) 
	{
		MMatchObject* pObj = (MMatchObject*)(*i).second;
		if ((pObj->GetEnterBattle() == false) && (!pObj->IsLaunchedGame()))
		{
			nStageObjects++;
			continue;
		}

		if (pObj->GetTeam() == MMT_RED)
		{
			nRedTeam++;
		}
		else if (pObj->GetTeam() == MMT_BLUE)
		{
			nBlueTeam++;
		}
	}

	if ( nRedTeam == 0 || nBlueTeam == 0)
	{
		return false;
	}

	return true;
}

// 만약 레드팀이나 블루팀에서 팀원이 0명일 경우는 false 반환 , true,false 모두 AliveCount 반환
bool MMatchRuleTeamDeath::GetAliveCount(int* pRedAliveCount, int* pBlueAliveCount)
{
	int nRedCount = 0, nBlueCount = 0;
	int nRedAliveCount = 0, nBlueAliveCount = 0;
	(*pRedAliveCount) = 0;
	(*pBlueAliveCount) = 0;

	MMatchStage* pStage = GetStage();
	if (pStage == NULL) return false;

	for (MUIDRefCache::iterator i=pStage->GetObjBegin(); i!=pStage->GetObjEnd(); i++) 
	{
		MMatchObject* pObj = (MMatchObject*)(*i).second;
		if (pObj->GetEnterBattle() == false) continue;	// 배틀참가하고 있는 플레이어만 체크

		if (pObj->GetTeam() == MMT_RED)
		{
			nRedCount++;
			if (pObj->CheckAlive()==true)
			{
				nRedAliveCount++;
			}
		}
		else if (pObj->GetTeam() == MMT_BLUE)
		{
			nBlueCount++;
			if (pObj->CheckAlive()==true)
			{
				nBlueAliveCount++;
			}
		}
	}

	(*pRedAliveCount) = nRedAliveCount;
	(*pBlueAliveCount) = nBlueAliveCount;

	if ((nRedAliveCount == 0) || (nBlueAliveCount == 0))
	{
		return false;
	}
	return true;
}

bool MMatchRuleTeamDeath::OnCheckRoundFinish()
{
	int nRedAliveCount = 0;
	int nBlueAliveCount = 0;

	// 팀원이 0명인 팀이 있으면 false반환
	if (GetAliveCount(&nRedAliveCount, &nBlueAliveCount) == false)
	{
		int nRedTeam = 0, nBlueTeam = 0;
		int nStageObjects = 0;		// 게임안에 없고 스테이지에 있는 사람

		MMatchStage* pStage = GetStage();

		for (MUIDRefCache::iterator i=pStage->GetObjBegin(); i!=pStage->GetObjEnd(); i++) 
		{
			MMatchObject* pObj = (MMatchObject*)(*i).second;
			if ((pObj->GetEnterBattle() == false) && (!pObj->IsLaunchedGame()))
			{
				nStageObjects++;
				continue;
			}

			if (pObj->GetTeam() == MMT_RED)		nRedTeam++;
			else if (pObj->GetTeam() == MMT_BLUE)	nBlueTeam++;
		}

		if( nBlueTeam ==0 && (pStage->GetTeamScore(MMT_BLUE) > pStage->GetTeamScore(MMT_RED)) )
			SetRoundArg(MMATCH_ROUNDRESULT_BLUE_ALL_OUT);
		else if( nRedTeam ==0 && (pStage->GetTeamScore(MMT_RED) > pStage->GetTeamScore(MMT_BLUE)) )
			SetRoundArg(MMATCH_ROUNDRESULT_RED_ALL_OUT);
		else if ( (nRedAliveCount == 0) && (nBlueAliveCount == 0) )
			SetRoundArg(MMATCH_ROUNDRESULT_DRAW);
		else if (nRedAliveCount == 0)
			SetRoundArg(MMATCH_ROUNDRESULT_BLUEWON);
		else if (nBlueAliveCount == 0)
			SetRoundArg(MMATCH_ROUNDRESULT_REDWON);
	}

	if (nRedAliveCount==0 || nBlueAliveCount==0) return true;
	else return false;
}

void MMatchRuleTeamDeath::OnRoundTimeOut()
{
	int nRedAliveCount = 0;
	int nBlueAliveCount = 0;
	GetAliveCount(&nRedAliveCount, &nBlueAliveCount);

	if (nRedAliveCount > nBlueAliveCount)
		SetRoundArg(MMATCH_ROUNDRESULT_REDWON);
	else if (nBlueAliveCount > nRedAliveCount)
		SetRoundArg(MMATCH_ROUNDRESULT_BLUEWON);
	else SetRoundArg(MMATCH_ROUNDRESULT_DRAW);
}

// 반환값이 false이면 게임이 끝난다.
bool MMatchRuleTeamDeath::RoundCount() 
{
	if (m_pStage == NULL) return false;

	int nTotalRound = m_pStage->GetStageSetting()->GetRoundMax();
	m_nRoundCount++;

	if (m_pStage->GetStageSetting()->IsTeamWinThePoint() == false)
	{
		// 선승제가 아닐 경우
		if (m_nRoundCount < nTotalRound) return true;

	}
	else
	{
		// 선승제일 경우 

		// 팀원이 0명인 팀이 있어도 게임이 끝난다.
		int nRedTeamCount=0, nBlueTeamCount=0;
		m_pStage->GetTeamMemberCount(&nRedTeamCount, &nBlueTeamCount, NULL, true);

		if ((nRedTeamCount == 0) || (nBlueTeamCount == 0))
		{
			return false;
		}

		int nRedScore = m_pStage->GetTeamScore(MMT_RED);
		int nBlueScore = m_pStage->GetTeamScore(MMT_BLUE);
		
		// 래더게임에서 먼저 4승인 팀이 승리
		const int LADDER_WINNING_ROUNT_COUNT = 4;


		// 두팀이 모두 4승이 아니면 true반환
		if ((nRedScore < LADDER_WINNING_ROUNT_COUNT) && (nBlueScore < LADDER_WINNING_ROUNT_COUNT))
		{
			return true;
		}
	}

	return false;
}

void MMatchRuleTeamDeath::CalcTeamBonus(MMatchObject* pAttacker, MMatchObject* pVictim,
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

//////////////////////////////////////////////////////////////////////////////////
// MMatchRuleSoloDeath ///////////////////////////////////////////////////////////
MMatchRuleSoloDeath::MMatchRuleSoloDeath(MMatchStage* pStage) : MMatchRule(pStage)
{

}

void MMatchRuleSoloDeath::OnBegin()
{

}
void MMatchRuleSoloDeath::OnEnd()
{
}

bool MMatchRuleSoloDeath::RoundCount()
{
	if (++m_nRoundCount < 1) return true;
	return false;
}

bool MMatchRuleSoloDeath::CheckKillCount(MMatchObject* pOutObject)
{
	MMatchStage* pStage = GetStage();
	for (MUIDRefCache::iterator i=pStage->GetObjBegin(); i!=pStage->GetObjEnd(); i++) 
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

bool MMatchRuleSoloDeath::OnCheckRoundFinish()
{
	MMatchObject* pObject = NULL;

	if (CheckKillCount(pObject))
	{
		return true;
	}
	return false;
}

void MMatchRuleSoloDeath::OnRoundTimeOut()
{
	SetRoundArg(MMATCH_ROUNDRESULT_DRAW);
}




// 무한 팀데스매치 - 추가 by 동섭
//////////////////////////////////////////////////////////////////////////
MMatchRuleTeamDeath2::MMatchRuleTeamDeath2(MMatchStage* pStage) : MMatchRule(pStage)
{
}

void MMatchRuleTeamDeath2::OnBegin()
{
	m_pStage->InitTeamKills();
}

void MMatchRuleTeamDeath2::OnEnd()
{
}

bool MMatchRuleTeamDeath2::OnRun()
{
	bool ret = MMatchRule::OnRun();


	return ret;
}

void MMatchRuleTeamDeath2::OnRoundBegin()
{
	MMatchRule::OnRoundBegin();
}

void MMatchRuleTeamDeath2::OnRoundEnd()
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
void MMatchRuleTeamDeath2::GetTeamScore(int* pRedTeamScore, int* pBlueTeamScore)
{
	(*pRedTeamScore) = 0;
	(*pBlueTeamScore) = 0;

	MMatchStage* pStage = GetStage();
	if (pStage == NULL) return;

	(*pRedTeamScore) = pStage->GetTeamKills(MMT_RED);
	(*pBlueTeamScore) = pStage->GetTeamKills(MMT_BLUE);

	return;
}

bool MMatchRuleTeamDeath2::OnCheckRoundFinish()
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

void MMatchRuleTeamDeath2::OnRoundTimeOut()
{
	if (!OnCheckRoundFinish())
		SetRoundArg(MMATCH_ROUNDRESULT_DRAW);
}

// 반환값이 false이면 게임이 끝난다.
bool MMatchRuleTeamDeath2::RoundCount() 
{
	if (++m_nRoundCount < 1) return true;
	return false;
}

void MMatchRuleTeamDeath2::CalcTeamBonus(MMatchObject* pAttacker, MMatchObject* pVictim,
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




void MMatchRuleTeamDeath2::OnGameKill(const MUID& uidAttacker, const MUID& uidVictim)
{
	MMatchObject* pAttacker = MMatchServer::GetInstance()->GetObject(uidAttacker);
	MMatchObject* pVictim = MMatchServer::GetInstance()->GetObject(uidVictim);

	if (m_pStage != NULL)
	{
//		if (pAttacker->GetTeam() != pVictim->GetTeam())
//		{
//			m_pStage->AddTeamKills(pAttacker->GetTeam());
//		}

		m_pStage->AddTeamKills(pVictim->GetTeam() == MMT_BLUE ? MMT_RED : MMT_BLUE);		// 죽은사람 반대편팀 킬수 올림
	}
}
// 무한 팀데스매치 - 추가 by 동섭
//////////////////////////////////////////////////////////////////////////
MMatchRuleTeamCTF::MMatchRuleTeamCTF(MMatchStage* pStage) : MMatchRule(pStage)
{
	SetBlueFlagObtained(false);
	SetRedFlagObtained(false);
	SetBlueCarrier(MUID(0,0));
	SetRedCarrier(MUID(0,0));
}

void MMatchRuleTeamCTF::OnBegin()
{
	m_pStage->InitTeamKills();
}

void MMatchRuleTeamCTF::OnEnd()
{
	SetBlueFlagObtained(false);
	SetRedFlagObtained(false);
	SetBlueCarrier(MUID(0,0));
	SetRedCarrier(MUID(0,0));
}

bool MMatchRuleTeamCTF::OnRun()
{
	bool ret = MMatchRule::OnRun();


	return ret;
}

void MMatchRuleTeamCTF::OnRoundBegin()
{
	MMatchRule::OnRoundBegin();
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
	if (++m_nRoundCount < 1) return true;
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
	if( 0 == pObj )
		return;

	if (m_pStage == NULL)
	{
		SetBlueFlagObtained(false);
		SetRedFlagObtained(false);
		SetBlueCarrier(MUID(0,0));
		SetRedCarrier(MUID(0,0));
		return;
	}

	MMatchObject* pObtainer = pObj;
	MMatchTeam nTeam = pObj->GetTeam();

	int nBlueTeamCount = 0;
	int nRedTeamCount = 0;
	for (MUIDRefCache::iterator i=m_pStage->GetObjBegin(); i!=m_pStage->GetObjEnd(); i++) {
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

	if(MMT_BLUE == nTeam)
	{
		if(IsRedFlagTaken() == false && nItemID == CTF_RED_ITEM_ID)
		{
			MUID obtainerUID = pObtainer->GetUID();
			// this is a grab
			SetRedFlagObtained(true);
			m_pStage->m_WorldItemManager.ChangeFlagState(false, MMT_RED);
			SetBlueCarrier(obtainerUID);
			RouteAssignFlag(obtainerUID, nTeam);
			SendAssignState();
		}
		else if(IsBlueFlagTaken() == false && nItemID == CTF_BLUE_ITEM_ID && IsRedFlagTaken() == true)
		{
			SetBlueFlagObtained(false);
			SetRedFlagObtained(false);
			SetBlueCarrier(MUID(0,0));
			SetRedCarrier(MUID(0,0));
			m_pStage->AddTeamKills(nTeam);
			m_pStage->m_WorldItemManager.ChangeFlagState(true, MMT_RED);
			if(pObj->GetCharInfo() && nBlueTeamCount > NUM_APPLYED_TEAMBONUS_TEAM_PLAYERS && nRedTeamCount > NUM_APPLYED_TEAMBONUS_TEAM_PLAYERS)
			{
			unsigned long int nGettingExp = (MMatchFormula::GetGettingExp(pObj->GetCharInfo()->m_nLevel, pObj->GetCharInfo()->m_nLevel) * 3);
			MMatchServer::GetInstance()->ApplyObjectTeamBonus(pObj, nGettingExp * 3);
			m_pStage->OnApplyTeamBonus(nTeam);
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
	else if(MMT_RED == nTeam) //reverse logic
	{
		if(IsBlueFlagTaken() == false && nItemID == CTF_BLUE_ITEM_ID)
		{
			MUID obtainerUID = pObtainer->GetUID();
			// this is a grab
			SetBlueFlagObtained(true);
			m_pStage->m_WorldItemManager.ChangeFlagState(false, MMT_BLUE);
			SetRedCarrier(obtainerUID);
			RouteAssignFlag(obtainerUID, nTeam);
			SendAssignState();
		}
		else if(IsBlueFlagTaken() == true && nItemID == CTF_RED_ITEM_ID && IsRedFlagTaken() == false)
		{
			SetBlueFlagObtained(false);
			SetRedFlagObtained(false);
			SetBlueCarrier(MUID(0,0));
			SetRedCarrier(MUID(0,0));
			m_pStage->AddTeamKills(nTeam);
			m_pStage->m_WorldItemManager.ChangeFlagState(true, MMT_BLUE);
			if(pObj->GetCharInfo() && nBlueTeamCount > NUM_APPLYED_TEAMBONUS_TEAM_PLAYERS && nRedTeamCount > NUM_APPLYED_TEAMBONUS_TEAM_PLAYERS)
			{
			unsigned long int nGettingExp = (MMatchFormula::GetGettingExp(pObj->GetCharInfo()->m_nLevel, pObj->GetCharInfo()->m_nLevel) * 3);
			MMatchServer::GetInstance()->ApplyObjectTeamBonus(pObj, nGettingExp * 3);
			m_pStage->OnApplyTeamBonus(nTeam);
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
{	MCommand* pNew = MMatchServer::GetInstance()->CreateCommand(MC_MATCH_FLAG_EFFECT, MUID(0, 0));
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
{	MCommand* pNew = MMatchServer::GetInstance()->CreateCommand(MC_MATCH_FLAG_CAP, MUID(0, 0));
	pNew->AddParameter(new MCmdParamInt(nTeam));
	MMatchServer::GetInstance()->RouteToBattle(m_pStage->GetUID(), pNew);
}

void MMatchRuleTeamCTF::RouteAssignState(MUID uidSendTo)
{
			MMatchRuleTeamCTF* pTeamCTF = (MMatchRuleTeamCTF*)this;

			//Route Blue Flag
			MCommand* pCmdBlue = MMatchServer::GetInstance()->CreateCommand(MC_MATCH_FLAG_STATE, MUID(0,0));
			pCmdBlue->AddParameter(new MCmdParamInt(CTF_BLUE_ITEM_ID));
			pCmdBlue->AddParameter(new MCmdParamShortVector(pTeamCTF->GetBlueFlagPosition().x, pTeamCTF->GetBlueFlagPosition().y, pTeamCTF->GetBlueFlagPosition().z ));
			pCmdBlue->AddParameter(new MCmdParamInt(pTeamCTF->IsBlueFlagTaken()));
			pCmdBlue->AddParameter(new MCmdParamUID(pTeamCTF->GetBlueCarrier()));
			MMatchServer::GetInstance()->RouteToObjInStage(m_pStage->GetUID(), uidSendTo, pCmdBlue);

			//Route Red Flag
			MCommand* pCmdRed = MMatchServer::GetInstance()->CreateCommand(MC_MATCH_FLAG_STATE, MUID(0,0));
			pCmdRed->AddParameter(new MCmdParamInt(CTF_RED_ITEM_ID));
			pCmdRed->AddParameter(new MCmdParamShortVector(pTeamCTF->GetRedFlagPosition().x, pTeamCTF->GetRedFlagPosition().y, pTeamCTF->GetRedFlagPosition().z ));
			pCmdRed->AddParameter(new MCmdParamInt(pTeamCTF->IsRedFlagTaken()));
			pCmdRed->AddParameter(new MCmdParamUID(pTeamCTF->GetRedCarrier()));
			MMatchServer::GetInstance()->RouteToObjInStage(m_pStage->GetUID(), uidSendTo, pCmdRed);
}

void MMatchRuleTeamCTF::SendAssignState()
{
			MMatchRuleTeamCTF* pTeamCTF = (MMatchRuleTeamCTF*)this;

			//Route Blue Flag
			MCommand* pCmdBlue = MMatchServer::GetInstance()->CreateCommand(MC_MATCH_FLAG_STATE, MUID(0,0));
			pCmdBlue->AddParameter(new MCmdParamInt(CTF_BLUE_ITEM_ID));
			pCmdBlue->AddParameter(new MCmdParamShortVector(pTeamCTF->GetBlueFlagPosition().x, pTeamCTF->GetBlueFlagPosition().y, pTeamCTF->GetBlueFlagPosition().z ));
			pCmdBlue->AddParameter(new MCmdParamInt(pTeamCTF->IsBlueFlagTaken()));
			pCmdBlue->AddParameter(new MCmdParamUID(pTeamCTF->GetBlueCarrier()));
			MMatchServer::GetInstance()->RouteToBattle(m_pStage->GetUID(), pCmdBlue);

			//Route Red Flag
			MCommand* pCmdRed = MMatchServer::GetInstance()->CreateCommand(MC_MATCH_FLAG_STATE, MUID(0,0));
			pCmdRed->AddParameter(new MCmdParamInt(CTF_RED_ITEM_ID));
			pCmdRed->AddParameter(new MCmdParamShortVector(pTeamCTF->GetRedFlagPosition().x, pTeamCTF->GetRedFlagPosition().y, pTeamCTF->GetRedFlagPosition().z ));
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
///////////////////////////////////////////////////////////////////////////////////////////////////
// MMatchRulePaintBall ////////////////////////////////////////////////////////////////////////
MMatchRuleSoloPaintball::MMatchRuleSoloPaintball(MMatchStage* pStage) : MMatchRuleSoloDeath(pStage)
{
}

MMatchRuleTeamPaintball::MMatchRuleTeamPaintball(MMatchStage* pStage) : MMatchRuleTeamDeath(pStage)
{
}
//////////////////////////////////////////////////////////////////////////////////
// MMatchRuleModeStaff ///////////////////////////////////////////////////////////
MMatchRuleModeStaff::MMatchRuleModeStaff(MMatchStage* pStage) : MMatchRule(pStage)
{

}

void MMatchRuleModeStaff::OnBegin()
{

}
void MMatchRuleModeStaff::OnEnd()
{
}

bool MMatchRuleModeStaff::RoundCount()
{
	if (++m_nRoundCount < 1) return true;
	return false;
}

bool MMatchRuleModeStaff::CheckKillCount(MMatchObject* pOutObject)
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

bool MMatchRuleModeStaff::OnCheckRoundFinish()
{
	MMatchObject* pObject = NULL;

	if (CheckKillCount(pObject))
	{
		return true;
	}
	return false;
}

void MMatchRuleModeStaff::OnRoundTimeOut()
{
	SetRoundArg(MMATCH_ROUNDRESULT_DRAW);
}
// Infected Mode
// red -> infected
// blue -> survivors
#define INFECTED_MIN_PLAYER_COUNT 4

MMatchRuleTeamInfected::MMatchRuleTeamInfected(MMatchStage* pStage) : MMatchRule(pStage)
{
	m_bInfectionStarted = false;
}

void MMatchRuleTeamInfected::OnBegin()
{
}

void MMatchRuleTeamInfected::OnEnd()
{
}

void MMatchRuleTeamInfected::SetInfectedRoundState(MMATCH_ROUNDSTATE nState)
{
	if (m_nRoundState == nState) return;

	m_nRoundState = nState;
	SetRoundStateTimer(MMatchServer::GetInstance()->GetGlobalClockCount());

	if (nState == MMATCH_ROUNDSTATE_COUNTDOWN)
	{
		InitRound();
	}
	else if (nState == MMATCH_ROUNDSTATE_PRE_COUNTDOWN)
	{
		// don't know what to do with this now
		//OnPreCountDown();
	}
	else if (nState == MMATCH_ROUNDSTATE_PLAY)
	{
		OnRoundBegin();
	}
	else if (nState == MMATCH_ROUNDSTATE_FINISH)
	{
		OnRoundEnd();
	}
	else if (nState == MMATCH_ROUNDSTATE_FREE)
	{
		m_bInfectionStarted = false;
		m_uidSurvivors.clear();
		OnRoundEnd(); // force round end..
	}

	MMatchServer::GetInstance()->ResponseRoundState(GetStage()->GetUID());
}

bool MMatchRuleTeamInfected::OnRun()
{
	DWORD nClock = MMatchServer::GetInstance()->GetGlobalClockCount();

	switch (GetRoundState())
	{
	case MMATCH_ROUNDSTATE_PREPARE:
	{
		if (GetStage()->CheckBattleEntry() == true)
		{
			if (OnCheckEnableBattleCondition())
			{
				if (!m_bInfectionStarted)	SetInfectedRoundState(MMATCH_ROUNDSTATE_PRE_COUNTDOWN);
				else								SetInfectedRoundState(MMATCH_ROUNDSTATE_COUNTDOWN);
			}
			else									SetInfectedRoundState(MMATCH_ROUNDSTATE_FREE);
		}
		return true;

	}

	case MMATCH_ROUNDSTATE_PRE_COUNTDOWN:
	{
		DWORD dwPeriod = 3000;

		if (nClock - GetRoundStateTimer() > dwPeriod) { SetInfectedRoundState(MMATCH_ROUNDSTATE_COUNTDOWN); }
		return true;
	}

	case MMATCH_ROUNDSTATE_COUNTDOWN: {
		if (nClock - GetRoundStateTimer() > 2 * 1000) { SetInfectedRoundState(MMATCH_ROUNDSTATE_PLAY); }
		return true;
	}

	case MMATCH_ROUNDSTATE_PLAY: {
		// 게임하지 못할 상황이면 Free상태로 변환
		if (!OnCheckEnableBattleCondition()) { SetInfectedRoundState(MMATCH_ROUNDSTATE_FREE); }

		if (OnCheckRoundFinish())
		{
			SetInfectedRoundState(MMATCH_ROUNDSTATE_FINISH);
		}
		else if (OnCheckBattleTimeOut(nClock - GetRoundStateTimer()))
		{
			OnRoundTimeOut();
			SetInfectedRoundState(MMATCH_ROUNDSTATE_FINISH);
		}
		CheckOnGameEvent();
		return true;
	}

	case MMATCH_ROUNDSTATE_FINISH:
	{
		if (nClock - GetRoundStateTimer() > 3 * 1000) {
			if (RoundCount() == true)	{ SetInfectedRoundState(MMATCH_ROUNDSTATE_PREPARE); }
			else						{ SetInfectedRoundState(MMATCH_ROUNDSTATE_EXIT); }
		}
		return true;
	}

	case MMATCH_ROUNDSTATE_FREE: {
		if (OnCheckEnableBattleCondition()) { SetInfectedRoundState(MMATCH_ROUNDSTATE_PREPARE); }
		return true;
	}

	case MMATCH_ROUNDSTATE_EXIT: {
		return false;
	}

	default: {
		_ASSERT(0);
		return false;
	}
	}

	return false;
}

void MMatchRuleTeamInfected::OnRoundBegin()
{
	// push commandId: MC_MATCH_SET_COMMANDER
	// swap all users to other team
	MMatchServer* pServer = MMatchServer::GetInstance();
	MMatchStage* pStage = GetStage();
	if (pServer == NULL || pStage == NULL) return;

	// force a damn shuffle
	//pStage->ShuffleTeamMembers();

	m_uidPatientZero = ChooseInfected();
	if (m_uidPatientZero == MUID(0, 0))
	{
		// aww ;-;
		mlog("<Infected> m_uidPatientZero could not be chosen.\n");
		SetRoundState(MMATCH_ROUNDSTATE_FREE);
		return;
	}
	MMatchObject* pObj = MMatchServer::GetInstance()->GetObject(m_uidPatientZero);

	if (!IsEnabledObject(pObj))
	{
		mlog("<Infected> m_uidPatientZero chosen but disconnected, object handle lost?\n");
		SetRoundState(MMATCH_ROUNDSTATE_FREE);
		return;
	}

	pStage->SwitchPlayerTeamForInfected(m_uidPatientZero);

	for (MUIDRefCache::iterator i = pStage->GetObjBegin(); i != pStage->GetObjEnd(); i++)
	{
		MMatchObject* pObj = (MMatchObject*)(*i).second;

		if ((pObj->GetEnterBattle() == true) && (pObj->GetGameInfo()->bJoinedGame == true) && !pObj->CheckPlayerFlags(MTD_PlayerFlags_AdminHide))
		{
			if (pObj->CheckAlive() && pObj->GetTeam() == MMT_BLUE)
			{
				m_uidSurvivors.push_back(pObj->GetUID());
			}
		}
	}

	// fuck logic
	//if (pObj->GetTeam() != MMT_RED)
	//	pStage->SwitchPlayerTeam(m_uidPatientZero, false);

	//pStage->SwitchPlayerTeam(m_uidPatientZero, true);

	m_bInfectionStarted = true;

	// notify the infection
	MCommand* pCmd = pServer->CreateCommand(MC_MATCH_ASSIGN_COMMANDER, MUID(0, 0));
	pCmd->AddParameter(new MCmdParamUID(m_uidPatientZero));
	pCmd->AddParameter(new MCmdParamUID(MUID(0, 0))); // unused
	pServer->RouteToStage(pStage->GetUID(), pCmd);
}

void MMatchRuleTeamInfected::OnRoundEnd()
{
	if (m_pStage != NULL)
	{
		m_bInfectionStarted = false;
		m_pStage->SwitchPlayerTeamForInfected(m_uidPatientZero);

		switch (m_nRoundArg)
		{
		case MMATCH_ROUNDRESULT_BLUE_ALL_OUT: m_pStage->OnRoundEnd_FromTeamGame(MMT_RED); break;
		case MMATCH_ROUNDRESULT_RED_ALL_OUT: m_pStage->OnRoundEnd_FromTeamGame(MMT_BLUE); break;
		case MMATCH_ROUNDRESULT_REDWON: m_pStage->OnRoundEnd_FromTeamGame(MMT_RED); break;
		case MMATCH_ROUNDRESULT_BLUEWON: m_pStage->OnRoundEnd_FromTeamGame(MMT_BLUE); break;
		case MMATCH_ROUNDRESULT_DRAW: break;
		}

		m_uidSurvivors.clear();
		// do it!
		//m_pStage->ShuffleTeamMembers(); // shuffle next time
	}

	MMatchRule::OnRoundEnd();
}

const MUID MMatchRuleTeamInfected::ChooseInfected()
{
	MMatchStage* pStage = GetStage();
	if (pStage == NULL) return MUID(0, 0);

	int nRedAliveCount, nBlueAliveCount, nChooseTeamCount;
	if (GetAliveCount(&nRedAliveCount, &nBlueAliveCount) == false)
		return MUID(0, 0);

	if ((nRedAliveCount + nBlueAliveCount) < INFECTED_MIN_PLAYER_COUNT)
		return MUID(0, 0);

	nChooseTeamCount = nRedAliveCount + nBlueAliveCount;

	MTime time;
	int nChoose = time.MakeNumber(1, nChooseTeamCount);

	//if (m_PickHistory.size() > 4)
	//	m_PickHistory.pop_front();

	//m_PickHistory.push_back(nChoose);

	//// avoid similar instances. infinite loop lol
	//while (true)
	//{
	//	if (std::find(m_PickHistory.begin(), m_PickHistory.end(), nChoose) != m_PickHistory.end())
	//	{
	//		// seek forward
	//		if (nChoose < nChooseTeamCount)
	//			++nChoose;
	//		else if (nChoose > 0)
	//			--nChoose;
	//	}
	//	else
	//		break;
	//}

	int nCount = 0;
	for (MUIDRefCache::iterator itor = pStage->GetObjBegin(); itor != pStage->GetObjEnd(); itor++)
	{
		MMatchObject* pObj = (MMatchObject*)(*itor).second;
		if (pObj->GetEnterBattle() == false || pObj->CheckPlayerFlags(MTD_PlayerFlags_AdminHide)) continue;

		nCount++;
		if (nCount == nChoose)
			return pObj->GetUID();
	}

	return MUID(0, 0);
}

bool MMatchRuleTeamInfected::GetAliveCount(int* pRedAliveCount, int* pBlueAliveCount)
{
	int nRedCount = 0, nBlueCount = 0;
	int nRedAliveCount = 0, nBlueAliveCount = 0;
	(*pRedAliveCount) = 0;
	(*pBlueAliveCount) = 0;

	MMatchStage* pStage = GetStage();
	if (pStage == NULL) return false;

	for (MUIDRefCache::iterator i = pStage->GetObjBegin(); i != pStage->GetObjEnd(); i++)
	{
		MMatchObject* pObj = (MMatchObject*)(*i).second;
		if (pObj->GetEnterBattle() == false) continue;	// 배틀참가하고 있는 플레이어만 체크

		if (pObj->GetTeam() == MMT_RED)
		{
			nRedCount++;
			if (pObj->CheckAlive() == true)
			{
				nRedAliveCount++;
			}
		}
		else if (pObj->GetTeam() == MMT_BLUE)
		{
			nBlueCount++;
			if (pObj->CheckAlive() == true)
			{
				nBlueAliveCount++;
			}
		}
	}

	(*pRedAliveCount) = nRedAliveCount;
	(*pBlueAliveCount) = nBlueAliveCount;

	if (m_bInfectionStarted)
	{
		if (nRedAliveCount == 0 || nBlueAliveCount == 0)
			return false;

		//if (nRedAliveCount + nBlueAliveCount == 0)
		//	return false;
	}
	else
	{
		if (nRedAliveCount + nBlueAliveCount == 0 || (nRedAliveCount + nBlueAliveCount == 0 && m_uidPatientZero.IsValid()))
			return false;

	}

	//if (GetRoundState() != MMATCH_ROUNDSTATE_PLAY)
	//{
	//	if (nRedAliveCount + nBlueAliveCount == 0)
	//		return false;
	//}
	//else if ((nRedAliveCount == 0) || (nBlueAliveCount == 0))
	//{
	//	return false;
	//}
	return true;
}

bool MMatchRuleTeamInfected::OnCheckRoundFinish()
{
	int nRedAliveCount = 0;
	int nBlueAliveCount = 0;

	MMatchStage* pStage = GetStage();
	if (pStage == NULL)
	{
		SetRoundArg(MMATCH_ROUNDRESULT_DRAW);
		return true;
	}

	if (m_bInfectionStarted && GetAliveCount(&nRedAliveCount, &nBlueAliveCount) == false)
	{
		int nRedTeam = 0, nBlueTeam = 0;
		int nStageObjects = 0;

		for (MUIDRefCache::iterator i = pStage->GetObjBegin(); i != pStage->GetObjEnd(); i++)
		{
			MMatchObject* pObj = (MMatchObject*)(*i).second;
			if ((pObj->GetEnterBattle() == false) && (!pObj->IsLaunchedGame()))
			{
				nStageObjects++;
				continue;
			}

			if (pObj->GetTeam() == MMT_RED)		nRedTeam++;
			else if (pObj->GetTeam() == MMT_BLUE)	nBlueTeam++;
		}

		MMatchObject* pInfectedObj = MMatchServer::GetInstance()->GetObject(m_uidPatientZero);
		if ((pInfectedObj == NULL) || (pInfectedObj->GetStageUID() != pStage->GetUID()))
		{
			SetRoundArg(MMATCH_ROUNDRESULT_BLUEWON);
			return true;
		}

		if (pInfectedObj->CheckAlive() == false)
		{
			SetRoundArg(MMATCH_ROUNDRESULT_BLUEWON);
			return true;
		}

		if (nRedAliveCount == 0)
		{
			SetRoundArg(MMATCH_ROUNDRESULT_BLUEWON);
			return true;
		}
		else if (nBlueAliveCount == 0 && m_uidSurvivors.size() > 0)
		{
			SetRoundArg(MMATCH_ROUNDRESULT_REDWON);
			return true;
		}
		else
		{
			SetRoundArg(MMATCH_ROUNDRESULT_DRAW);
			return true;
		}
		//if( nBlueTeam ==0 && (pStage->GetTeamScore(MMT_BLUE) > pStage->GetTeamScore(MMT_RED)) )
		//	SetRoundArg(MMATCH_ROUNDRESULT_BLUE_ALL_OUT);
		//else if( nRedTeam ==0 && (pStage->GetTeamScore(MMT_RED) > pStage->GetTeamScore(MMT_BLUE)) )
		//	SetRoundArg(MMATCH_ROUNDRESULT_RED_ALL_OUT);
		//else if ( (nRedAliveCount == 0) && (nBlueAliveCount == 0) )
		//	SetRoundArg(MMATCH_ROUNDRESULT_DRAW);
		//else if (nRedAliveCount == 0)
		//	SetRoundArg(MMATCH_ROUNDRESULT_BLUEWON);
		//else if (nBlueAliveCount == 0)
		//	SetRoundArg(MMATCH_ROUNDRESULT_REDWON);
	}

	if (nRedAliveCount == 0 || nBlueAliveCount == 0) return true;
	else return false;
}

bool MMatchRuleTeamInfected::RoundCount()
{
	if (m_pStage == NULL) return false;

	int nTotalRound = m_pStage->GetStageSetting()->GetRoundMax();
	m_nRoundCount++;

	if (m_nRoundCount < nTotalRound) return true;

	return false;
}

void MMatchRuleTeamInfected::OnRoundTimeOut()
{
	int nRedAliveCount = 0;
	int nBlueAliveCount = 0;
	GetAliveCount(&nRedAliveCount, &nBlueAliveCount);

	// red never wins unless they kill all :D
	if (nBlueAliveCount > 0)
		SetRoundArg(MMATCH_ROUNDRESULT_BLUEWON);
	else
		SetRoundArg(MMATCH_ROUNDRESULT_DRAW);

	//if (nRedAliveCount > nBlueAliveCount)
	//	SetRoundArg(MMATCH_ROUNDRESULT_REDWON);
	//else if (nBlueAliveCount > nRedAliveCount)
	//	SetRoundArg(MMATCH_ROUNDRESULT_BLUEWON);
	//else SetRoundArg(MMATCH_ROUNDRESULT_DRAW);
}

bool MMatchRuleTeamInfected::OnCheckEnableBattleCondition()
{
	int nRedTeam = 0, nBlueTeam = 0;
	int nPreRedTeam = 0, nPreBlueTeam = 0;
	int nStageObjects = 0;

	MMatchStage* pStage = GetStage();
	if (pStage == NULL) return false;

	for (MUIDRefCache::iterator i = pStage->GetObjBegin(); i != pStage->GetObjEnd(); i++)
	{
		MMatchObject* pObj = (MMatchObject*)(*i).second;
		if ((pObj->GetEnterBattle() == false && !pObj->IsLaunchedGame()) || pObj->CheckPlayerFlags(MTD_PlayerFlags_AdminHide))
		{
			nStageObjects++;
			continue;
		}

		if (pObj->GetTeam() == MMT_RED)
		{
			nRedTeam++;
		}
		else if (pObj->GetTeam() == MMT_BLUE)
		{
			nBlueTeam++;
		}
	}

	if ((nRedTeam + nBlueTeam) < INFECTED_MIN_PLAYER_COUNT)
	{
		return false;
	}
	//if ( nRedTeam == 0 || nBlueTeam == 0)
	//{
	//	return false;
	//}

	return true;
}

void MMatchRuleTeamInfected::OnEnterBattle(MUID& uidChar)
{
	MMatchObject* pChar = MMatchServer::GetInstance()->GetObject(uidChar);
	if (!pChar) return;
	if (!IsEnabledObject(pChar)) return;

	if (m_pStage != NULL)
	{
		if (GetRoundState() == MMATCH_ROUNDSTATE_PLAY)
		{
			// todo: create a state sync packet :: just sync for now
			list<MUID> listInfected;
			for (MUIDRefCache::iterator i = m_pStage->GetObjBegin(); i != m_pStage->GetObjEnd(); i++)
			{
				MMatchObject* pObj = (MMatchObject*)(*i).second;
				if ((pObj->GetEnterBattle() == false && !pObj->IsLaunchedGame()) || pObj->CheckPlayerFlags(MTD_PlayerFlags_AdminHide))
				{
					continue;
				}

				if (pObj->GetTeam() == MMT_RED && pObj->CheckAlive() && pObj->GetUID() != m_uidPatientZero)
				{
					listInfected.push_back(pObj->GetUID());
				}
			}

			for (list<MUID>::iterator it = listInfected.begin(); it != listInfected.end(); ++it)
			{
				// TODO: a state blob
				MCommand* pNew = MMatchServer::GetInstance()->CreateCommand(MC_MATCH_INFECT, MUID(0, 0));
				pNew->AddParameter(new MCmdParamUID((*it)));
				MMatchServer::GetInstance()->RouteToListener(pChar, pNew);
			}
		}
	}
}

void MMatchRuleTeamInfected::OnLeaveBattle(MUID& uidChar)
{
	MMatchObject* pChar = MMatchServer::GetInstance()->GetObject(uidChar);

	if (m_pStage != NULL)
	{
		// remove player
		list<MUID>::iterator it = m_uidSurvivors.begin();

		while (it != m_uidSurvivors.end())
		{
			if ((*it) == uidChar)
			{
				it = m_uidSurvivors.erase(it);
				break;
			}
			else
				++it;
		}
	}
}

void MMatchRuleTeamInfected::OnGameKill(const MUID& uidAttacker, const MUID& uidVictim)
{
	MMatchObject* pAttacker = MMatchServer::GetInstance()->GetObject(uidAttacker);
	MMatchObject* pVictim = MMatchServer::GetInstance()->GetObject(uidVictim);

	if (m_pStage != NULL)
	{
		if (pAttacker != NULL && pVictim != NULL && pAttacker == pVictim)
		{
			return;
		}

		if (pVictim->GetTeam() != MMT_RED)
		{
			MUID uidAttacker = pAttacker ? pAttacker->GetUID() : MUID(0, 0);
			MUID uidVictim = pVictim ? pVictim->GetUID() : MUID(0, 0);
			OnInfectPlayer(uidAttacker, uidVictim);
		}
	}
}

void MMatchRuleTeamInfected::CalcTeamBonus(MMatchObject* pAttacker, MMatchObject* pVictim,
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

void MMatchRuleTeamInfected::OnInfectPlayer(const MUID& uidInfector, const MUID& uidVictim)
{
	MMatchObject* pInfector = MMatchServer::GetInstance()->GetObject(uidInfector);
	MMatchObject* pVictim = MMatchServer::GetInstance()->GetObject(uidVictim);

	if (!IsEnabledObject(pInfector) || !IsEnabledObject(pVictim))
		return;

	MCommand* pNew = MMatchServer::GetInstance()->CreateCommand(MC_MATCH_INFECT, MUID(0, 0));
	pNew->AddParameter(new MCmdParamUID(pVictim->GetUID()));
	MMatchServer::GetInstance()->RouteToBattle(m_pStage->GetUID(), pNew);

	m_pStage->SwitchPlayerTeam(uidVictim);
	pVictim->SetAlive(true);

	int nRedAliveCount = 0;
	int nBlueAliveCount = 0;

	if (GetAliveCount(&nRedAliveCount, &nBlueAliveCount) == false)
		return;

	if (nBlueAliveCount == 1)
	{
		MMatchStage* pStage = GetStage();

		if (!pStage)
			return;

		MUID uidLastSurvivor;
		for (MUIDRefCache::iterator i = pStage->GetObjBegin(); i != pStage->GetObjEnd(); i++)
		{
			MMatchObject* pObj = (MMatchObject*)(*i).second;
			if ((pObj->GetEnterBattle() == false && !pObj->IsLaunchedGame()) || pObj->CheckPlayerFlags(MTD_PlayerFlags_AdminHide))
				continue;

			if (pObj->GetTeam() == MMT_BLUE && pObj->CheckAlive())
			{
				uidLastSurvivor = pObj->GetUID();
				break;
			}
		}

		if (uidLastSurvivor.IsInvalid())
			return;

		pNew = MMatchServer::GetInstance()->CreateCommand(MC_MATCH_LASTSURVIVOR, MUID(0, 0));
		pNew->AddParameter(new MCmdParamUID(uidLastSurvivor));
		MMatchServer::GetInstance()->RouteToBattle(m_pStage->GetUID(), pNew);
	}
}

void MMatchRuleTeamInfected::OnCommand(MCommand* pCommand)
{
	MMatchRule::OnCommand(pCommand);

	switch (pCommand->GetID())
	{
	case MC_MATCH_REQUEST_INFECT:
	{
		MUID uidInfector, uidVictim;

		pCommand->GetParameter(&uidInfector, 0, MPT_UID);
		uidVictim = pCommand->GetSenderUID();

		OnInfectPlayer(uidInfector, uidVictim);
	}
	break;
	}
}


void* MMatchRuleTeamInfected::CreateRuleInfoBlob()
{
	void* pRuleInfoArray = MMakeBlobArray(sizeof(MTD_RuleInfo_Assassinate), 1);
	MTD_RuleInfo_Assassinate* pRuleItem = (MTD_RuleInfo_Assassinate*)MGetBlobArrayElement(pRuleInfoArray, 0);
	memset(pRuleItem, 0, sizeof(MTD_RuleInfo_Assassinate));

	pRuleItem->nRuleType = MMATCH_GAMETYPE_INFECTED;
	pRuleItem->uidRedCommander = m_uidPatientZero;
	pRuleItem->uidBlueCommander = MUID(0, 0);

	return pRuleInfoArray;
}

// MMatchRuleGunMode ///////////////////////////////////////////////////////////
MMatchRuleGunMode::MMatchRuleGunMode(MMatchStage* pStage) : MMatchRule(pStage)
{

}

void MMatchRuleGunMode::OnBegin()
{

}
void MMatchRuleGunMode::OnEnd()
{
}

bool MMatchRuleGunMode::RoundCount()
{
	if (++m_nRoundCount < 1) return true;
	return false;
}

bool MMatchRuleGunMode::CheckKillCount(MMatchObject* pOutObject)
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

bool MMatchRuleGunMode::OnCheckRoundFinish()
{
	MMatchObject* pObject = NULL;

	if (CheckKillCount(pObject))
	{
		return true;
	}
	return false;
}

void MMatchRuleGunMode::OnRoundTimeOut()
{
	SetRoundArg(MMATCH_ROUNDRESULT_DRAW);
}

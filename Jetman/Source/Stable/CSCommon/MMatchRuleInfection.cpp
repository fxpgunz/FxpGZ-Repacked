#include "stdafx.h"
#include "MMatchRuleInfection.h"
#include <iterator>

MMatchRuleInfection::MMatchRuleInfection(MMatchStage* pStage) : MMatchRule(pStage)
{
	m_uidFirstZombie.SetInvalid();
}

MMatchRuleInfection::~MMatchRuleInfection()
{
	m_uidFirstZombie.SetInvalid();
}

void MMatchRuleInfection::OnEnterBattle(MUID& uidPlayer)
{
	///TODO: fix so latejoiners see items correctly
	for (auto itor = GetStage()->GetObjBegin(); itor != GetStage()->GetObjEnd(); ++itor)
	{
		MMatchObject* pObj = (MMatchObject*)(*itor).second;
		if (pObj->GetUID() == uidPlayer)
			continue;

		if (pObj->GetTeam() != MMT_RED)
			continue;

		MCommand* pCmd = MGetMatchServer()->CreateCommand(MC_MATCH_INFECTION_LATEJOIN, MUID(0, 0));
		pCmd->AddParameter(new MCmdParamUID(uidPlayer));
		pCmd->AddParameter(new MCmdParamUID(pObj->GetUID()));
		pCmd->AddParameter(new MCmdParamBool(pObj->GetUID() == m_uidFirstZombie));

		MGetMatchServer()->RouteToBattle(GetStage()->GetUID(), pCmd);

	}
}

void MMatchRuleInfection::OnRoundBegin()
{

	OnInfectPlayer(m_uidFirstZombie,true);
}

void MMatchRuleInfection::OnRoundEnd()
{
	m_uidFirstZombie.SetZero();
	MMatchRule::OnRoundEnd();
}

void MMatchRuleInfection::OnRoundCountDown()
{
	auto it = GetStage()->GetObjBegin();
	std::advance(it, rand() % GetStage()->GetObjInBattleCount());
	m_uidFirstZombie = (*it).first;

	//todo: replace with range-based forloop.
	for (auto itor = GetStage()->GetObjBegin(); itor != GetStage()->GetObjEnd(); ++itor)
	{
		MMatchObject* pObj = static_cast<MMatchObject*>((*itor).second);
		if (pObj == nullptr || pObj->GetPlace() != MMP_BATTLE)
			continue;
		GetStage()->PlayerTeam(pObj->GetUID(), MMT_BLUE);
	}
}

void MMatchRuleInfection::OnRoundTimeOut()
{
	SetRoundArg(MMATCH_ROUNDRESULT_BLUEWON);
}

void MMatchRuleInfection::OnInfectPlayer(const MUID& uidVictim, bool bFirstInfect)
{
	int hp = 0, ap = 0;

	if (bFirstInfect) {
		hp = 500;
		ap = 250;
	}
	else {
		hp = 200;
		ap = 150;
	}
	GetStage()->PlayerTeam(uidVictim, MMT_RED);

	if (!GetStage()->GetObj(uidVictim)->CheckAlive())
	{
		GetStage()->GetObj(uidVictim)->SetAlive(true);
	}

	MCommand* pCmd = MGetMatchServer()->CreateCommand(MC_MATCH_INFECT_PLAYER, MUID(0, 0));
	pCmd->AddParameter(new MCmdParamUID(uidVictim));
	pCmd->AddParameter(new MCmdParamInt(hp));
	pCmd->AddParameter(new MCmdParamInt(ap));
	pCmd->AddParameter(new MCmdParamBool(bFirstInfect));

	MGetMatchServer()->RouteToBattle(GetStage()->GetUID(), pCmd);
}

void MMatchRuleInfection::OnGameKill(const MUID& uidAttacker, const MUID& uidVictim)
{
	MMatchObject* pAttacker = MMatchServer::GetInstance()->GetObject(uidAttacker);
	MMatchObject* pVictim = MMatchServer::GetInstance()->GetObject(uidVictim);

	if (pAttacker == nullptr || pVictim == nullptr)
		return;
	if (pAttacker == pVictim) return;

	if (pVictim->GetTeam() == MMT_RED)
		return;

	OnInfectPlayer(pVictim->GetUID());
}

bool MMatchRuleInfection::OnCheckRoundFinish()
{
	int nRedAliveCount = 0;
	int nBlueAliveCount = 0;

	if (GetAliveCount(&nRedAliveCount, &nBlueAliveCount) == false)
	{
		int nRedTeam = 0, nBlueTeam = 0;

		for (auto itor = GetStage()->GetObjBegin(); itor != GetStage()->GetObjEnd(); ++itor)
		{
			MMatchObject* pObj = (MMatchObject*)(*itor).second;
			if ((pObj->GetEnterBattle() == false) && (!pObj->IsLaunchedGame()))
				continue;

			if (pObj->GetTeam() == MMT_RED)		nRedTeam++;
			else if (pObj->GetTeam() == MMT_BLUE)	nBlueTeam++;
		}

		if (nBlueTeam == 0 && (GetStage()->GetTeamScore(MMT_BLUE) > GetStage()->GetTeamScore(MMT_RED)))
			SetRoundArg(MMATCH_ROUNDRESULT_BLUE_ALL_OUT);
		else if (nRedTeam == 0 && (GetStage()->GetTeamScore(MMT_RED) > GetStage()->GetTeamScore(MMT_BLUE)))
			SetRoundArg(MMATCH_ROUNDRESULT_RED_ALL_OUT);
		else if ((nRedAliveCount == 0) && (nBlueAliveCount == 0))
			SetRoundArg(MMATCH_ROUNDRESULT_DRAW);
		else if (nRedAliveCount == 0)
			SetRoundArg(MMATCH_ROUNDRESULT_BLUEWON);
		else if (nBlueAliveCount == 0)
			SetRoundArg(MMATCH_ROUNDRESULT_REDWON);
	}

	if (nRedAliveCount == 0 || nBlueAliveCount == 0) return true;
	else return false;
}

bool MMatchRuleInfection::GetAliveCount(int* pRedAliveCount, int* pBlueAliveCount)
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

	if ((nRedAliveCount == 0) || (nBlueAliveCount == 0))
	{
		return false;
	}
	return true;
}

bool MMatchRuleInfection::OnCheckEnableBattleCondition()
{
	if (GetStage()->GetObjInBattleCount() < 2)
		return false;

	return true;
}

bool MMatchRuleInfection::RoundCount()
{
	if (m_pStage == NULL) return false;

	int nTotalRound = m_pStage->GetStageSetting()->GetRoundMax();
	m_nRoundCount++;

	if (m_nRoundCount < nTotalRound) return true;

	return false;
}

bool MMatchRuleInfection::OnRun()
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
			}
			return true;
		}
		case MMATCH_ROUNDSTATE_COUNTDOWN:
		{
			if (nClock - GetRoundStateTimer() > 7 * 1000)
			{
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
			if (OnCheckBattleTimeOut(nClock - GetRoundStateTimer()))
			{
				OnRoundTimeOut();
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
		{
			return true;
		}
	}
	return true;
}
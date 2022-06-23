#include "stdafx.h"
#include "MMatchRuleSpyMode.h"
#include "SpyMode.h"
#include <iterator>
#include "MBlobArray.h"

MMatchRuleSpyMode::MMatchRuleSpyMode(MMatchStage* pStage) : MMatchRule(pStage)
{
}

MMatchRuleSpyMode::~MMatchRuleSpyMode()
{

}

void MMatchRuleSpyMode::OnBegin()
{
}


void MMatchRuleSpyMode::OnEnd()
{
}

void MMatchRuleSpyMode::OnRoundBegin()
{
	winningTeam = MMT_ALL;//0
	SendWeapons();
}

void MMatchRuleSpyMode::OnRoundEnd()
{
	AddWinningTeamReward(winningTeam);
	Spies.clear();
	MMatchRule::OnRoundEnd();
}

void MMatchRuleSpyMode::OnRoundCountDown()
{
	m_SpyCount = 0;
	ChooseSpies();
}

void MMatchRuleSpyMode::OnEnterBattle(MUID& uidPlayer)
{

	for (auto itor = GetStage()->GetObjBegin(); itor != GetStage()->GetObjEnd(); ++itor)
	{
		MMatchObject* pObj = (MMatchObject*)(*itor).second;
		if (pObj->GetUID() == uidPlayer)
			continue;
		//TODO:
		//SendCurrWeapons(pObj->GetUID(), uidPlayer);
	}
}

void MMatchRuleSpyMode::OnLeaveBattle(MUID& uidPlayer)
{
	auto it = std::find(Spies.begin(), Spies.end(), uidPlayer);

	if (it != Spies.end())
		Spies.erase(it);
}

void MMatchRuleSpyMode::ChooseSpies()
{
	int SpyCount = MGetSpyMode()->GetSpyTableElement(GetStage()->GetObjInBattleCount()).spyPlayers;
	if (SpyCount == 0)
		return;

	vector<MUID> randomSpies;
	for (auto itor = GetStage()->GetObjBegin(); itor != GetStage()->GetObjEnd(); ++itor)
	{
		MMatchObject* pObj = static_cast<MMatchObject*>((*itor).second);
		if (pObj->GetPlace() != MMP_BATTLE || pObj == nullptr)
			continue;

		randomSpies.push_back((*itor).first);
	}
	std::random_shuffle(randomSpies.begin(), randomSpies.end());

	for (int i = 0; i < SpyCount; ++i)
	{
		Spies.push_back(randomSpies[i]);
	}
	
	m_SpyCount = SpyCount;

	for (auto itor = GetStage()->GetObjBegin(); itor != GetStage()->GetObjEnd(); ++itor)
	{
		MMatchObject* pObj = (MMatchObject*)(*itor).second;

		if (IsSpy(pObj->GetUID()))
			GetStage()->PlayerTeam(pObj->GetUID(), MMT_RED);
		else
			GetStage()->PlayerTeam(pObj->GetUID(), MMT_BLUE);
	}

	
	void* SpyUIDBlob = MMakeBlobArray(sizeof(MUID), (int)Spies.size());

	for (int i = 0; i < (int)Spies.size(); ++i)
	{
		*(MUID*)MGetBlobArrayElement(SpyUIDBlob, i) = Spies[i];
	}

	int hpApBonus = MGetSpyMode()->GetSpyTableElement(GetStage()->GetObjInBattleCount()).hpap;


	MCommand* pCmd = MGetMatchServer()->CreateCommand(MC_MATCH_SPY_TEAMS, MUID(0, 0));
	pCmd->AddParameter(new MCmdParamBlob(SpyUIDBlob, MGetBlobArraySize(SpyUIDBlob)));
	pCmd->AddParameter(new MCmdParamInt(hpApBonus));
	MGetMatchServer()->RouteToBattle(GetStage()->GetUID(), pCmd);
	MEraseBlobArray(SpyUIDBlob);
}

void MMatchRuleSpyMode::SendWeapons()
{
	SpyPlayerTable ItemTable = MGetSpyMode()->GetSpyTableElement(GetStage()->GetObjInBattleCount());
	void* ItemBlob = MMakeBlobArray(sizeof(MTD_SpyItems), (int)MGetSpyMode()->GetSpyItems().size());
	for (int i = 0; i < MGetBlobArrayCount(ItemBlob); ++i)
	{
		MTD_SpyItems Items = {}; 
		Items.id = MGetSpyMode()->GetSpyItems()[i].ID;
		switch (Items.id)
		{
		case 601006:
			Items.count = 1; break;
		case 601001:
			Items.count = ItemTable.lightCount; break;
		case 601003:
			Items.count = ItemTable.iceCount; break;
		case 601002:
			Items.count = ItemTable.smokeCount; break;
		case 601004:
			Items.count = MGetSpyMode()->GetTracerTable().stunCount; break;
		case 601005:
			Items.count = MGetSpyMode()->GetTracerTable().mineCount; break;
		}
		strcpy_s(Items.desc, MGetSpyMode()->GetSpyItems()[i].Desc);
		*(MTD_SpyItems*)MGetBlobArrayElement(ItemBlob, i) = Items;
	}
	MCommand* Info = MGetMatchServer()->CreateCommand(MC_MATCH_SPY_INFO, MUID(0, 0));
	Info->AddParameter(new MCmdParamBlob(ItemBlob, MGetBlobArraySize(ItemBlob)));


	MGetMatchServer()->RouteToBattle(GetStage()->GetUID(), Info);
	MEraseBlobArray(ItemBlob);
}

void MMatchRuleSpyMode::OnRoundTimeOut()
{
	SetRoundArg(MMATCH_ROUNDRESULT_REDWON);
}

void MMatchRuleSpyMode::AddWinningTeamReward(MMatchTeam winningTeam)
{
	for (auto itor = GetStage()->GetObjBegin(); itor != GetStage()->GetObjEnd(); ++itor)
	{
		MMatchObject* player = (MMatchObject*)((*itor).second);
		///TODO: calculate the score value to be added

		if (player->GetTeam() == winningTeam)
		{
			MCommand* command = MGetMatchServer()->CreateCommand(MC_MATCH_SPY_ROUNDREWARD, MUID(0, 0));
			command->AddParameter(new MCmdParamUID(player->GetUID()));
			command->AddParameter(new MCmdParamUChar(128));//TODO: actual value?

			MGetMatchServer()->RouteToBattle(GetStage()->GetUID(), command);
		}
	}
}

bool MMatchRuleSpyMode::IsSpy(const MUID& uidPlayer)
{
	if (std::find(Spies.begin(), Spies.end(), uidPlayer) != Spies.end())
		return true;

	return false;
}

bool MMatchRuleSpyMode::OnCheckEnableBattleCondition()
{
	if (GetStage()->GetObjInBattleCount() < 4)
		return false;

	return true;
}

bool MMatchRuleSpyMode::OnRun()
{
	DWORD nClock = MMatchServer::GetInstance()->GetGlobalClockCount();

	switch (GetRoundState())
	{
		case MMATCH_ROUNDSTATE_PREPARE:
		{
			if (GetStage()->CheckBattleEntry())
			{
				if (OnCheckEnableBattleCondition())
				{
					SetRoundState(MMATCH_ROUNDSTATE_COUNTDOWN);
				}
				else
				{
					SetRoundState(MMATCH_ROUNDSTATE_FREE);
				}
				return true;
			}
			else
			{
				SetRoundState(MMATCH_ROUNDSTATE_FREE);
				return true;
			}
			return true;
		}
		case MMATCH_ROUNDSTATE_COUNTDOWN:
		{
			if (OnCheckEnableBattleCondition())
			{
				if (nClock - GetRoundStateTimer() >= static_cast<unsigned long>((MGetSpyMode()->GetSpySelect().spySelectTime * 1000)))
				{
					SetRoundState(MMATCH_ROUNDSTATE_PLAY);

					return true;
				}
			}
			else
			{
				SetRoundState(MMATCH_ROUNDSTATE_FREE);
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
			if(OnCheckRoundFinish())
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
			if (nClock - GetRoundStateTimer() >= static_cast<unsigned long>((MGetSpyMode()->GetSpySelect().roundFinishTime * 1000)))
			{
				if (RoundCount())
				{
					SetRoundState(MMATCH_ROUNDSTATE_PREPARE);
					return true;
				}
				else
				{
					SetRoundState(MMATCH_ROUNDSTATE_EXIT);
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
		case MMATCH_ROUNDSTATE_EXIT:
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

bool MMatchRuleSpyMode::GetAliveCount(int* blueCount,int redCount)
{

	int nBlueAliveCount = 0, blueTeam = 0;
	(*blueCount) = 0;
	MMatchStage* pStage = GetStage();
	if (pStage == NULL) return false;

	for (MUIDRefCache::iterator i = pStage->GetObjBegin(); i != pStage->GetObjEnd(); i++)
	{
		MMatchObject* pObj = (MMatchObject*)(*i).second;
		if (pObj->GetEnterBattle() == false) continue;	// 배틀참가하고 있는 플레이어만 체크

		if (pObj->GetTeam() == MMT_BLUE)
		{
			++blueTeam;
			if (pObj->CheckAlive() == true)
			{
				nBlueAliveCount++;
			}
		}
	}

	(*blueCount) = nBlueAliveCount;


	if (blueTeam == 0 && (GetStage()->GetTeamScore(MMT_BLUE) > GetStage()->GetTeamScore(MMT_RED)))
		SetRoundArg(MMATCH_ROUNDRESULT_BLUE_ALL_OUT);
	else if (redCount == 0 && (GetStage()->GetTeamScore(MMT_RED) > GetStage()->GetTeamScore(MMT_BLUE)))
		SetRoundArg(MMATCH_ROUNDRESULT_RED_ALL_OUT);
	else if ((redCount == 0) && (nBlueAliveCount == 0))
		SetRoundArg(MMATCH_ROUNDRESULT_DRAW);
	else if (redCount == 0)
		SetRoundArg(MMATCH_ROUNDRESULT_BLUEWON);
	else if (nBlueAliveCount == 0)
		SetRoundArg(MMATCH_ROUNDRESULT_REDWON);

	if (redCount == 0)
		winningTeam = MMT_BLUE;
	else
		winningTeam = MMT_RED;

	if (redCount == 0 || blueCount == 0)
		return false;


	return true;
}

bool MMatchRuleSpyMode::OnCheckRoundFinish()
{
	int redAliveCount = 0;
	for (int i = 0; i < m_SpyCount; ++i)
	{
		MMatchObject* pObj = (MMatchObject*)MGetMatchServer()->GetObject(Spies[i]);
		if (pObj->CheckAlive())
		{
			++redAliveCount;
		}
	}

	int blueAliveCount = 0;
	if (!GetAliveCount(&blueAliveCount,redAliveCount))
		return true;

	return false;
}

bool MMatchRuleSpyMode::RoundCount()
{
	if (m_pStage == NULL) return false;

	int nTotalRound = m_pStage->GetStageSetting()->GetRoundMax();
	m_nRoundCount++;

	if (m_nRoundCount < nTotalRound) return true;

	return false;
}
#include "stdafx.h"
#include "MMatchRuleDemolition.h"


///TODO: finish writing this!

MMatchRuleDemolition::MMatchRuleDemolition(MMatchStage* stage) : MMatchRule(stage)
{
}

MMatchRuleDemolition::~MMatchRuleDemolition()
{

}

void MMatchRuleDemolition::OnBegin()
{
	for (int i = 0; i < 2; ++i)
	{
		isBombPlanted[i] = false;
		bombPlantedTime[i] = 0;
		bombOwner[i].SetInvalid();
	}
}

void MMatchRuleDemolition::OnEnd()
{
	for (int i = 0; i < 2; ++i)
	{
		isBombPlanted[i] = false;
		bombPlantedTime[i] = 0;
		bombOwner[i].SetInvalid();
	}
}

void MMatchRuleDemolition::OnRoundBegin()
{
	for (int i = 0; i < 2; ++i)
	{
		isBombPlanted[i] = false;
		bombPlantedTime[i] = 0;
		bombOwner[i].SetInvalid();
	}
}

void MMatchRuleDemolition::OnRoundEnd()
{
	for (int i = 0; i < 2; ++i)
	{
		isBombPlanted[i] = false;
		bombPlantedTime[i] = 0;
		bombOwner[i].SetInvalid();
	}
}

void MMatchRuleDemolition::OnEnterBattle(MUID& player)
{

}

void MMatchRuleDemolition::OnLeaveBattle(MUID& player)
{
	if (player == bombOwner[0])
	{
		bombOwner[0].SetInvalid();
		isBombPlanted[0] = false;
		bombPlantedTime[0] = 0;
	}
	if (player == bombOwner[1])
	{
		bombOwner[1].SetInvalid();
		isBombPlanted[1] = false;
		bombPlantedTime[1] = 0;
	}

	MMatchObject* playerObj = MGetMatchServer()->GetObject(player);

	MCommand* pCmd = MGetMatchServer()->CreateCommand(MC_MATCH_RESPONSE_PLANT_BOMB, MUID(0, 0));
	pCmd->AddParameter(new MCmdParamInt((int)playerObj->GetTeam()));
	pCmd->AddParameter(new MCmdParamBool(false));

	MGetMatchServer()->RouteToBattle(GetStage()->GetUID(), pCmd);
}

void MMatchRuleDemolition::OnRoundTimeOut()
{
	SetRoundArg(MMATCH_ROUNDRESULT_DRAW);
}

void MMatchRuleDemolition::OnCommand(MCommand* command)
{

}

void MMatchRuleDemolition::OnPlantBomb(const MUID& attacker)
{
	MMatchObject* player = MGetMatchServer()->GetObject(attacker);
	if (player == nullptr)
		return;

	if (player->GetTeam() == MMT_RED)
	{
		isBombPlanted[0] = true;
		bombOwner[0] = player->GetUID();
	}
	else if (player->GetTeam() == MMT_BLUE)
	{
		isBombPlanted[1] = true;
		bombOwner[1] = player->GetUID();
	}

	MCommand* pCmd = MGetMatchServer()->CreateCommand(MC_MATCH_RESPONSE_PLANT_BOMB, MUID(0, 0));
	pCmd->AddParameter(new MCmdParamInt((int)player->GetTeam()));
	pCmd->AddParameter(new MCmdParamBool(true));

	MGetMatchServer()->RouteToBattle(GetStage()->GetUID(), pCmd);

}

void MMatchRuleDemolition::OnGameKill(const MUID& uidAttacker, const MUID& uidVictim)
{
	MMatchObject* player = MGetMatchServer()->GetObject(uidVictim);
	if (player == nullptr)
		return;

	if (player->GetTeam() == MMT_RED)
	{
		if (player->GetUID() == bombOwner[0])
		{
			isBombPlanted[0] = false;
			bombPlantedTime[0] = 0;
			bombOwner[0].SetInvalid();
			MCommand* pCmd = MGetMatchServer()->CreateCommand(MC_MATCH_RESPONSE_PLANT_BOMB, MUID(0, 0));
			pCmd->AddParameter(new MCmdParamInt((int)player->GetTeam()));
			pCmd->AddParameter(new MCmdParamBool(false));
		}
	}

	else if (player->GetTeam() == MMT_BLUE)
	{
		if (player->GetUID() == bombOwner[1])
		{
			isBombPlanted[1] = false;
			bombPlantedTime[1] = 0;
			bombOwner[1].SetInvalid();
		}
		MCommand* pCmd = MGetMatchServer()->CreateCommand(MC_MATCH_RESPONSE_PLANT_BOMB, MUID(0, 0));
		pCmd->AddParameter(new MCmdParamInt((int)player->GetTeam()));
		pCmd->AddParameter(new MCmdParamBool(false));
	}


}

void MMatchRuleDemolition::Tick(unsigned long tickTime)
{
	for (int i = 0; i < 2; ++i)
	{
		if (isBombPlanted[i] == true)
		{
			bombPlantedTime[i] += tickTime * 1000;
		}
	}
}

bool MMatchRuleDemolition::OnRun()
{
	DWORD currTime = MGetMatchServer()->GetGlobalClockCount();

	switch (GetRoundState())
	{
		case MMATCH_ROUNDSTATE_PREPARE:
		{
			if (OnCheckEnableBattleCondition())
				SetRoundState(MMATCH_ROUNDSTATE_COUNTDOWN);
			else
				SetRoundState(MMATCH_ROUNDSTATE_FREE);
		}return true;
		case MMATCH_ROUNDSTATE_COUNTDOWN:
		{
			if (currTime - GetRoundStateTimer() > 7 * 1000)
				SetRoundState(MMATCH_ROUNDSTATE_PLAY);
		}return true;
		case MMATCH_ROUNDSTATE_PLAY:
		{
			if (OnCheckRoundFinish() && currTime - GetRoundStateTimer() > 10000)
				SetRoundState(MMATCH_ROUNDSTATE_FINISH);
			if (!OnCheckEnableBattleCondition())
				SetRoundState(MMATCH_ROUNDSTATE_FREE);
			
		}return true;
		case MMATCH_ROUNDSTATE_FINISH:
		{
			if (currTime - GetRoundStateTimer() > 3000)
			{
				if (RoundCount())
					SetRoundState(MMATCH_ROUNDSTATE_PREPARE);
				else
					SetRoundState(MMATCH_ROUNDSTATE_EXIT);
			}
		}return true;
		case MMATCH_ROUNDSTATE_FREE:
		{
			if (OnCheckEnableBattleCondition())
				SetRoundState(MMATCH_ROUNDSTATE_PREPARE);
		}return true;
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

//TODO: Change >= 1
bool MMatchRuleDemolition::OnCheckEnableBattleCondition()
{
	return GetStage()->GetObjInBattleCount() >= 1;
}

bool MMatchRuleDemolition::OnCheckRoundFinish()
{
	if (isBombPlanted[0] && isBombPlanted[1])
	{
		if (bombPlantedTime[0] / 1000 >= 10 && bombPlantedTime[0] > bombPlantedTime[1])
		{
			MMatchObject* player = MGetMatchServer()->GetObject(bombOwner[0]);
			if (player == nullptr)
				return false;
			MCommand* winCommand = MGetMatchServer()->CreateCommand(MC_MATCH_BOMB_EXPLODE, MUID(0, 0));
			winCommand->AddParameter(new MCmdParamInt((int)player->GetTeam()));

			MGetMatchServer()->RouteToBattle(GetStage()->GetUID(), winCommand);

			SetRoundArg(MMATCH_ROUNDRESULT_REDWON);
			return true;
		}
		else if (bombPlantedTime[1] / 1000 >= 10 && bombPlantedTime[1] > bombPlantedTime[0])
		{
			MMatchObject* player = MGetMatchServer()->GetObject(bombOwner[1]);
			if (player == nullptr)
				return false;
			MCommand* winCommand = MGetMatchServer()->CreateCommand(MC_MATCH_BOMB_EXPLODE, MUID(0, 0));
			winCommand->AddParameter(new MCmdParamInt((int)player->GetTeam()));

			MGetMatchServer()->RouteToBattle(GetStage()->GetUID(), winCommand);

			SetRoundArg(MMATCH_ROUNDRESULT_BLUEWON);
			return true;
		}
		else if (bombPlantedTime[1] >= 10000 && bombPlantedTime[0] >= 10000 && bombPlantedTime[0] == bombPlantedTime[1])
		{
			MMatchObject* player = MGetMatchServer()->GetObject(bombOwner[1]);
			if (player == nullptr)
				return false;
			MCommand* winCommand = MGetMatchServer()->CreateCommand(MC_MATCH_BOMB_EXPLODE, MUID(0, 0));
			winCommand->AddParameter(new MCmdParamInt((int)MMT_ALL));

			MGetMatchServer()->RouteToBattle(GetStage()->GetUID(), winCommand);

			SetRoundArg(MMATCH_ROUNDRESULT_DRAW);
			return true;
		}
		return false;
	}
	else
	{
		if (isBombPlanted[0] && bombPlantedTime[0] / 1000 > 10)
		{
			MCommand* winCommand = MGetMatchServer()->CreateCommand(MC_MATCH_BOMB_EXPLODE, MUID(0, 0));
			winCommand->AddParameter(new MCmdParamInt(MMT_RED));

			MGetMatchServer()->RouteToBattle(GetStage()->GetUID(), winCommand);
			SetRoundArg(MMATCH_ROUNDRESULT_REDWON);
			return true;

		}
		else if (isBombPlanted[1] && bombPlantedTime[1] / 1000 > 10)
		{
			MCommand* winCommand = MGetMatchServer()->CreateCommand(MC_MATCH_BOMB_EXPLODE, MUID(0, 0));
			winCommand->AddParameter(new MCmdParamInt(MMT_BLUE));

			MGetMatchServer()->RouteToBattle(GetStage()->GetUID(), winCommand);

			SetRoundArg(MMATCH_ROUNDRESULT_BLUEWON);
			return true;

		}
		return false;
	}

	return false;
}

bool MMatchRuleDemolition::RoundCount()
{
	if (m_pStage == NULL) return false;

	int nTotalRound = m_pStage->GetStageSetting()->GetRoundMax();
	m_nRoundCount++;

	if (m_nRoundCount < nTotalRound) return true;

	return false;
}
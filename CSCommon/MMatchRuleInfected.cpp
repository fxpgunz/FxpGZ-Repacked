#include "stdafx.h"
#include "MMatchRuleInfected.h"
#include "MMatchTransDataType.h"
#include "MBlobArray.h"
#include "MMatchServer.h"

MMatchRuleInfection::MMatchRuleInfection(MMatchStage* pStage) : MMatchRule(pStage)
{
	nLastStateTime = 0;
	CurrentState = End;
}

void MMatchRuleInfection::OnEnd()
{
	InfectionMap.clear();
}

void MMatchRuleInfection::OnEnterBattle(MUID& uidChar)
{				
	MMatchObject* pObj = m_pStage->GetObj(uidChar);
	if (pObj == NULL) return;
	if (IsAdminGrade(pObj) && pObj->CheckPlayerFlags(MTD_PlayerFlags_AdminHide)) return;
//	if (IsAdminGrade(pObj) == true && pObj->CheckPlayerFlags(MTD_PlayerFlags_AdminHide) || pObj->GetAccountInfo()->m_nUGrade == MMUG_DEV && pObj->CheckPlayerFlags(MTD_PlayerFlags_AdminHide) || pObj->GetAccountInfo()->m_nUGrade == MMUG_TRAIL && pObj->CheckPlayerFlags(MTD_PlayerFlags_AdminHide)) return;
	if(CurrentState == Playing) SendQueueInfo(false, pObj);
	InfectionMap.insert(map<MUID, InfectS*>::value_type(uidChar, new InfectS()));
}

void MMatchRuleInfection::OnLeaveBattle(MUID& uidChar)
{
	MMatchObject* pObj = m_pStage->GetObj(uidChar);
	if (pObj == NULL) return;
	if (IsAdminGrade(pObj) && pObj->CheckPlayerFlags(MTD_PlayerFlags_AdminHide)) return;
//	if (IsAdminGrade(pObj) == true && pObj->CheckPlayerFlags(MTD_PlayerFlags_AdminHide) || pObj->GetAccountInfo()->m_nUGrade == MMUG_DEV && pObj->CheckPlayerFlags(MTD_PlayerFlags_AdminHide) || pObj->GetAccountInfo()->m_nUGrade == MMUG_TRAIL && pObj->CheckPlayerFlags(MTD_PlayerFlags_AdminHide)) return;
	m_pStage->PlayerTeam(uidChar, MMT_RED);
	InfectionMap.erase(uidChar);
}

void MMatchRuleInfection::OnRoundBegin()
{
	MMatchRule::OnRoundBegin();
	for(map<MUID, InfectS*>::iterator Infect = InfectionMap.begin(); Infect!= InfectionMap.end();Infect++)
		Infect->second->Alive = true; // Fix to round 1 ending
	SetState(SelectInfected, 5);
}

void MMatchRuleInfection::OnRoundEnd()
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
	SendQueueInfo();
	MMatchRule::OnRoundEnd();
}

void MMatchRuleInfection::SendQueueInfo(bool ToStage, MMatchObject* Peer)
{
	MTD_InfectedQueueInfo InfecttionInfo;
	InfecttionInfo.QueueSize = InfectionMap.size();
	int c=0;
	for(map<MUID, InfectS*>::iterator Infect = InfectionMap.begin(); Infect!= InfectionMap.end();Infect++, c++)
	{
		MMatchObject* pObj = m_pStage->GetObj(Infect->first);
		if (pObj == NULL || pObj->GetCharInfo() == NULL) continue;
		if(ToStage)
		{
			m_pStage->PlayerTeam(Infect->first, MMT_RED);
			Infect->second->Alive = true;
			Infect->second->Zombie = false;
			InfecttionInfo.IsRoundInfo = true;
		} else InfecttionInfo.IsRoundInfo = false;
		
		if(17 > c) 
		{
			InfecttionInfo.m_WaitQueue[c] = Infect->first;
			for (int i = 0; i < MMCIP_END; i++)
			{			
				if(Infect->second->Zombie)
				{
					switch(i)
					{
					case MMCIP_AVATAR:
						{
							InfecttionInfo.nEquipedItemId[c][i] = pObj->GetCharInfo()->m_nSex == MMS_MALE ? 4003 : 4017;
							InfecttionInfo.nEquipedItemCount[c][i] = 1;
						}
						break;
					case MMCIP_MELEE:
						{
							InfecttionInfo.nEquipedItemId[c][i] = 8;	
							InfecttionInfo.nEquipedItemCount[c][i] = 1;						
						}
						break;
					default:
						{							
							InfecttionInfo.nEquipedItemId[c][i]  = 0;
							InfecttionInfo.nEquipedItemCount[c][i] = 0;
							continue; 
						}
						break;	
					}
				}
				else
				{
					if (pObj->GetCharInfo()->m_EquipedItem.IsEmpty(MMatchCharItemParts(i))) 
					{
						InfecttionInfo.nEquipedItemId[c][i]  = 0;
						InfecttionInfo.nEquipedItemCount[c][i] = 0;
					}
					else 
					{
						MMatchItem* pItem = pObj->GetCharInfo()->m_EquipedItem.GetItem(MMatchCharItemParts(i));
						if( NULL == pItem ) 
						{
							InfecttionInfo.nEquipedItemId[c][i]  = 0;;
							InfecttionInfo.nEquipedItemCount[c][i] = 0;
						}
						else 
						{
							MMatchItemDesc* pItemDesc = pItem->GetDesc();
							if (pItemDesc) 
							{
								InfecttionInfo.nEquipedItemId[c][i]  = pItemDesc->m_nID;
								InfecttionInfo.nEquipedItemCount[c][i] = pItem->GetItemCount();
							}
						}
					}
				}
			}
		}
	}	
	MCommand* pCmd = MGetMatchServer()->CreateCommand(MC_MATCH_RESPONSE_LIST, MUID(0,0));
	pCmd->AddParameter(new MCmdParamBlob(&InfecttionInfo, sizeof(MTD_InfectedQueueInfo)));
	ToStage ? MGetMatchServer()->RouteToBattle(m_pStage->GetUID(), pCmd) : MGetMatchServer()->RouteToListener(Peer, pCmd);
}

bool MMatchRuleInfection::Controller()
{
	if(CurrentState == Playing || CurrentState == End) return true;
	if(timeGetTime() > nLastStateTime)
	{
		switch (CurrentState)
		{
		case SelectInfected:
			{
				int rand = RandomNumber(0, InfectionMap.size()-1);
				int j = 0;
				MUID Infected = MUID(0, 0);
				MMatchObject* pObj;
				for (map<MUID, InfectS*>::iterator i = InfectionMap.begin(); i!= InfectionMap.end();i++, j++)
				if(j == rand) 
				{						
					pObj = m_pStage->GetObj(i->first);
					if(pObj!=NULL)
					{
						i->second->Zombie = true;
						Infected = i->first;
					}
				}	
				if(Infected != MUID(0, 0 ) && pObj && pObj->GetCharInfo())
				{
					RouteMessage(Infected, 0);
					RouteInfected(Infected);
					SetState(Playing, 5);
				} else SetState(SelectInfected, 2);
			}
			break;
		}
	}
	return true;
}

void MMatchRuleInfection::RouteInfected(MUID Player)
{ 
		MMatchObject* pObj = m_pStage->GetObj(Player);
		if(pObj==NULL || pObj->GetCharInfo() == NULL) return;
		m_pStage->PlayerTeam(Player, MMT_BLUE);
		MCommand* pCmd = MGetMatchServer()->CreateCommand(MC_MATCH_RESPONSE_FIRST_INFECTED, MUID(0,0));
		pCmd->AddParameter(new MCmdParamUID(Player));
		pCmd->AddParameter(new MCmdParamInt(pObj->GetCharInfo()->m_nSex == MMS_MALE ? 530002 : 530502));
		pCmd->AddParameter(new MCmdParamInt(8));
		MGetMatchServer()->RouteToBattle(m_pStage->GetUID(), pCmd);
		pObj->SetAlive(true);
}

void MMatchRuleInfection::RouteMessage(MUID Victim, int Type, MUID Attacker)
{
	MTD_InfectedMessage Msg;
	Msg.Victim = Victim;
	Msg.Attacker = Attacker;
	Msg.MessageType = Type;
	MCommand* pCmd = MGetMatchServer()->CreateCommand(MC_MATCH_RESPONSE_INFECTEDMESSAGE, MUID(0,0));
	pCmd->AddParameter(new MCmdParamBlob(&Msg, sizeof(MTD_InfectedMessage)));
	MGetMatchServer()->RouteToBattle(m_pStage->GetUID(), pCmd);
}

bool MMatchRuleInfection::OnRun()
{
	if(Controller()) return MMatchRule::OnRun();
	return false;
}

bool MMatchRuleInfection::OnCheckEnableBattleCondition()
{	
	if(InfectionMap.size() <= 1) return false;
	return true;
}

bool MMatchRuleInfection::GetAliveCount(int* pRedAliveCount, int* pBlueAliveCount)
{
	(*pRedAliveCount) = 0;
	(*pBlueAliveCount) = 0;
	int AliveHumans = 0, AliveZombies = 0;
	for(map<MUID, InfectS*>::iterator i = InfectionMap.begin(); i!= InfectionMap.end();i++)
		if(i->second->Alive) i->second->Zombie ? AliveZombies++ : AliveHumans++;
	(*pRedAliveCount) = AliveHumans;
	(*pBlueAliveCount) = AliveZombies;
	if ((AliveHumans == 0) || (AliveZombies == 0))	return false;
	return true;
}

bool MMatchRuleInfection::OnCheckRoundFinish()
{
	if(CurrentState == Playing)
	{
		int nRedAliveCount = 0;
		int nBlueAliveCount = 0;
		if (GetAliveCount(&nRedAliveCount, &nBlueAliveCount) == false)
		{
			int nRedTeam = 0, nBlueTeam = 0;
			int nStageObjects = 0;
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
		if (nRedAliveCount==0 || nBlueAliveCount==0)
		{			
			SetState(End, 5);
			return true;	
		}
	}	
	return false;
}

void MMatchRuleInfection::OnRoundTimeOut()
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

bool MMatchRuleInfection::RoundCount() 
{
	if (m_pStage == NULL) return false;
	int nTotalRound = m_pStage->GetStageSetting()->GetRoundMax();
	m_nRoundCount++;
	if (m_nRoundCount < nTotalRound) return true;
	return false;
}

void MMatchRuleInfection::CalcTeamBonus(MMatchObject* pAttacker, MMatchObject* pVictim,	int nSrcExp, int* poutAttackerExp, int* poutTeamExp)
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

void MMatchRuleInfection::OnGameKill(const MUID& uidAttacker, const MUID& uidVictim)
{
	map<MUID, InfectS*>::iterator Attacker, Victim;
	Attacker = InfectionMap.find(uidAttacker);
	Victim = InfectionMap.find(uidVictim);
	int Humans = 0, Zombies = 0;
	GetAliveCount(&Humans, &Zombies);
	if(Victim != InfectionMap.end() && Victim->second->Zombie)
	{
		if(Attacker != InfectionMap.end() && Zombies >= 2) 
			RouteMessage(uidVictim, 2, uidAttacker);
		else RouteMessage(uidVictim, 4, uidAttacker);
		Victim->second->Alive = false;
	}
	else if (Attacker != InfectionMap.end() && Attacker->second->Zombie)
	{
		if(Humans >= 2)
		{
			RouteMessage(uidVictim, 1, uidAttacker);
			RouteInfected(Victim->first);
			Victim->second->Zombie = true;
		} 
		else 
		{
			RouteMessage(uidVictim, 3, uidAttacker);
			Victim->second->Alive = false;
		}
	}
}

bool MMatchRuleInfection::IsDead(MUID uid)
{
	map<MUID, InfectS*>::iterator i = InfectionMap.find(uid);
	if(i!=InfectionMap.end()) return !i->second->Alive;
	return true;
}
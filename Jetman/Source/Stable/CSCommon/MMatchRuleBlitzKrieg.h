#pragma once
#include "IMatchRuleNewQuest.h"

class MNewQuestPlayerManager;
class MNewQuestNpcManager;

class MMatchRuleBlitzKrieg : public IMatchRuleNewQuest
{
public:
	MMatchRuleBlitzKrieg(MMatchStage* pStage);
	virtual ~MMatchRuleBlitzKrieg();
	virtual MMATCH_GAMETYPE GetGameType() { return MMATCH_GAMETYPE_BLITZKRIEG;  }
	DWORD m_tresureSpawnTime;
	MUID redBoss;
	MUID blueBoss;
protected:
	virtual void OnBegin();											
	virtual void OnEnd();
	virtual void OnEnterBattle(MUID& uidChar);
	virtual void OnLeaveBattle(MUID& uidChar);
	virtual void OnGameKill(const MUID& uidAttacker, const MUID& uidVictim);
	virtual void OnRoundBegin();
	virtual void OnRoundEnd();
	virtual void OnRoundCountDown() override;
	virtual void OnCommand(MCommand* pCommand);
	virtual void OnRequestPlayerDead(const MUID& uidPlayer);

	virtual void ProcessNpcSpawning();
	virtual MUID SpawnNpc(const char* name, int customSpawnIndex, int spawnIndex, int dropItemID);
	virtual void RouteSpawnNpc(MUID uidNPC, MUID uidController, const char* szNpcDefName, int nCustomSpawnTypeIndex, int nSpawnIndex);
	virtual void SpawnNpcSummon(const char* szActorDef, int num, MShortVector pos, MShortVector dir,const int route);
	virtual void RouteSpawnNpcSummon(MUID uidNpc, MUID uidController, int Num, const char* szNpcDefName, MShortVector pos, MShortVector dir,const int route);
	virtual void RouteNpcDead(MUID uidNPC, MUID uidKiller);
	virtual void RouteSpawnLateJoinNpc(MUID uidLateJoiner, MUID uidNpc, const char* szNpcDefName, int nCustomSpawnTypeIndex, int nSpawnIndex);
	virtual bool OnRun();
	//temporarily
	virtual bool OnCheckRoundFinish();
	virtual bool CheckPlayersAlive() { return true; }
	
	void SpawnTresure();
	void SendBaseHonor();
	void SendPlayerKillHonor(const MUID& uidKiller, const MUID& uidVictim);

	bool OnCheckTresureSpawnTime();
	bool OnRequestUpgrade(const MUID& player, const int upgradeIndex, const int currHonorPoints);
	bool OnRequestSelectClass(const MUID& player, const int classID);
	MNewQuestNpcManager* getNpcMgr() { return IMatchRuleNewQuest::m_pNpcMgr; }
};
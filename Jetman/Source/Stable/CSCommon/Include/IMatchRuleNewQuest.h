#pragma once
#include "MMatchRule.h"
#include "MActorDef.h"
class MNewQuestPlayerManager;
class MNewQuestNpcManager;

class IMatchRuleNewQuest : public MMatchRule
{
protected:
	static MActorDefManager ms_actorDefMgr;

	MNewQuestPlayerManager* m_pPlayerMgr;
	MNewQuestNpcManager* m_pNpcMgr;
public:
	static void InitActorDefMgr();
public:
	IMatchRuleNewQuest(MMatchStage* pStage);
	virtual ~IMatchRuleNewQuest();
	virtual void OnBegin();
	virtual void OnEnd();
	virtual void OnRoundBegin();
	virtual void OnRoundEnd();
	virtual void OnEnterBattle(MUID& uidChar);
	virtual void OnLeaveBattle(MUID& uidChar);
	virtual void OnGameKill(const MUID& uidAttacker, const MUID& uidVictim);

	virtual bool OnCheckRoundFinish();
	virtual bool OnCheckEnableBattleCondition();


	virtual void ProcessNpcSpawning() = 0;
	virtual MUID SpawnNpc(const char* szActorDef, int nCustomSpawnTypeIndex, int nSpawnIndex, int nDropItemId);
	virtual void SpawnNpcSummon(const char* szActorDef, int num, MShortVector nSpawnPos, MShortVector nSpawnDir,const int route);
	virtual void RouteSpawnNpc(MUID uidNPC, MUID uidController, const char* szNpcDefName, int nCustomSpawnTypeIndex, int nSpawnIndex);
	virtual void RouteSpawnNpcSummon(MUID uidNPC, MUID uidController, int num, const char* szNpcDefName, MShortVector sVec, MShortVector dir,const int route);
	virtual void RouteNpcDead(MUID uidNPC, MUID uidKiller);
	virtual void DropItemByNpcDead(const MUID& uidKiller, int nWorldItemId, const MVector& pos);
	//Custom: latejoin
	virtual void RouteSpawnLateJoinNpc(MUID uidLateJoiner,MUID uidNpc, const char* szNpcDefName, int nCustomSpawnTypeIndex, int nSpawnIndex);

	virtual void OnCommand(MCommand* pCommand);
	virtual void OnRequestPlayerDead(const MUID& uidVictim);

};
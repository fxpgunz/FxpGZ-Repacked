#ifndef _MMATCHRULEQUESTCHALLENGE_H
#define _MMATCHRULEQUESTCHALLENGE_H

#include "IMatchRuleNewQuest.h"
#include "MActorDef.h"
#include "MNewQuestScenario.h"

class MNewQuestPlayerManager;
class MNewQuestNpcManager;

class MMatchRuleQuestChallenge : public IMatchRuleNewQuest
{
	static MNewQuestScenarioManager ms_scenarioMgr;
public:
	static void InitScenarioMgr();
	static MNewQuestScenarioManager* GetScenarioMgr();

private:
	MNewQuestScenario* m_pScenario;

	int m_nCurrSector;		// ���� �������� ����

protected:
	virtual bool RoundCount();										///< ���� ī��Ʈ. ��� ���尡 ������ false�� ��ȯ�Ѵ�.
	virtual void OnBegin();											///< ��ü ���� ���۽� ȣ��
	virtual void OnEnd();											///< ��ü ���� ����� ȣ��
	virtual void OnRoundBegin();									///< ���� ������ �� ȣ��
	virtual void OnRoundEnd();										///< ���� ���� �� ȣ��
	virtual bool OnRun();											///< ����ƽ�� ȣ��
	virtual void OnEnterBattle(MUID& uidChar);
	virtual void OnLeaveBattle(MUID& uidChar);
	virtual void OnGameKill(const MUID& uidAttacker, const MUID& uidVictim);
	virtual void OnRequestPlayerDead(const MUID& uidPlayer);
	virtual void RouteSpawnLateJoinNpc(MUID uidLateJoiner, MUID uidNpc, const char* szNpcDefName, int nCustomSpawnTypeIndex, int nSpawnIndex);
protected:
	virtual bool OnCheckRoundFinish();							///< ���尡 �������� üũ
	virtual bool CheckPlayersAlive();
public:
	virtual MMATCH_GAMETYPE GetGameType() { return MMATCH_GAMETYPE_QUEST_CHALLENGE; }

public:
	MMatchRuleQuestChallenge(MMatchStage* pStage);
	virtual ~MMatchRuleQuestChallenge();

	virtual void OnCommand(MCommand* pCommand);
	void RefreshStageGameInfo();

private:
	virtual void RouteSpawnNpc(MUID uidNPC, MUID uidController, const char* szNpcDefName, int nCustomSpawnTypeIndex, int nSpawnIndex);
	virtual void RouteSpawnNpcSummon(MUID uidNPC, MUID uidController, int num, const char* szNpcDefName, MShortVector sVec, MShortVector dir,
		const int route);
	void RouteNpcDead(MUID uidNPC, MUID uidKiller);
	void RouteMoveToNextSector();

	virtual void ProcessNpcSpawning();
	MUID SpawnNpc(const char* szActorDef, int nCustomSpawnTypeIndex, int nSpawnIndex, int nDropItemId);
	void SpawnNpcSummon(const char* szActorDef, int num, MShortVector nSpawnPos, MShortVector nSpawnDir,const int route);
	void DropItemByNpcDead(const MUID& uidKiller, int nWorldItemId, const MVector& pos);
	void RouteXpBpBonus();
	void OnSectorBonus(MMatchObject* pObj, const unsigned int nAddedXP, const unsigned int nAddedBP);
	void MakeStageGameInfo();
	void RouteStageGameInfo(int const& maxPlayers, int const& minLevel);
};

#endif
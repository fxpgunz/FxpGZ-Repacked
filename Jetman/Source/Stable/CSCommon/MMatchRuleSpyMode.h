#pragma once
#include "MMatchRule.h"

class MMatchRuleSpyMode : public MMatchRule
{
private:
	MMatchTeam   winningTeam;
	vector<MUID> Spies;
	int			 m_SpyCount;
public:
	MMatchRuleSpyMode(MMatchStage* pStage);
	virtual ~MMatchRuleSpyMode();
	virtual void OnBegin() override;
	virtual void OnRoundBegin() override;
	virtual void OnRoundEnd() override;
	virtual void OnEnd() override;
	virtual void OnEnterBattle(MUID& uidPlayer) override;
	virtual void OnLeaveBattle(MUID& uidPlayer) override;
	virtual void OnRoundTimeOut() override;
	virtual void OnRoundCountDown() override;
	virtual bool OnCheckEnableBattleCondition() override;
	virtual bool OnCheckRoundFinish() override;
	virtual bool OnRun() override;
	virtual bool RoundCount() override;
	virtual MMATCH_GAMETYPE GetGameType() { return MMATCH_GAMETYPE_SPYMODE; }
	void ChooseSpies();
	void SendWeapons();
	void AddWinningTeamReward(MMatchTeam winningTeam);
	bool IsSpy(const MUID& uidPlayer);
	bool GetAliveCount(int* blueCount,int redCount);
//	void SendInfo();
};
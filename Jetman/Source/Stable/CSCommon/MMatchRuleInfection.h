#pragma once
#include "MMatchRule.h"

class MMatchRuleInfection : public MMatchRule
{
private:
	MUID m_uidFirstZombie;
	bool m_bFirstInfect;
public:
	MMatchRuleInfection(MMatchStage* pStage);
	virtual ~MMatchRuleInfection();

	virtual void OnEnterBattle(MUID& uidPlayer) override;
	virtual void OnRoundBegin() override;
	virtual void OnRoundEnd() override;
	virtual void OnGameKill(const MUID& uidAttacker, const MUID& uidVictim) override;
	virtual void OnRoundCountDown() override;
	virtual void OnRoundTimeOut() override;
	virtual bool OnCheckRoundFinish() override;
	virtual bool OnCheckEnableBattleCondition() override;
	virtual bool RoundCount() override;
	virtual bool OnRun() override;
	virtual MMATCH_GAMETYPE GetGameType() override { return MMATCH_GAMETYPE_INFECTION; }

	void OnInfectPlayer(const MUID& uidVictim = MUID(0,0), bool bFirstInfect = false);
	bool GetAliveCount(int* redteam, int* blueteam);

};
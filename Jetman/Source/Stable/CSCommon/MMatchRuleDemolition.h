#pragma once
#include "MMatchRule.h"

class MMatchRuleDemolition : public MMatchRule
{
private:
	DWORD bombPlantedTime[2];
	bool isBombPlanted[2];
	MUID bombOwner[2];
public:
	MMatchRuleDemolition(MMatchStage* stage);
	virtual ~MMatchRuleDemolition();
protected:
	virtual void OnBegin() override;
	virtual void OnEnd() override;
	virtual void OnRoundBegin() override;
	virtual void OnRoundEnd() override;
	virtual void OnEnterBattle(MUID& player) override;
	virtual void OnLeaveBattle(MUID& player) override;
	virtual void OnRoundTimeOut() override;
	virtual void OnCommand(MCommand* command) override;
	virtual void OnGameKill(const MUID& uidAttacker, const MUID& uidVictim) override;

	virtual bool OnCheckRoundFinish() override;
	virtual bool OnRun() override;
	virtual bool OnCheckEnableBattleCondition() override;
	virtual bool RoundCount() override;

	virtual MMATCH_GAMETYPE GetGameType() override { return MMATCH_GAMETYPE_DEMOLITION; }

public:
	void OnPlantBomb(const MUID& attacker);
	virtual void Tick(unsigned long tickTime) override;
};
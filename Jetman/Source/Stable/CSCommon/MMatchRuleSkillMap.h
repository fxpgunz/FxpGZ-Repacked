#pragma once
#include "MMatchRule.h"

class MMatchRuleSkillMap : public MMatchRule
{
public:
	MMatchRuleSkillMap(MMatchStage* pStage);
	virtual ~MMatchRuleSkillMap();
protected:
	virtual void OnBegin();
	virtual void OnEnd();
	virtual void OnRoundBegin();
	virtual void OnRoundEnd();
	virtual void OnEnterBattle(MUID& player) override;
	virtual void OnObtainWorldItem(MMatchObject* pObj, int nItemID, int* pnExtraValues) override;
	virtual bool OnRun() override;
	virtual bool OnCheckEnableBattleCondition() override;
	virtual bool OnCheckRoundFinish() override;
	virtual bool RoundCount() override;

public:
	virtual MMATCH_GAMETYPE GetGameType() { return MMATCH_GAMETYPE_SKILLMAP; }

private:
	bool m_roundFinish;

};
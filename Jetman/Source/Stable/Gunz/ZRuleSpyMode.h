#pragma once
#include "ZRule.h"

class ZRuleSpyMode : public ZRule
{
private:
	vector<MUID> Spies;
	vector<MTD_SpyItems> SpyItems;
	bool		 m_bRevealTime;
	int			 m_HpApBonus;
public:
	ZRuleSpyMode(ZMatch* pMatch);
	virtual ~ZRuleSpyMode();

	virtual bool OnCommand(MCommand* pCmd) override;
	virtual void AfterCommandProcessed(MCommand* pCmd) override;
	virtual void OnSetRoundState(MMATCH_ROUNDSTATE roundState) override;
	virtual void OnUpdate(float fElapsed) override;

	bool IsSpy(const MUID& uidPlayer);
	bool IsSpyOpenTime();
	bool IsIntroduceTeam();

	void DrawRemainingSpies(int SpyCount);
	void Draw(MDrawContext* pDC);
	void SetTeams();
	void ApplyBonus(ZCharacter* playerObj,unsigned char scoreValue);

	auto GetSpies() { return Spies; }
	auto GetSpyItems() {
		return SpyItems;
	}
	//for replays only
	void SetSpyItems(vector<MTD_SpyItems> items) { SpyItems = items; }
	void SetSpies(vector<MUID> spies) { Spies = spies; }
};
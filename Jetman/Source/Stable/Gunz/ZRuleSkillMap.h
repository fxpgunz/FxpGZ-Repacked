#pragma once
#include "ZRule.h"

class ZRuleSkillMap : public ZRule
{
private:
	MUID  m_chestGrabber;
	bool  m_setDrawStartNotice;
	DWORD m_roundStartTime;
	DWORD currRoundTime;
	DWORD myBestTime;
public:
	ZRuleSkillMap(ZMatch* pMatch);
	virtual ~ZRuleSkillMap();

	virtual bool OnCommand(MCommand* command) override;

	void OnDraw(MDrawContext* pDC);
	void OnUpdateUI(const DWORD& newBestTime);

	virtual void OnUpdate(float fElapsed) override;
	virtual void OnSetRoundState(MMATCH_ROUNDSTATE roundState) override;
	virtual void AfterCommandProcessed(MCommand* command) override;

	MUID GetChestGrabber() { return m_chestGrabber; }
};
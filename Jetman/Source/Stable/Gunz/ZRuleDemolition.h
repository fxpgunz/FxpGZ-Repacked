#pragma once

#include "ZRule.h"

class ZRuleDemolition : public ZRule
{
private:
	MUID bombCarrier[2];
	bool m_bombPlanted[2];
	RVisualMesh* m_bombMesh[2];
public:
	ZRuleDemolition(ZMatch* pMatch);
	virtual ~ZRuleDemolition();

	virtual bool OnCommand(MCommand* command) override;

	virtual void OnUpdate(float fElapsed) override;
	virtual void OnSetRoundState(MMATCH_ROUNDSTATE roundState) override;
	virtual void AfterCommandProcessed(MCommand* command) override;

	void OnDraw(MDrawContext* pDC);
	void AddBombExplodeEffect(MMatchTeam winningTeam);
};
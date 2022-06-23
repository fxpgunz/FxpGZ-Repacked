#pragma once
#include "ZRule.h"
#include "MBitmap.h"
#include "ZCharacterView.h"

class ZRuleInfection : public ZRule
{
private:
	MBitmapR2* ZombieOverlay;
public:
	MUID	   uidFirstZombie;

	ZRuleInfection(ZMatch* pMatch);
	virtual ~ZRuleInfection();
	virtual bool OnCommand(MCommand* pCmd) override;
	virtual void OnSetRoundState(MMATCH_ROUNDSTATE roundstate) override;
	virtual void AfterCommandProcessed(MCommand* pCmd) override;

	void InfectPlayer(const MUID& uidPlayer, const int hp, const int ap, bool bFirstInfect);
	void OnDraw(MDrawContext* pDC);
	void DrawInfectionPanel(MDrawContext* pDC);
	void DrawRemainingPlayers(const MUID& uidPlayer);
	MUID GetFirstInfected() { return uidFirstZombie; }


	inline ZCharacterView* GetCharViewWidget() {
		return (ZCharacterView*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("Infected_CharView");
	}
};
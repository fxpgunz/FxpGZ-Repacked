#ifndef _ZRULE_INFECTED_H
#define _ZRULE_INFECTED_H
#include "ZRule.h"
class ZRuleInfected : public ZRule
{
	enum DrawText
	{
		FirstInfected,
		BeenInfected,
		KilledInfected,
		ZombiesWin,
		HumansWin,
		None
	};
private:
	unsigned long int Timer;
	DrawText CurrentText;
	MUID Victim, Attacker;
protected:
	void SetMessage(int Msg) { CurrentText = (DrawText)Msg; Timer = timeGetTime() + 8000;}
	virtual ~ZRuleInfected();
	virtual bool OnCommand(MCommand* pCommand);
	void ChangeClothes(ZCharacter* cha, int ItemID);

public:
	ZRuleInfected(ZMatch* pMatch);
	virtual void OnDrawInfected(MDrawContext* pDC);
};
#endif
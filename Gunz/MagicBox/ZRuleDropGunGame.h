#ifndef _ZRULE_DROPGUNGAME_H
#define _ZRULE_DROPGUNGAME_H

#include "../ZRule.h"

class ZDropGunGameWorldItem
{
protected:
	int m_nID;
	float m_fassx;
	float m_fassy;
	float m_fassz;
public:
	void SetID(int i) { m_nID = i; }
	void SetAssX(int f) { m_fassx = f; }
	void SetAssY(int f) { m_fassy = f; }
	void SetAssZ(int f) { m_fassz = f; }

	int GetID() { return m_nID; }
	float GetAssX() { return m_fassx; }
	float GetAssY() { return m_fassy; }
	float GetAssZ() { return m_fassz; }
};

class ZRuleDropGunGame : public ZRule, public map<int, ZDropGunGameWorldItem*>
{
protected:
	int m_nTime[3];
	bool m_show[3];
	string m_szText[3];
	int m_nID = 0;
public:
	ZRuleDropGunGame(ZMatch* pMatch);
	virtual ~ZRuleDropGunGame();
	virtual bool OnCommand(MCommand* pCommand);
	void OnDrawDropGunGame(MDrawContext* pDC);
private:
	void AddWorldItem(int nID, float possx, float possy, float possz);
	bool AddDropGunGameWorldItem(ZDropGunGameWorldItem* pGunGameWorldItem);
public:
	ZDropGunGameWorldItem* GetDropGunGameWorldItemByID(int nID);
};

#endif
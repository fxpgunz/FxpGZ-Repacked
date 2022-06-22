#ifndef _MMATCHRULE_DROPGUNGAME_H
#define _MMATCHRULE_DROPGUNGAME_H

#include "MMatchRule.h"
#include "MDropGunGameManager.h"

class MDropGunGameXMLManager;
class MDropGunGameStageManager;

////////////////////////////////////////////////
// MMatchRuleDropGunGame

class MMatchRuleDropGunGame : public MMatchRule
{
private:
	int GunGameListID;
protected:
	virtual void OnBegin();
	virtual void OnEnd();
	virtual void OnRoundBegin();
	virtual void OnRoundTimeOut();
	virtual bool OnCheckRoundFinish();
	virtual bool RoundCount();
	bool CheckKillCount(MMatchObject* pOutObject);
	virtual void OnGameKill(const MUID& uidAttacker, const MUID& uidVictim);
	virtual void OnEnterBattle(MUID& uidChar);
	virtual void OnLeaveBattle(MUID& uidChar);
public:
	MMatchRuleDropGunGame(MMatchStage* pStage);
	virtual ~MMatchRuleDropGunGame() { }
	virtual MMATCH_GAMETYPE GetGameType() { return MMATCH_GAMETYPE_DROPMAGIC; }
	void OnObtainWorldItem(MMatchObject* pObj);
	static bool InitDropGunGameListMgr();
public:
	static MDropGunGameXMLManager m_DropGunGameXMLManager;
	static MDropGunGameStageManager m_DropGunGameStageManager;
	void SetGunGameListID(int i) { GunGameListID = i; }
	int GetGunGameListID() { return GunGameListID; }
};

#endif
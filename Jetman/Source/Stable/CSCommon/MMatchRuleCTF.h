#pragma once
#include "MMatchRule.h"



class MMatchRuleTeamCTF : public MMatchRule {

private:
	MUID		m_uidRedFlagHolder;
	MUID		m_uidBlueFlagHolder;
	bool		m_bIsRedFlagTaken;
	bool		m_bIsBlueFlagTaken;
	MVector		m_BlueFlagPosition;
	MVector		m_RedFlagPosition;
private:
	inline void SetBlueFlagObtained(int state) { m_bIsBlueFlagTaken = state; }
	inline void SetRedFlagObtained(int state) { m_bIsRedFlagTaken = state; }
	inline void SetBlueCarrier(MUID state) { m_uidBlueFlagHolder = state; }
	inline void SetRedCarrier(MUID state) { m_uidRedFlagHolder = state; }
protected:
	//bool GetAliveCount(int* pRedAliveCount, int* pBlueAliveCount);
	void GetTeamScore(int* pRedTeamScore, int* pBLueTeamScore);
	virtual void OnBegin();
	virtual void OnEnd();
	virtual bool OnRun();
	virtual void OnRoundBegin();
	virtual void OnRoundEnd();
	virtual void OnLeaveBattle(MUID& uidChar);
	virtual void OnEnterBattle(MUID& uidChar);
	virtual bool OnCheckRoundFinish();
	virtual void OnRoundTimeOut();
	virtual void RouteAssignFlag(MUID& uidBearer, int nTeam);
	virtual void RouteAssignFlagToJoiner(MUID& uidFlagBearer, MUID& uidSendTo, int nTeam);
	virtual void RouteAssignCap(int nTeam);
	virtual bool RoundCount();
	virtual void OnGameKill(const MUID& uidAttacker, const MUID& uidVictim);
	virtual bool OnCheckEnableBattleCondition() override;
	int nRedCount;
	int nBlueCount;
public:
	MMatchRuleTeamCTF(MMatchStage* pStage);
	virtual ~MMatchRuleTeamCTF() {}
	virtual void CalcTeamBonus(MMatchObject* pAttacker, MMatchObject* pVictim,
		int nSrcExp, int* poutAttackerExp, int* poutTeamExp);
	virtual MMATCH_GAMETYPE GetGameType() { return MMATCH_GAMETYPE_CTF; }
	virtual void OnObtainWorldItem(MMatchObject* pObj, int nItemID, int* pnExtraValues);
	inline MUID GetBlueCarrier() { return m_uidBlueFlagHolder; }
	inline MUID GetRedCarrier() { return m_uidRedFlagHolder; }

	inline MVector GetBlueFlagPosition() { return m_BlueFlagPosition; }
	inline MVector GetRedFlagPosition() { return m_RedFlagPosition; }
	inline void SetBlueFlagPosition(MVector &pos) { m_BlueFlagPosition = pos; }
	inline void SetRedFlagPosition(MVector &pos) { m_RedFlagPosition = pos; }
	inline bool IsBlueFlagTaken() { return m_bIsBlueFlagTaken; }
	inline bool IsRedFlagTaken() { return m_bIsRedFlagTaken; }
	virtual void RouteAssignState(MUID uidSendTo);
	virtual void SendAssignState();
};
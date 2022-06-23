#pragma once

#include "IRuleNewQuest.h"

///f
enum SPECIAL_ANNOUNCEMENT
{
	SA_EVNTMSG0 = 0,
	SA_EVNTMSG1,
	SA_EVNTMSG2,
	SA_EVNTMSG3,
	SA_EVNTMSG4,
	SA_EVNTMSG5,
	SA_EVNTMSG6,
	SA_EVNTMSG7,
	SA_EVNTMSG8,
	SA_EVNTMSG9,
	SA_EVNTMSG10,
	SA_EVNTMSG11,
	SA_EVNTMSG12,
	SA_EVNTMSG13,
	SA_EVNTMSG14,
	SA_EVNTMSG15,
};

class ZRuleBlitzKrieg : public IRuleNewQuest
{
private:
	int m_redBarricades;
	int m_blueBarricades;
	int m_tresureChests;
	MUID m_bossRedUID;
	MUID m_bossBlueUID;
	bool m_drawHPTopInfo;
	DWORD m_updateTips;
	DWORD m_updateHonorTick;
	DWORD m_roundStartTime;;
public:
	ZRuleBlitzKrieg(ZMatch* pMatch);
	virtual ~ZRuleBlitzKrieg();

	virtual bool Init();
	virtual bool OnCommand(MCommand* pCommand);

	virtual void OnUpdate(float fDelta);
	virtual void OnSetRoundState(MMATCH_ROUNDSTATE roundState);

	virtual ZActorWithFSM* SpawnActor(const char* szActorDefName, MUID& uid, int nCustomSpawnTypeIndex, int nSpawnIndex, bool bMyControl);
	virtual ZActorWithFSM* SpawnActorSummon(const char* szActorName, MUID& uid, int nNum, bool bMyControl, MShortVector vec, MShortVector vec2,int route);
	virtual ZActorWithFSM* SpawnLateJoinNpc(const char* szActorDefName, MUID& uid, int ncustomSpawnTypeIndex, int nspawnIndex, bool bMyControl = false);
	virtual void CheckSpecialEvent(ZActorWithFSM* pDeadActor, MMatchTeam nTeamID);

	DWORD GetElapsedTime() { return IRuleNewQuest::GetElapsedTime(); }

	void OnDraw(MDrawContext* pDC);
	void DrawHPTop(bool draw);
	void DrawMap(MDrawContext* pDC, bool draw);
	void DrawHelpScreen(MDrawContext* pDC, bool draw);
	void DrawClassSelect(MDrawContext* pDC, bool draw);
	void DrawUpgradeList(MDrawContext* pDC, bool draw);
	void ApplyActorName(ZActorWithFSM* pActor);
	void AddHonorReward(ZActorWithFSM* pActor,MUID uidKiller);
	void AddPlayerKillReward(const MUID& uidKiller, const MUID& uidVictim);
	void DrawAnnouncement(SPECIAL_ANNOUNCEMENT announcementType);
	void UpdateClassSelectDesc(const MMatchObjectClass& classID);
	void UpdateUpgradeUI(int& const index, int& const upgradeLevel);

	const int& GetBlueBarricadeCount() { return m_blueBarricades; }
	const int& GetRedBarricateCount() { return m_redBarricades; }
	const int& GetRedBossHP()
	{
		ZActorWithFSM* pRedBoss = dynamic_cast<ZActorWithFSM*>(ZGetObjectManager()->GetNPCObject(m_bossRedUID));
		if (pRedBoss == nullptr)
			return 0;

		return pRedBoss->GetActualHP();
	}
	const int& GetRedBossMaxHP()
	{
		ZActorWithFSM* pRedBoss = dynamic_cast<ZActorWithFSM*>(ZGetObjectManager()->GetNPCObject(m_bossRedUID));
		if (pRedBoss == nullptr)
			return 0;

		return pRedBoss->GetActualMaxHP();
	}

	const int& GetBlueBossHP()
	{
		ZActorWithFSM* pBlueBoss = dynamic_cast<ZActorWithFSM*>(ZGetObjectManager()->GetNPCObject(m_bossBlueUID));
		if (pBlueBoss == nullptr)
			return 0;

		return pBlueBoss->GetActualHP();
	}

	const int& GetBlueBossMaxHP()
	{
		ZActorWithFSM* pBlueBoss = dynamic_cast<ZActorWithFSM*>(ZGetObjectManager()->GetNPCObject(m_bossBlueUID));
		if (pBlueBoss == nullptr)
			return 0;

		return pBlueBoss->GetActualMaxHP();
	}

	MBitmap* GetClassBitmap(const int& classID, const MMatchTeam& teamID);
};
#pragma once

#include "ZRule.h"
#include "MMatchUtil.h"
#include "ZActorWithFSM.h"
#include "MActorDef.h"

class ZFSMManager;
class MActorDefManager;
class ZActorActionManager;

class IRuleNewQuest : public ZRule
{
protected:
	ZFSMManager* m_pFsmManager;
	MActorDefManager* m_pActorDefMgr;
	ZActorActionManager* m_pActorActionMgr;
	int m_nNPC;
	DWORD m_dwElapsedTime;
	DWORD m_currRoundElapsedTime;
public:
	IRuleNewQuest(ZMatch* pMatch);
	virtual ~IRuleNewQuest();

	virtual void OnUpdate(float fDelta);
	virtual void CheckSpecialEvent(ZActorWithFSM* pDeadActor, MMatchTeam nTeamID) = 0;
	virtual void SetNewNpcController(MUID& uidController, MUID& uidNpc);

	virtual ZActorWithFSM* SpawnActor(const char* szActorDefName, MUID& uid, int nCustomSpawnTypeIndex, int nSpawnIndex, bool bMyControl);
	virtual ZActorWithFSM* SpawnActorSummon(const char* szActorName, MUID& uid, int nNum, bool bMyControl, MShortVector vec, MShortVector vec2,int route);
	virtual ZActorWithFSM* SpawnLateJoinNpc(const char* szActorDefName, MUID& uid, int ncustomSpawnTypeIndex, int nSpawnIndex, bool bMyControl = false);

	virtual bool OnCommand(MCommand* pCmd);
	virtual bool Init();

	int	  GetNPC() { return static_cast<int>(ZGetGame()->m_ObjectManager.GetNPCObjectMap()->size()); }
	DWORD GetElapsedTime() {return m_dwElapsedTime;}
	void SetElapsedTime(UINT elaspedTime) { m_dwElapsedTime = elaspedTime; }
	const DWORD& GetCurrRoundTime() { return m_currRoundElapsedTime; }

	ZActorActionManager* GetActorActionMgr() { return m_pActorActionMgr; }
	MActorDefManager* const GetActorDefMgr() { return m_pActorDefMgr; }
	ZFSMManager* const GetFSMMgr() { return m_pFsmManager; }

};
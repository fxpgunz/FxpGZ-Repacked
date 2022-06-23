#pragma once
#include "MRTTI.h"
#include "ZActorBase.h"
#include "MUID.h"
#include "RTypes.h"
#include "RPathFinder.h"
#include "RVisualMeshMgr.h"
#include "../CSCommon/MCompanionDef.h"

enum SHOTTYPE
{
	ST_NONE,
	ST_MELEE,
	ST_RANGE,
	///todo: determine more
};

class ZCompanion : public ZActorBase
{
	///TODO: fill this
private:
	MMatchTeam	m_teamID;
	MUID		targetUID;
	SHOTTYPE	shotType;
	DWORD		m_lastTargetSearch;
	MCompanionDef* m_companionDef;
public:
	ZCompanion(IGame* pGame);
	virtual ~ZCompanion();
	ZCharacter* companionOwner;
	RMesh*		m_pMesh;
protected:
	ZModule_HPAP			*m_pModule_HPAP;
	ZModule_Resistance		*m_pModule_Resistance;
	ZModule_FireDamage		*m_pModule_FireDamage;
	ZModule_ColdDamage		*m_pModule_ColdDamage;
	ZModule_PoisonDamage	*m_pModule_PoisonDamage;
	ZModule_LightningDamage	*m_pModule_LightningDamage;


	virtual void OnDraw();
	virtual void OnUpdate(float fDelta);
	virtual void OnDie();

	virtual void SetMyControl(bool bMyControl) {};
	virtual bool IsMyControl() { return true; }
	virtual bool IsNeverPushed() {
		return true;
	}
	virtual bool IsCollideable();
	virtual float GetCollideRadius() {
		return 35.f;
	}

	virtual float ModifyAttackDamage(float fDamage) {
		return fDamage;
	}

	virtual int GetActualHP() {
		return 100;
	}
	virtual int GetActualAP() {
		return 100;
	}
	virtual int GetActualMaxHP() {
		return 100;
	}
	virtual int GetActualMaxAP() {
		return 100;
	}

	virtual void InputBasicInfo(ZBasicInfo* pni, BYTE anistate) {};
	virtual void InputBossHpAp(float fHp, float fAp) {};

	virtual void OnPeerDie(MUID& uidKiller) {};

	virtual bool IsDie();

	virtual ZOBJECTHITTEST HitTest(const rvector& origin, const rvector& to, float fTime, rvector *pOutPos);


	void UpdatePosition(float fDelta);
	void UpdateHeight(float fDelta);
	void ProcessShot(float fDelta);
	void OnDamaged(ZObject* pAttacker, rvector srcPos, ZDAMAGETYPE damageType, MMatchWeaponType weaponType, float fDamage, float fPiercingRatio, int nMeleeType);
	void FindTarget();
	void OnAttack(SHOTTYPE shotType);

	bool ProcessMotion(float fDelta);
	rvector m_RealPositionBefore;

public:
	void SetOwner(ZCharacter* owner) { 
		companionOwner = owner;
	}
	ZCharacter* GetOwner() { return companionOwner; }

	virtual void GetDebugInfoString(char* szBuf1, char* szBuf2, int index) {};

	virtual MMatchTeam GetTeamID() { return m_teamID; }			// NPC는 무조건 BLUE팀
	void SetTeamID(MMatchTeam teamID) { m_teamID = teamID; }

	MUID getTargetUID() { return targetUID; }
	void setTargetPlayer(MUID target) { targetUID = target; }

	MUID& GetUID() { return m_UID; }
	void SetUID(MUID& uid) { m_UID = uid; }

	void InitProperties(MCompanionDef* companionDef,MUID& companionUID);

	bool IsAir()
	{
		// 공중에 떠있거나 떠오르는 중인지 체크
		bool bAir = (GetDistToFloor() > 10.f || GetVelocity().z > 0.1f);
		return bAir;
	}
};
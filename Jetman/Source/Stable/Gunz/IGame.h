#pragma once

#include "ZCharacterManager.h"
class ZNavigationMesh;

// 로직 객체가 ZGame에 직접 의존하는 것을 제거하기 위한 인터페이스
// game 에서 pick 되어져 나온 결과.. pCharacter(캐릭터) 혹은 pNode(맵)둘중에 하나의 결과만 나온다.
struct ZPICKINFO {

	// 캐릭터가 결과인 경우
	//	ZCharacter *pCharacter;
	ZObject*	pObject;
	RPickInfo	info;

	// 맵이 결과인경우.
	bool bBspPicked;
	int nBspPicked_DebugRegister;
	RBSPPICKINFO bpi;
};


class IGame
{
public:
	virtual float GetTime() = 0;
	virtual ZMyCharacter* GetMyCharacter() = 0;
	virtual MUID GetMyUid() = 0;

	virtual ZCharacterManagerBase* GetCharacterMgr() = 0;

	virtual bool CanAttack(ZObject *pAttacker, ZObject *pTarget) = 0;
	virtual bool CanAttack_DebugRegister(ZObject *pAttacker, ZObject *pTarget) = 0;

	virtual bool Pick(ZObject *pOwnerObject,rvector &origin,rvector &dir,ZPICKINFO *pickinfo,DWORD dwPassFlag=RM_FLAG_ADDITIVE | RM_FLAG_HIDE,bool bMyChar=false) = 0;

	virtual bool PickWorld( const rvector &pos, const rvector &dir,RBSPPICKINFO *pOut,DWORD dwPassFlag=RM_FLAG_ADDITIVE | RM_FLAG_USEOPACITY | RM_FLAG_HIDE) = 0;
	virtual bool CheckWall(rvector &origin, rvector &targetpos, float fRadius, float fHeight=0.f, RCOLLISIONMETHOD method=RCW_CYLINDER, int nDepth=0, rplane *pimpactplane=NULL) = 0;
	virtual rvector GetFloor(rvector pos, rplane *pimpactplane=NULL, MUID uID=MUID(0,0) ) = 0;
	virtual void AdjustMoveDiff(ZObject* pObject, rvector& diff) = 0;

	virtual ZNavigationMesh GetNavigationMesh() = 0;

	virtual bool InRanged( ZObject* pAttacker, ZObject* pVictim) = 0;
	virtual bool InRanged( ZObject* pAttacker, ZObject* pVictim, int &nDebugRegister/*디버그 레지스터 해킹 방지를 위한 변수*/) = 0;


	virtual bool IsWallBlocked(ZObject* pObj1,ZObject* pObj2, bool bCoherentToPeer=false) = 0;
	virtual bool IsWallBlocked(ZObject* pObj1,ZObject* pObj2, int & nDebugRegister/*단순 비교용*/, bool bCoherentToPeer=false) = 0;

	virtual bool IsExceptedFromNpcTargetting(const ZCharacter* pChar) = 0;
	virtual bool IsEnabledToWarpNpcWhenStucked() { return false; }

	virtual int GetCharacterBasicInfoTick() { return 100; }
};
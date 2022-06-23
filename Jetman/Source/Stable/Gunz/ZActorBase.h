#pragma once
#include "MRTTI.h"
#include "ZCharacterObject.h"
#include "MUID.h"
#include "MObjectTypes.h"

/// �÷��̾ �ƴ� ������Ʈ���� �θ� Ŭ����

class ZActorBase : public ZCharacterObject
{
	MDeclareRTTI;

protected:
	IGame* m_pGame;

public:
	ZActorBase(IGame* pGame);
	virtual ~ZActorBase() {}

	virtual void SetMyControl(bool bMyControl) = 0;
	virtual bool IsMyControl() = 0;
	virtual bool IsNeverPushed() = 0;
	virtual float GetCollideRadius() = 0;
	virtual float ModifyAttackDamage(float fDamage) = 0;

	virtual int GetActualHP() = 0;
	virtual int GetActualAP() = 0;
	virtual int GetActualMaxHP() = 0;
	virtual int GetActualMaxAP() = 0;

	virtual void InputBasicInfo(ZBasicInfo* pni, BYTE anistate) = 0;
	virtual void InputBossHpAp(float fHp, float fAp) = 0;

	virtual void OnPeerDie(MUID& uidKiller) = 0;
	virtual void OnDie() = 0;

public:
	// Ư�������� hit test �� ��������� �Ѵ�, pOutPos �� ������ hit�� ��ġ�� ��������� �Ѵ�
	ZOBJECTHITTEST HitTest_Actor( bool bUseMeshPick, const rvector& origin, const rvector& to, float fTime, rvector *pOutPos=NULL );

protected:
	virtual void AllocObjectVMesh();

public:
	// ����׿�
	char m_szOwner[64];
	float m_fLastBasicInfo;
	void SetOwner(const char* szOwner) { strcpy(m_szOwner,szOwner); }

	virtual void GetDebugInfoString(char* szBuf1, char* szBuf2, int index) = 0;
};
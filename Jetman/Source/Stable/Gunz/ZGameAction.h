#ifndef _ZGAMEACTION_H
#define _ZGAMEACTION_H

#include "MCommand.h"

// �� enum �� ���� �������ݰ� ������ �����Ƿ� ������ �Ұ�. �߰��� ����
enum ZREACTIONID{	// �� ������ �ٸ�������� �˸���
	ZR_CHARGING = 0,		// �� �����⸦ ����
	ZR_CHARGED,				// �Ⱑ �𿳴�
	ZR_BE_UPPERCUT,			// Was up
	ZR_BE_DASH_UPPERCUT,	// Daggers dash technique when I hit
	ZR_DISCHARGED,			// �� ���ΰ� Ǯ�ȴ�

	ZR_END
};

class ZCharacter;
class ZMyCharacter;

/// ZGame���� ĳ���͵鰣�� �׼ǿ� ���� �͵�(ZGame�� OnPeer�������)�� �̰����� �����ô�.
class ZGameAction
{
	bool OnReaction(MCommand* pCommand);
	bool OnPeerSkill(MCommand* pCommand);
	bool OnEnchantDamage(MCommand* pCommand);

public:
	bool ApplyFireEnchantDamage(ZObject* pTarget, ZObject* pOwner, int nDamage, int nDuration);
	bool ApplyColdEnchantDamage(ZObject* pTarget, int nLimitSpeed, int nDuration);
	bool ApplyPoisonEnchantDamage(ZObject* pTarget, ZObject* pOwner, int nDamage, int nDuration);
	bool ApplyLightningEnchantDamage(ZObject* pTarget, ZObject* pOwner, int nDamage, int nDuration);

private:
	void OnPeerSkill_Uppercut(ZCharacter *pOwnerCharacter);
	void OnPeerSkill_LastShot(float fShotTime,ZCharacter *pOwnerCharacter);	// ������ ���÷���
	void OnPeerSkill_Dash(ZCharacter *pOwnerCharacter);

public:
	bool OnCommand(MCommand* pCommand);
};


#define CHARGED_TIME			15.f		// ������ ���ӽð� (����:��)
#define COUNTER_CHARGED_TIME	1.f			// �ݰݱ� ������ ���ӽð�
#define COUNTER_CHARGED_TIME_NM 0.f


#endif
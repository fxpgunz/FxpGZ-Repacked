#pragma once
#include "ZFSMGlobal.h"

class ZFSMState;

// ���ǰ�ü�� �Ǵ��� ���� ������ ��û�� �� �ִ� �������̽�
class IFSMConditionSource
{
public:
	virtual float Cond_GetHp() = 0;
	virtual float Cond_GetGroggy() = 0;
	virtual const rvector& Cond_GetVelocity() = 0;
	virtual bool Cond_IsEndAction() = 0;
	virtual bool Cond_IsLanding() = 0;

	virtual bool Cond_HasTarget() = 0;
	virtual void Cond_GetPos(rvector& out) = 0;
	virtual void Cond_GetTargetPos(rvector& out) = 0;

	virtual void Cond_GetDirection(rvector& out) = 0;

	virtual int Cond_GetDice() = 0;

	virtual bool Cond_CheckStateCooltime(ZFSMState* pState, DWORD dwCooltime) = 0;
	virtual DWORD Cond_GetStateEnteredTime() = 0;

	virtual bool IsActiveLightningDamageModule() = 0;
	virtual bool Cond_CanSeeTarget() = 0;
	virtual bool Cond_CantSeeTarget() = 0;
	virtual bool Cond_Pick(float fAngleDegree, float fDist) = 0;
	//the missing conditions
	virtual bool Cond_FailedBuildWaypoints() = 0;
	virtual bool Cond_AngleTargetHeight(int low, int high) = 0;
	virtual bool Cond_TargetHeightHigher(int diff) = 0;
	virtual bool Cond_SummonLess(int Count) = 0;
};


// NPC ���±�迡�� �������̸� �Ǵ��ϱ� ���� ���ǰ�ü

enum E_FSMCOND_ID
{
	FSMCOND_INVALID = -1,

	FSMCOND_DEFAULT,			// �׻� TRUE�� ��ȯ�ϴ� ���ǰ�ü (�������ǵ��� ������ ����Ʈ ���̸� ���� ���)
	FSMCOND_DICE,

	FSMCOND_HP_EQUAL,
	FSMCOND_HP_LESS,
	FSMCOND_HP_GREATER,
	FSMCOND_GROGGY_GREATER,
	FSMCOND_VELOCITY_Z_LESS,
	FSMCOND_VELOCITY_MAGNITUDE_LESS,
	FSMCOND_END_ACTION,
	FSMCOND_IS_LANDING,			// �����ϴ� �ٷ� �� �����ΰ�

	FSMCOND_HAS_TARGET,
	FSMCOND_HAS_NO_TARGET,
	FSMCOND_DIST_TARGET_IN,
	FSMCOND_LOOK_AT_TARGET,

	FSMCOND_IS_EMPTY_SPACE,		// Ư�� ������ ��������� ��ŷ�غ���

	FSMCOND_TIME_ELAPSED_SINCE_ENTERED,
	FSMCOND_END_LIGHTNINGDAMAGE,
	FSMCOND_CAN_SEE_TARGET,
	FSMCOND_FAILED_BUILD_WAYPOINTS,
	FSMCOND_ANGLE_TARGET_HEIGHT,
	FSMCOND_TARGET_HEIGHT_HIGHER,
	FSMCOND_CANT_SEE_TARGET,
	FSMCOND_SUMMONLESS,
	// �� ID�� �߰��� ������ ParseConditionStatement �׽�Ʈ�� �� �׸��� �߰��սô�
};

class ZFSMCondition
{
protected:
	E_FSMCOND_ID m_condId;

	int m_nArg[MAX_FSMMETHOD_ARG];

public:
	void Init(E_FSMCOND_ID id, int arg1=FSMMETHOD_ARG_NONE, int arg2=FSMMETHOD_ARG_NONE, int arg3=FSMMETHOD_ARG_NONE, int arg4=FSMMETHOD_ARG_NONE)
	{
		m_condId = id;
		m_nArg[0] = arg1;
		m_nArg[1] = arg2;
		m_nArg[2] = arg3;
		m_nArg[3] = arg4;
	}

	bool Check(IFSMConditionSource* pSrc);


	E_FSMCOND_ID GetConditionId() { return m_condId; }
	int GetArg(int i)
	{ 
		if(0<=i && i<MAX_FSMMETHOD_ARG) 
			return m_nArg[i]; 
		return 0;
	}
	int GetNumArg()
	{
		for (int i=0; i<MAX_FSMMETHOD_ARG; ++i)
		{
			if (m_nArg[i] == FSMMETHOD_ARG_NONE)
				return i;
		}
		return MAX_FSMMETHOD_ARG;
	}
};

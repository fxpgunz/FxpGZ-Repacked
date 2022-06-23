#include "stdafx.h"
#include "ZFSMCondition.h"

bool ZFSMCondition::Check(IFSMConditionSource* pSrc)
{
	if (!pSrc) { _ASSERT(0); return false; }

	switch (m_condId)
	{
	case FSMCOND_DEFAULT:
		return true;

	case FSMCOND_DICE:
		{
			int nDice = pSrc->Cond_GetDice();
			return (m_nArg[0] <= nDice && nDice < m_nArg[1]);
		}

	case FSMCOND_HP_EQUAL:
		return pSrc->Cond_GetHp() == m_nArg[0];

	case FSMCOND_HP_LESS:
		return pSrc->Cond_GetHp() < m_nArg[0];

	case FSMCOND_HP_GREATER:
		return pSrc->Cond_GetHp() > m_nArg[0];

	case FSMCOND_GROGGY_GREATER:
		return pSrc->Cond_GetGroggy() > m_nArg[0];

	case FSMCOND_VELOCITY_Z_LESS:
		{
			const rvector& vel = pSrc->Cond_GetVelocity();
			return vel.z < m_nArg[0];
		}

	case FSMCOND_VELOCITY_MAGNITUDE_LESS:
		{
			const rvector& vel = pSrc->Cond_GetVelocity();
			return Magnitude(vel) < m_nArg[0];
		}

	case FSMCOND_END_ACTION:
		return pSrc->Cond_IsEndAction();

	case FSMCOND_IS_LANDING:
		return pSrc->Cond_IsLanding();


	case FSMCOND_HAS_TARGET:
		return pSrc->Cond_HasTarget();

	case FSMCOND_HAS_NO_TARGET:
		return !pSrc->Cond_HasTarget();

	case FSMCOND_DIST_TARGET_IN:
		{
			if (!pSrc->Cond_HasTarget()) return false;
			rvector vMyPos, vTargetPos;
			pSrc->Cond_GetPos(vMyPos);
			pSrc->Cond_GetTargetPos(vTargetPos);
			float lenSq = MagnitudeSq(vMyPos - vTargetPos);
			float rangeMinSq = (m_nArg[0]*m_nArg[0]);
			float rangeMaxSq = (m_nArg[1]*m_nArg[1]);
			return rangeMinSq <= lenSq && lenSq <= rangeMaxSq;
		}

	case FSMCOND_LOOK_AT_TARGET:
		{
			rvector vMyPos, vTargetPos, vMyDir;
			pSrc->Cond_GetPos(vMyPos);
			pSrc->Cond_GetTargetPos(vTargetPos);
			pSrc->Cond_GetDirection(vMyDir);
			float fAngle = GetAngleOfVectors(vTargetPos-vMyPos, vMyDir);
			float fDegree = fabs(ToDegree(fAngle));
			return fDegree <= m_nArg[0];
		}

	case FSMCOND_IS_EMPTY_SPACE:
		{
			float fAngle = m_nArg[0];
			float fDist = m_nArg[1];

			return !pSrc->Cond_Pick(fAngle, fDist);
		}

	case FSMCOND_TIME_ELAPSED_SINCE_ENTERED:
		{
			DWORD dwElapsed = timeGetTime() - pSrc->Cond_GetStateEnteredTime();
			return dwElapsed > (DWORD)m_nArg[0];
		}

	case FSMCOND_END_LIGHTNINGDAMAGE:
		{
			bool bActive = pSrc->IsActiveLightningDamageModule();
			return !bActive;
		}
	case FSMCOND_CAN_SEE_TARGET:
		{
			return pSrc->Cond_CanSeeTarget();
		}
	case FSMCOND_FAILED_BUILD_WAYPOINTS:
		{
			return pSrc->Cond_FailedBuildWaypoints();
		}
	case FSMCOND_ANGLE_TARGET_HEIGHT:
		{	
			int low = m_nArg[0];
			int high = m_nArg[1];
			return pSrc->Cond_AngleTargetHeight(low,high);
		}
	case FSMCOND_TARGET_HEIGHT_HIGHER:
		{
			return pSrc->Cond_TargetHeightHigher(m_nArg[0]);
		}
	case FSMCOND_CANT_SEE_TARGET:
		{
			return pSrc->Cond_CantSeeTarget();
		}
	case FSMCOND_SUMMONLESS:
		{
			return pSrc->Cond_SummonLess(m_nArg[0]);
		}
	}

	mlog("ERROR : undefined fsm condition id.\n");
	_ASSERT(0);
	return false;
}
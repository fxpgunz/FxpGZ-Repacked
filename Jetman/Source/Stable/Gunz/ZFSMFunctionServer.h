#pragma once
#include "ZFSMGlobal.h"

// FSM에서 액터에게 요청할 수 있는 기능의 목록을 가지고 있고 그 기능의 실행을 처리해준다
// FSM을 사용하는 액터는 이 인터페이스를 구현해야 한다
class IFSMFunctionServer
{
public:
	virtual bool ExecuteFunction(float fFrameDelta, int id, int nArg1=0, int nArg2=0, int nArg3=0, int nArg4=0) = 0;
};


// 신규퀘스트에서 액터가 FSM를 위해 처리해줄 수 있는 펑션들의 구현체
class ZFSMFunctionServer : public IFSMFunctionServer
{
public:
	enum {
		FSMFUNC_INVALID = -1,
		FSMFUNC_DONOTHING,
		FSMFUNC_DICE,

		FSMFUNC_FIND_TARGET,
		FSMFUNC_SET_TARGET_LAST_ATTACKER,	// 마지막으로 자신을 공격한 상대를 타겟으로 선택

		FSMFUNC_ROTATE_TO_TARGET,			// 적을 향해 일정한 속도로 회전
		FSMFUNC_FACE_TO_TARGET,				// 적을 향해 즉시 회전

		FSMFUNC_REDUCE_GROGGY,

		FSMFUNC_BUILD_WAYPOINTS_TO_TARGET,
		FSMFUNC_RUN_WAYPOINTS,

		// built-in state에서만 사용하는 것들 (따라서 파서는 없음)
		FSMFUNC_DISAPPEAR,					// 시간에 따라 메쉬 투명해짐(죽었을때 사용)
		FSMFUNC_POST_DEAD,					// 서버에 죽음 알림
		FSMFUNC_ON_ENTER_BLASTED_SOAR,		// 장검 띄우기 피격 진입
		FSMFUNC_ON_ENTER_BLASTED_THRUST,	// 단검 밀치기 피격 진입
		FSMFUNC_ON_UPDATE_BLASTED_THRUST,	// 단검 밀치기 피격 업데이트
		FSMFUNC_EXIT_SUFFERING,				// 조작불능 피격 플래그 끄기
		FSMFUNC_ON_ENTER_DIE,
		FSMFUNC_FIND_TARGET_IN_HEIGHT,
		FSMFUNC_RUN_ALONG_TARGET_ORBITAL,
		FSMFUNC_CLEAR_WAYPOINTS,
		FSMFUNC_SPEED_ACCEL,
		FSMFUNC_TURN_ORBITAL_DIR,
		FSMFUNC_RUNWAYPOINTSALONGROUTE,
		FSMFUNC_FINDTARGETINDIST,
		FSMFUNC_FACE_TO_LATEST_ATTACKER,
	};
public:
	ZFSMFunctionServer();

	virtual bool ExecuteFunction(float fFrameDelta, int funcId, int nArg1=0, int nArg2=0, int nArg3=0, int nArg4=0);

	int GetDice() { return m_nDice; }

protected:
	virtual void Func_Dice(int nMin, int nMax);
	virtual void Func_FindTarget() { _ASSERT(0); }
	virtual void Func_SetTargetLastAttacker() { _ASSERT(0); }

	virtual void Func_RotateToTarget(float fFrameDelta) { _ASSERT(0); }
	virtual void Func_FaceToTarget() { _ASSERT(0); }

	virtual void Func_ReduceGroggy(float f) { _ASSERT(0); }

	virtual void Func_BuildWaypointsToTarget() { _ASSERT(0); }
	virtual void Func_RunWaypoints(float fFrameDelta) { _ASSERT(0); }
	
	virtual void Func_Disappear(float fFrameDelta, DWORD nDelay, float fRatio) { _ASSERT(0); }
	virtual void Func_PostDead() { _ASSERT(0); }
	virtual void Func_OnEnterBlastedSoar() { _ASSERT(0); }
	virtual void Func_OnEnterBlastedThrust() { _ASSERT(0); }
	virtual void Func_OnUpdateBlastedThrust(float fFrameDelta) { _ASSERT(0); }
	virtual void Func_ExitSuffering() { _ASSERT(0); }
	virtual void Func_OnEnterDie() { _ASSERT(0); }
	virtual void Func_FindTargetInHeight(int height) { _ASSERT(0); }
	virtual void Func_RunAlongTargetOrbital(int dist,float fDelta) { _ASSERT(0); }
	virtual void Func_ClearWayPoints() { _ASSERT(0); }
	virtual void Func_SpeedAccel(int speedAcce) { _ASSERT(0); }
	virtual void Func_TurnOrbitalDirection() { _ASSERT(0); }
	virtual void Func_RunWayPointsAlongRoute(float fDelta) {_ASSERT(0); }
	virtual void Func_FindTargetInDist(int dist) { _ASSERT(0); }
	virtual void Func_FaceToLatestAttacker() { _ASSERT(0); }
private:
	int m_nDice;
};


// 펑션 호출 저장객체
struct ZFSMFunctionCall
{
	int id;
	int nArg[MAX_FSMMETHOD_ARG];

	void Clear() {
		id = ZFSMFunctionServer::FSMFUNC_INVALID;
		for (int i=0; i<MAX_FSMMETHOD_ARG; ++i)
			nArg[i] = FSMMETHOD_ARG_NONE;
	}
};
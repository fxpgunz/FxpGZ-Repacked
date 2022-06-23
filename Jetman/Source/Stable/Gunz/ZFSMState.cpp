#include "stdafx.h"
#include "ZFSMState.h"
#include "ZFSMTransition.h"
#include "ZActorAction.h"
#include "ZActorActionManager.h"

ZFSMState::ZFSMState() : m_nCooltime(0), m_pAction(NULL)
{

}

ZFSMState::~ZFSMState()
{
	int n = (int)m_vecTrans.size();
	for (int i=0; i<n; ++i)
		delete m_vecTrans[i];
}

void ZFSMState::SetCooltime( int n )
{
	m_nCooltime=n; 
	if (n<0) 
	{
		m_nCooltime=0;
		_ASSERT(0);
	}
}

void ZFSMState::SetAction( IActorAction* pAction )
{
	m_pAction=pAction;
}

void ZFSMState::AddTransition( ZFSMTransition* pTrans )
{
	m_vecTrans.push_back(pTrans);
}

ZFSMState* ZFSMState::CheckTransition( IFSMConditionSource* pCondSrc )
{
	if (!pCondSrc) return NULL;
	
	//todok del
	//mlog("전이검사 시작-----\n");

	int n = (int)m_vecTrans.size();
	for (int i=0; i<n; ++i)
	{
		//todok del
		/*mlog("#### check for state : %s\n", m_vecTrans[i]->GetNextStateName());
		for (int z=0; z<m_vecTrans[i]->CountCondition(); ++z)
			mlog("# condid : %d -----\n", m_vecTrans[i]->GetCondition(z)->GetConditionId());*/

		if (m_vecTrans[i]->Check(pCondSrc))
			return m_vecTrans[i]->GetNextState();
	}
	return NULL;
}

ZFSMTransition* ZFSMState::GetTransition(int idx)
{
	if (0 <= idx && idx < (int)m_vecTrans.size())
		return m_vecTrans[idx];
	return NULL;
}

void ZFSMState::SetFunctionCalls( vector<ZFSMFunctionCall>& v )
{
	m_vecFuncCall = v;
}

const ZFSMFunctionCall* ZFSMState::GetFunctionCall( unsigned int i )
{
	if (i < (int)m_vecFuncCall.size())
		return &m_vecFuncCall[i];
	return NULL;
}

void ZFSMState::SetEnterFunctionCalls( vector<ZFSMFunctionCall>& v )
{
	m_vecEnterFuncCall = v;
}

const ZFSMFunctionCall* ZFSMState::GetEnterFunctionCall( unsigned int i )
{
	if (i < (int)m_vecEnterFuncCall.size())
		return &m_vecEnterFuncCall[i];
	return NULL;
}

void ZFSMState::SetExitFunctionCalls( vector<ZFSMFunctionCall>& v )
{
	m_vecExitFuncCall = v;
}

const ZFSMFunctionCall* ZFSMState::GetExitFunctionCall( unsigned int i )
{
	if (i < (int)m_vecExitFuncCall.size())
		return &m_vecExitFuncCall[i];
	return NULL;
}

void ZFSMState::MakeDiceFuncHasMaxValue()
{
	int sum = 0;

	// 펑션 "dice"가 랜덤값을 구할 max치를 인자로 가지도록 수정한다
	ZFSMTransition* pTrans;
	int numCond = 0;
	for (int i=0; i<(int)m_vecTrans.size(); ++i)
	{
		pTrans = m_vecTrans[i];
		numCond = pTrans->CountCondition();
		for (int k=0; k<numCond; ++k)
		{
			if (FSMCOND_DICE == pTrans->GetCondition(k)->GetConditionId())
			{
				sum += pTrans->GetCondition(k)->GetArg(0);
				
				// xml에 정의된대로 인자는 1개여야 한다. 2개라면 MakeDiceConditionHasRange()이 먼저 실행되었을 가능성이 있다
				_ASSERT(1 == pTrans->GetCondition(k)->GetNumArg());
			}
		}
	}

	for (int i=0; i<(int)m_vecEnterFuncCall.size(); ++i)
	{
		if (ZFSMFunctionServer::FSMFUNC_DICE == m_vecEnterFuncCall[i].id)
		{
			m_vecEnterFuncCall[i].nArg[0] = sum;
		}
	}
	for (int i=0; i<(int)m_vecFuncCall.size(); ++i)
	{
		if (ZFSMFunctionServer::FSMFUNC_DICE == m_vecFuncCall[i].id)
		{
			m_vecFuncCall[i].nArg[0] = sum;
		}
	}
}

void ZFSMState::MakeDiceConditionHasRange()
{
	// 조건문 "dice" 는 만들어진 랜덤값에 따라 분기를 일으키기 위한 조건문이다.
	// 그런데 "dice" 조건문은 xml에서 아래와 같이 정의된다.
	//
	// <STATE name="combat" cooltime="0" func="dice">
	//		<TRANS cond="dice:5" next="tumbleLeft"/>
	//		<TRANS cond="dice:5" next="tumbleRight"/>
	//		<TRANS cond="dice:5,distTarget:3;4" next="dashFoward"/>
	//		<TRANS cond="dice:5,distTarget:4;5" next="dashBackward"/>
	// </STATE>
	//
	// 랜덤한 값을 기준으로 분기를 일으키기 위해서는 [0~10이면 A행동, 10~30이면 B행동, 30~100이면 C행동]과 같이 범위를
	// 지정해서 분기 확률을 설정할 수 있어야 한다.
	// 그런데 이게 좀 불편하니까 xml 작성자의 편의를 위해 dice:10 과 같이 숫자를 1개만 쓰게 한다. (범위가 아닌 확률 수치)
	// 그렇게 xml을 파싱하고 난 뒤에, 실제로 FSM을 구동하려면 범위값으로 지정되어 있어야 하므로
	// 확률 인자를 범위 인자로 변경해주는 작업을 이 함수에서 한다.

	// 모든 transition에서 dice조건문이 있는 condition의 arg를 새로 지정해준다
	int sum = 0;
	int range = 0;
	int numTrans = (int)m_vecTrans.size();
	for (int i=0; i<numTrans; ++i)
	{
		ZFSMCondition* pCond = m_vecTrans[i]->FindCondition(FSMCOND_DICE);
		if (pCond)
		{
			range = pCond->GetArg(0);	// 원래 지정되어 있던 확률량
			pCond->Init(FSMCOND_DICE, sum, sum+range);
			sum += range;
		}
	}
}


// 죽는 스테이트 생성
ZFSMState* ZFSMState::CreateBuiltInState_Die(ZActorActionManager* pActionMgr)
{
	// 죽는 액션이 없으면 생성
	ZActorAction* pAction = pActionMgr->GetAction(FSM_BIACTION_DIE);
	if (!pAction)
	{
		pAction = new ZActorAction;
		pAction->SetName(FSM_BIACTION_DIE);
		pAction->SetAnimationName(FSM_ANIMATION_DIE);
		pActionMgr->AddAction(pAction);
	}

	vector<ZFSMFunctionCall> vecEnterFunc;
	vector<ZFSMFunctionCall> vecFunc;
	ZFSMFunctionCall func;

	func.id = ZFSMFunctionServer::FSMFUNC_ON_ENTER_DIE;
	vecEnterFunc.push_back(func);

	func.id = ZFSMFunctionServer::FSMFUNC_DISAPPEAR;
	func.nArg[0] = 1000;	// 이 시간(ms)만큼 기다린후 투명화 시작
	func.nArg[1] = 50;		// 초당 투명해지는 속도 (ex:50이면 1초에 50% 투명해짐)
	vecFunc.push_back(func);

	// 완전히 사라지는데 걸리는 시간을 계산
	int timeDisappearComplete = 1000;
	if (func.nArg[1] != 0)
	{
		timeDisappearComplete = 100/func.nArg[1] * 1000;
	}

	// 죽어서 시체가 사라지는 효과가 끝나면 삭제대기 상태로 전이
	ZFSMCondition* pCond = new ZFSMCondition;
	pCond->Init(FSMCOND_TIME_ELAPSED_SINCE_ENTERED, func.nArg[0] + timeDisappearComplete);
	ZFSMTransition* pTrans = new ZFSMTransition;
	pTrans->AddCondition(pCond);
	pTrans->SetNextStateName(FSM_BISTATE_WAIT_DELETE);

	ZFSMState* pState = new ZFSMState;
	pState->SetName(FSM_BISTATE_DIE);
	pState->SetEnterFunctionCalls(vecEnterFunc);
	pState->SetFunctionCalls(vecFunc);
	pState->SetAction(pAction);
	pState->AddTransition(pTrans);

	return pState;
}

// 죽어서 삭제 대기 중인 스테이트 생성
ZFSMState* ZFSMState::CreateBuiltInState_WaitDelete(ZActorActionManager* pActionMgr)
{
	// 액션도, 다른 상태로의 전이 조건도 없다

	vector<ZFSMFunctionCall> vecEnterFunc;
	ZFSMFunctionCall func;

	func.id = ZFSMFunctionServer::FSMFUNC_POST_DEAD;
	vecEnterFunc.push_back(func);

	ZFSMState* pState = new ZFSMState;
	pState->SetName(FSM_BISTATE_WAIT_DELETE);
	pState->SetEnterFunctionCalls(vecEnterFunc);

	return pState;
}


// 장검 띄우기 당하는 스테이트 생성
ZFSMState* ZFSMState::CreateBuiltInState_BlastedSoar(ZActorActionManager* pActionMgr)
{
	// 띄우기 당하는 액션이 없으면 생성
	ZActorAction* pAction = pActionMgr->GetAction(FSM_BIACTION_BLASTEDSOAR);
	if (!pAction) 
	{
		pAction = new ZActorAction;
		pAction->SetName(FSM_BIACTION_BLASTEDSOAR);
		pAction->SetAnimationName(FSM_ANIMATION_BLASTEDSOAR);
		pActionMgr->AddAction(pAction);
	}

	// velocity.z 가 0보다 작아지면 띄우기 후 추락하는 스테이트로 전이
	ZFSMCondition* pCond = new ZFSMCondition;
	pCond->Init(FSMCOND_VELOCITY_Z_LESS, 0);
	ZFSMTransition* pTrans = new ZFSMTransition;
	pTrans->AddCondition(pCond);
	pTrans->SetNextStateName(FSM_BISTATE_BLASTEDFALL);

	vector<ZFSMFunctionCall> vecFunc;
	ZFSMFunctionCall func;
	func.id = ZFSMFunctionServer::FSMFUNC_ON_ENTER_BLASTED_SOAR;
	vecFunc.push_back(func);

	ZFSMState* pState = new ZFSMState;
	pState->SetName(FSM_BISTATE_BLASTEDSOAR);
	pState->SetAction(pAction);
	pState->SetEnterFunctionCalls(vecFunc);
	pState->AddTransition(pTrans);

	return pState;
}

// 띄우기 당해서 추락하는 스테이트 생성
ZFSMState* ZFSMState::CreateBuiltInState_BlastedFall(ZActorActionManager* pActionMgr)
{
	// 띄우기 당하는 액션이 없으면 생성
	ZActorAction* pAction = pActionMgr->GetAction(FSM_BIACTION_BLASTEDFALL);
	if (!pAction) 
	{
		pAction = new ZActorAction;
		pAction->SetName(FSM_BIACTION_BLASTEDFALL);
		pAction->SetAnimationName(FSM_ANIMATION_BLASTEDFALL);
		pActionMgr->AddAction(pAction);
	}

	// 착지 시점이면 쓰러진 스테이트로 전이
	ZFSMCondition* pCond = new ZFSMCondition;
	pCond->Init(FSMCOND_IS_LANDING);
	ZFSMTransition* pTrans = new ZFSMTransition;
	pTrans->AddCondition(pCond);
	pTrans->SetNextStateName(FSM_BISTATE_BLASTEDDOWN);

	ZFSMState* pState = new ZFSMState;
	pState->SetName(FSM_BISTATE_BLASTEDFALL);
	pState->SetAction(pAction);
	pState->AddTransition(pTrans);

	return pState;
}

// 띄우기 추락 후 바닥에 쓰러지는 스테이트 생성
ZFSMState* ZFSMState::CreateBuiltInState_BlastedDown(ZActorActionManager* pActionMgr)
{
	// 쓰러지는 액션이 없으면 생성
	ZActorAction* pAction = pActionMgr->GetAction(FSM_BIACTION_BLASTEDDOWN);
	if (!pAction) 
	{
		pAction = new ZActorAction;
		pAction->SetName(FSM_BIACTION_BLASTEDDOWN);
		pAction->SetAnimationName(FSM_ANIMATION_BLASTEDDOWN);
		pActionMgr->AddAction(pAction);
	}

	// 쓰러진 애니 끝나면 일어나는 스테이트로 전이
	ZFSMCondition* pCond = new ZFSMCondition;
	pCond->Init(FSMCOND_END_ACTION);
	ZFSMTransition* pTrans = new ZFSMTransition;
	pTrans->AddCondition(pCond);
	pTrans->SetNextStateName(FSM_BISTATE_BLASTEDSTANDUP);

	ZFSMState* pState = new ZFSMState;
	pState->SetName(FSM_BISTATE_BLASTEDDOWN);
	pState->SetAction(pAction);
	pState->AddTransition(pTrans);

	return pState;
}

// 띄우기 후 바닥에서 일어나는 스테이트 생성
ZFSMState* ZFSMState::CreateBuiltInState_BlastedStandup(ZActorActionManager* pActionMgr)
{
	// 쓰러지는 액션이 없으면 생성
	ZActorAction* pAction = pActionMgr->GetAction(FSM_BIACTION_BLASTEDSTANDUP);
	if (!pAction) 
	{
		pAction = new ZActorAction;
		pAction->SetName(FSM_BIACTION_BLASTEDSTANDUP);
		pAction->SetAnimationName(FSM_ANIMATION_STANDUP);
		pActionMgr->AddAction(pAction);
	}

	// 쓰러진 애니 끝나면 일어나는 스테이트로 전이
	ZFSMCondition* pCond = new ZFSMCondition;
	pCond->Init(FSMCOND_END_ACTION);
	ZFSMTransition* pTrans = new ZFSMTransition;
	pTrans->AddCondition(pCond);
	pTrans->SetNextStateName(FSM_STATE_AFTERBLASTED);

	ZFSMState* pState = new ZFSMState;
	pState->SetName(FSM_BISTATE_BLASTEDSTANDUP);
	pState->SetAction(pAction);
	pState->AddTransition(pTrans);

	return pState;
}

// 단검 밀치기 당하는 스테이트 생성
ZFSMState* ZFSMState::CreateBuiltInState_BlastedThrust(ZActorActionManager* pActionMgr)
{
	// 밀치기 당하는 액션이 없으면 생성
	ZActorAction* pAction = pActionMgr->GetAction(FSM_BIACTION_BLASTEDTHRUST);
	if (!pAction) 
	{
		pAction = new ZActorAction;
		pAction->SetName(FSM_BIACTION_BLASTEDTHRUST);
		pAction->SetAnimationName(FSM_ANIMATION_BLASTEDTHRUST);
		pActionMgr->AddAction(pAction);
	}

	// velocity.z 가 0보다 작아지면 띄우기 후 추락하는 스테이트로 전이
	ZFSMCondition* pCond = new ZFSMCondition;
	pCond->Init(FSMCOND_VELOCITY_MAGNITUDE_LESS, 20);
	ZFSMTransition* pTrans = new ZFSMTransition;
	pTrans->AddCondition(pCond);
	pTrans->SetNextStateName(FSM_BISTATE_BLASTEDTHRUSTDOWN);

	ZFSMFunctionCall func;

	vector<ZFSMFunctionCall> vecEnterFunc;
	func.id = ZFSMFunctionServer::FSMFUNC_ON_ENTER_BLASTED_THRUST;
	vecEnterFunc.push_back(func);

	vector<ZFSMFunctionCall> vecFunc;
	func.id = ZFSMFunctionServer::FSMFUNC_ON_UPDATE_BLASTED_THRUST;
	vecFunc.push_back(func);

	vector<ZFSMFunctionCall> vecExitFunc;
	func.id = ZFSMFunctionServer::FSMFUNC_EXIT_SUFFERING;
	vecExitFunc.push_back(func);

	ZFSMState* pState = new ZFSMState;
	pState->SetName(FSM_BISTATE_BLASTEDTHRUST);
	pState->SetAction(pAction);
	pState->SetEnterFunctionCalls(vecEnterFunc);
	pState->SetFunctionCalls(vecFunc);
	pState->SetExitFunctionCalls(vecExitFunc);
	pState->AddTransition(pTrans);

	return pState;
}

// 단검 밀치기 후 쓰러지는 스테이트 생성
ZFSMState* ZFSMState::CreateBuiltInState_BlastedThrustDown(ZActorActionManager* pActionMgr)
{
	// 밀치기 당하는 액션이 없으면 생성
	ZActorAction* pAction = pActionMgr->GetAction(FSM_BIACTION_BLASTEDTHRUSTDOWN);
	if (!pAction) 
	{
		pAction = new ZActorAction;
		pAction->SetName(FSM_BIACTION_BLASTEDTHRUSTDOWN);
		pAction->SetAnimationName(FSM_ANIMATION_BLASTEDTHRUSTDOWN);
		pActionMgr->AddAction(pAction);
	}

	// 애니메이션 끝나면 일어나는 스테이트로 전이
	ZFSMCondition* pCond = new ZFSMCondition;
	pCond->Init(FSMCOND_END_ACTION);
	ZFSMTransition* pTrans = new ZFSMTransition;
	pTrans->AddCondition(pCond);
	pTrans->SetNextStateName(FSM_BISTATE_BLASTEDSTANDUP);

	ZFSMState* pState = new ZFSMState;
	pState->SetName(FSM_BISTATE_BLASTEDTHRUSTDOWN);
	pState->SetAction(pAction);
	pState->AddTransition(pTrans);

	return pState;
}

// 라이트닝 강베기맞고 스턴되는 스테이트 생성
ZFSMState* ZFSMState::CreateBuiltInState_LightningStun(ZActorActionManager* pActionMgr)
{
	// 라이트닝 강베기맞은 액션이 없으면 생성
	ZActorAction* pAction = pActionMgr->GetAction(FSM_BIACTION_LIGHTNINGSTUN);
	if (!pAction) 
	{
		pAction = new ZActorAction;
		pAction->SetName(FSM_BIACTION_LIGHTNINGSTUN);
		pAction->SetAnimationName(FSM_ANIMATION_LIGHTNINGSTUN);
		pActionMgr->AddAction(pAction);
	}

	// 라이트닝대미지가 끝날 때까지 상태 유지
	ZFSMCondition* pCond = new ZFSMCondition;
	pCond->Init(FSMCOND_END_LIGHTNINGDAMAGE);
	ZFSMTransition* pTrans = new ZFSMTransition;
	pTrans->AddCondition(pCond);
	pTrans->SetNextStateName(FSM_STATE_AFTERBLASTED);

	ZFSMState* pState = new ZFSMState;
	pState->SetName(FSM_BISTATE_LIGHTNINGSTUN);
	pState->SetAction(pAction);
	pState->AddTransition(pTrans);

	return pState;
}
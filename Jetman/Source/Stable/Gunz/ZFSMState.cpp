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
	//mlog("���̰˻� ����-----\n");

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

	// ��� "dice"�� �������� ���� maxġ�� ���ڷ� �������� �����Ѵ�
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
				
				// xml�� ���ǵȴ�� ���ڴ� 1������ �Ѵ�. 2����� MakeDiceConditionHasRange()�� ���� ����Ǿ��� ���ɼ��� �ִ�
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
	// ���ǹ� "dice" �� ������� �������� ���� �б⸦ ����Ű�� ���� ���ǹ��̴�.
	// �׷��� "dice" ���ǹ��� xml���� �Ʒ��� ���� ���ǵȴ�.
	//
	// <STATE name="combat" cooltime="0" func="dice">
	//		<TRANS cond="dice:5" next="tumbleLeft"/>
	//		<TRANS cond="dice:5" next="tumbleRight"/>
	//		<TRANS cond="dice:5,distTarget:3;4" next="dashFoward"/>
	//		<TRANS cond="dice:5,distTarget:4;5" next="dashBackward"/>
	// </STATE>
	//
	// ������ ���� �������� �б⸦ ����Ű�� ���ؼ��� [0~10�̸� A�ൿ, 10~30�̸� B�ൿ, 30~100�̸� C�ൿ]�� ���� ������
	// �����ؼ� �б� Ȯ���� ������ �� �־�� �Ѵ�.
	// �׷��� �̰� �� �����ϴϱ� xml �ۼ����� ���Ǹ� ���� dice:10 �� ���� ���ڸ� 1���� ���� �Ѵ�. (������ �ƴ� Ȯ�� ��ġ)
	// �׷��� xml�� �Ľ��ϰ� �� �ڿ�, ������ FSM�� �����Ϸ��� ���������� �����Ǿ� �־�� �ϹǷ�
	// Ȯ�� ���ڸ� ���� ���ڷ� �������ִ� �۾��� �� �Լ����� �Ѵ�.

	// ��� transition���� dice���ǹ��� �ִ� condition�� arg�� ���� �������ش�
	int sum = 0;
	int range = 0;
	int numTrans = (int)m_vecTrans.size();
	for (int i=0; i<numTrans; ++i)
	{
		ZFSMCondition* pCond = m_vecTrans[i]->FindCondition(FSMCOND_DICE);
		if (pCond)
		{
			range = pCond->GetArg(0);	// ���� �����Ǿ� �ִ� Ȯ����
			pCond->Init(FSMCOND_DICE, sum, sum+range);
			sum += range;
		}
	}
}


// �״� ������Ʈ ����
ZFSMState* ZFSMState::CreateBuiltInState_Die(ZActorActionManager* pActionMgr)
{
	// �״� �׼��� ������ ����
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
	func.nArg[0] = 1000;	// �� �ð�(ms)��ŭ ��ٸ��� ����ȭ ����
	func.nArg[1] = 50;		// �ʴ� ���������� �ӵ� (ex:50�̸� 1�ʿ� 50% ��������)
	vecFunc.push_back(func);

	// ������ ������µ� �ɸ��� �ð��� ���
	int timeDisappearComplete = 1000;
	if (func.nArg[1] != 0)
	{
		timeDisappearComplete = 100/func.nArg[1] * 1000;
	}

	// �׾ ��ü�� ������� ȿ���� ������ ������� ���·� ����
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

// �׾ ���� ��� ���� ������Ʈ ����
ZFSMState* ZFSMState::CreateBuiltInState_WaitDelete(ZActorActionManager* pActionMgr)
{
	// �׼ǵ�, �ٸ� ���·��� ���� ���ǵ� ����

	vector<ZFSMFunctionCall> vecEnterFunc;
	ZFSMFunctionCall func;

	func.id = ZFSMFunctionServer::FSMFUNC_POST_DEAD;
	vecEnterFunc.push_back(func);

	ZFSMState* pState = new ZFSMState;
	pState->SetName(FSM_BISTATE_WAIT_DELETE);
	pState->SetEnterFunctionCalls(vecEnterFunc);

	return pState;
}


// ��� ���� ���ϴ� ������Ʈ ����
ZFSMState* ZFSMState::CreateBuiltInState_BlastedSoar(ZActorActionManager* pActionMgr)
{
	// ���� ���ϴ� �׼��� ������ ����
	ZActorAction* pAction = pActionMgr->GetAction(FSM_BIACTION_BLASTEDSOAR);
	if (!pAction) 
	{
		pAction = new ZActorAction;
		pAction->SetName(FSM_BIACTION_BLASTEDSOAR);
		pAction->SetAnimationName(FSM_ANIMATION_BLASTEDSOAR);
		pActionMgr->AddAction(pAction);
	}

	// velocity.z �� 0���� �۾����� ���� �� �߶��ϴ� ������Ʈ�� ����
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

// ���� ���ؼ� �߶��ϴ� ������Ʈ ����
ZFSMState* ZFSMState::CreateBuiltInState_BlastedFall(ZActorActionManager* pActionMgr)
{
	// ���� ���ϴ� �׼��� ������ ����
	ZActorAction* pAction = pActionMgr->GetAction(FSM_BIACTION_BLASTEDFALL);
	if (!pAction) 
	{
		pAction = new ZActorAction;
		pAction->SetName(FSM_BIACTION_BLASTEDFALL);
		pAction->SetAnimationName(FSM_ANIMATION_BLASTEDFALL);
		pActionMgr->AddAction(pAction);
	}

	// ���� �����̸� ������ ������Ʈ�� ����
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

// ���� �߶� �� �ٴڿ� �������� ������Ʈ ����
ZFSMState* ZFSMState::CreateBuiltInState_BlastedDown(ZActorActionManager* pActionMgr)
{
	// �������� �׼��� ������ ����
	ZActorAction* pAction = pActionMgr->GetAction(FSM_BIACTION_BLASTEDDOWN);
	if (!pAction) 
	{
		pAction = new ZActorAction;
		pAction->SetName(FSM_BIACTION_BLASTEDDOWN);
		pAction->SetAnimationName(FSM_ANIMATION_BLASTEDDOWN);
		pActionMgr->AddAction(pAction);
	}

	// ������ �ִ� ������ �Ͼ�� ������Ʈ�� ����
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

// ���� �� �ٴڿ��� �Ͼ�� ������Ʈ ����
ZFSMState* ZFSMState::CreateBuiltInState_BlastedStandup(ZActorActionManager* pActionMgr)
{
	// �������� �׼��� ������ ����
	ZActorAction* pAction = pActionMgr->GetAction(FSM_BIACTION_BLASTEDSTANDUP);
	if (!pAction) 
	{
		pAction = new ZActorAction;
		pAction->SetName(FSM_BIACTION_BLASTEDSTANDUP);
		pAction->SetAnimationName(FSM_ANIMATION_STANDUP);
		pActionMgr->AddAction(pAction);
	}

	// ������ �ִ� ������ �Ͼ�� ������Ʈ�� ����
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

// �ܰ� ��ġ�� ���ϴ� ������Ʈ ����
ZFSMState* ZFSMState::CreateBuiltInState_BlastedThrust(ZActorActionManager* pActionMgr)
{
	// ��ġ�� ���ϴ� �׼��� ������ ����
	ZActorAction* pAction = pActionMgr->GetAction(FSM_BIACTION_BLASTEDTHRUST);
	if (!pAction) 
	{
		pAction = new ZActorAction;
		pAction->SetName(FSM_BIACTION_BLASTEDTHRUST);
		pAction->SetAnimationName(FSM_ANIMATION_BLASTEDTHRUST);
		pActionMgr->AddAction(pAction);
	}

	// velocity.z �� 0���� �۾����� ���� �� �߶��ϴ� ������Ʈ�� ����
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

// �ܰ� ��ġ�� �� �������� ������Ʈ ����
ZFSMState* ZFSMState::CreateBuiltInState_BlastedThrustDown(ZActorActionManager* pActionMgr)
{
	// ��ġ�� ���ϴ� �׼��� ������ ����
	ZActorAction* pAction = pActionMgr->GetAction(FSM_BIACTION_BLASTEDTHRUSTDOWN);
	if (!pAction) 
	{
		pAction = new ZActorAction;
		pAction->SetName(FSM_BIACTION_BLASTEDTHRUSTDOWN);
		pAction->SetAnimationName(FSM_ANIMATION_BLASTEDTHRUSTDOWN);
		pActionMgr->AddAction(pAction);
	}

	// �ִϸ��̼� ������ �Ͼ�� ������Ʈ�� ����
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

// ����Ʈ�� ������°� ���ϵǴ� ������Ʈ ����
ZFSMState* ZFSMState::CreateBuiltInState_LightningStun(ZActorActionManager* pActionMgr)
{
	// ����Ʈ�� ��������� �׼��� ������ ����
	ZActorAction* pAction = pActionMgr->GetAction(FSM_BIACTION_LIGHTNINGSTUN);
	if (!pAction) 
	{
		pAction = new ZActorAction;
		pAction->SetName(FSM_BIACTION_LIGHTNINGSTUN);
		pAction->SetAnimationName(FSM_ANIMATION_LIGHTNINGSTUN);
		pActionMgr->AddAction(pAction);
	}

	// ����Ʈ�״������ ���� ������ ���� ����
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
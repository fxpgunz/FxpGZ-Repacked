#include "stdafx.h"

#ifdef _TDD		// 모든 단위 테스트 관련 코드는 _TDD로 감싸도록 합니다
#include <gtest/gtest.h>
#include "ZFSMState.h"
#include "ZFSMTransition.h"
#include "ZFSMConditionSourceMock.h"
#include "ZActorAction.h"
#include "ZFSMFunctionServer.h"

class ZActionDummy : public IActorAction
{
public:
	virtual ~ZActionDummy() {}
	virtual const char* GetName() { return ""; }
	virtual const char* GetAnimationName() { return ""; };
	virtual bool IsMovingAnimation() { return false; }
	virtual const ZActorActionMeleeShot* GetMeleeShot() { return NULL; }
	virtual const ZActorActionRangeShot* GetRangeShot(int idx) { return NULL; }
	virtual int GetNumRangeShot() { return 0; }
};

TEST(ZFSMState, InitFSMState)
{
	ZFSMState* pState = new ZFSMState;
	
	pState->SetName("idle");
	EXPECT_STREQ("idle", pState->GetName());

	pState->SetCooltime(100);
	EXPECT_EQ(100, pState->GetCooltime());

	// 액션은 NULL도 유효함
	pState->SetAction(NULL);
	EXPECT_EQ(NULL, pState->GetAction());

	ZActionDummy* pAction = new ZActionDummy;
	pState->SetAction(pAction);
	EXPECT_EQ(pAction, pState->GetAction());

	delete pState;
	delete pAction;
}

TEST(ZFSMState, CheckTransition)
{
	ZFSMState* pStateNormal = new ZFSMState;
	ZFSMState* pStateFlee = new ZFSMState;

	ZFSMCondition* pCond = new ZFSMCondition;
	pCond->Init(FSMCOND_HP_LESS, 10);

	ZFSMTransition* pTrans = new ZFSMTransition;
	pTrans->AddCondition(pCond);
	pTrans->SetNextState(pStateFlee);

	pStateNormal->AddTransition(pTrans);

	// 체력을 가진 가짜객체
	ZFSMConditionSourceMock condSrc;
	condSrc.m_fHp = 80;

	// 체력을 점점 낮춰가며 전이검사 해본다
	ZFSMState* pNextState = pStateNormal->CheckTransition(&condSrc);
	EXPECT_EQ(NULL, pNextState);	// 전이가 일어나지 않으면 NULL을 리턴

	condSrc.m_fHp = 3;
	pNextState = pStateNormal->CheckTransition(&condSrc);
	EXPECT_EQ(pStateFlee, pNextState);	// 전이가 일어나야 하면 전이할 state 객체의 포인터를 리턴

	// 받은 ZFSMTransition객체는 알아서 해제한다
	delete pStateNormal;
	delete pStateFlee;
}

TEST(ZFSMState, HasFunctionCallList)
{
	// 대략 이런 값의 id와 인자를 가진 펑션콜이 있다고 하자
	ZFSMFunctionCall funcCall1;
	funcCall1.id = 23;
	funcCall1.nArg[0] = 4;
	funcCall1.nArg[1] = 0;
	funcCall1.nArg[2] = 5;
	funcCall1.nArg[3] = 0;
	ZFSMFunctionCall funcCall2;
	funcCall2.id = 1;
	funcCall2.nArg[0] = 2;
	funcCall2.nArg[1] = 99;
	funcCall2.nArg[2] = 0;
	funcCall2.nArg[3] = 0;

	vector<ZFSMFunctionCall> vecFunctionCall;
	vecFunctionCall.push_back(funcCall1);
	vecFunctionCall.push_back(funcCall2);
	
	// state에 집어넣어본다
	ZFSMState state;
	state.SetFunctionCalls(vecFunctionCall);

	// 꺼내서 비교해본다
	EXPECT_EQ(2, state.GetFunctionCallCount());
	const ZFSMFunctionCall* r1 = state.GetFunctionCall(0);
	const ZFSMFunctionCall* r2 = state.GetFunctionCall(1);

	EXPECT_EQ(0, memcmp(&funcCall1, r1, sizeof(ZFSMFunctionCall)));
	EXPECT_EQ(0, memcmp(&funcCall2, r2, sizeof(ZFSMFunctionCall)));

	// 범위에 넘어선 요청처리
	EXPECT_EQ(NULL, state.GetFunctionCall(9999));
	EXPECT_EQ(NULL, state.GetFunctionCall(-1000));


	// 위와 같은 절차를 상태진입시 실행할 펑션리스트에도 적용해본다
	ZFSMFunctionCall funcCall3;
	funcCall3.id = 77;
	funcCall3.nArg[0] = 9;
	funcCall3.nArg[1] = 8;
	funcCall3.nArg[2] = 7;
	funcCall3.nArg[3] = 6;
	ZFSMFunctionCall funcCall4;
	funcCall4.id = 101;
	funcCall4.nArg[0] = 44;
	funcCall4.nArg[1] = 55;
	funcCall4.nArg[2] = 66;
	funcCall4.nArg[3] = 0;

	// 넣기
	vector<ZFSMFunctionCall> vecEnterFunctionCall;
	vecEnterFunctionCall.push_back(funcCall3);
	vecEnterFunctionCall.push_back(funcCall4);

	state.SetEnterFunctionCalls(vecEnterFunctionCall);

	// 꺼내서 비교
	EXPECT_EQ(2, state.GetEnterFunctionCallCount());
	const ZFSMFunctionCall* r3 = state.GetEnterFunctionCall(0);
	const ZFSMFunctionCall* r4 = state.GetEnterFunctionCall(1);

	EXPECT_EQ(0, memcmp(&funcCall3, r3, sizeof(ZFSMFunctionCall)));
	EXPECT_EQ(0, memcmp(&funcCall4, r4, sizeof(ZFSMFunctionCall)));

	// 범위에 넘어선 요청처리
	EXPECT_EQ(NULL, state.GetEnterFunctionCall(9999));
	EXPECT_EQ(NULL, state.GetEnterFunctionCall(-1000));
}

TEST(ZFSMState, MakeDiceConditionHasRange)
{
	// "dice" 조건문은 xml에서 파싱한 뒤에 특별한 처리를 거쳐야 실제로 사용할 수 있게 된다. 
	// 여기서 그 '처리'를 테스트한다. 자세한 설명은 해당 함수 내부의 주석을 참고.

	// 우선 테스트할 state를 생성
	ZFSMState* pState = new ZFSMState;
	// 펑션리스트에 "dice" 펑션이 있다 (랜덤값을 생성하는 부분)
	{
		vector<ZFSMFunctionCall> vecFuncCall;
		ZFSMFunctionCall fc;
		fc.Clear();
		fc.id = ZFSMFunctionServer::FSMFUNC_DICE;
		vecFuncCall.push_back(fc);
		pState->SetFunctionCalls(vecFuncCall);
	}
	// 전이조건에도 dice가 있다 (랜덤값을 사용하는 부분)
	{
		ZFSMTransition* pTrans;

		pTrans = new ZFSMTransition;
		{
			ZFSMCondition* pCond = new ZFSMCondition;
			pCond->Init(FSMCOND_HAS_TARGET);
			pTrans->AddCondition(pCond);
			pTrans->SetNextStateName("combat");
		}
		pState->AddTransition(pTrans);

		pTrans = new ZFSMTransition;
		{
			ZFSMCondition* pCond = new ZFSMCondition;
			pCond->Init(FSMCOND_DICE, 10);
			pTrans->AddCondition(pCond);
			pTrans->SetNextStateName("idle");
		}
		pState->AddTransition(pTrans);

		pTrans = new ZFSMTransition;
		{
			ZFSMCondition* pCond = new ZFSMCondition;
			pCond->Init(FSMCOND_DICE, 10);
			pTrans->AddCondition(pCond);
			pTrans->SetNextStateName("dance");
		}
		pState->AddTransition(pTrans);

		pTrans = new ZFSMTransition;
		{
			ZFSMCondition* pCond = new ZFSMCondition;
			pCond->Init(FSMCOND_DICE, 20);
			pTrans->AddCondition(pCond);
			pTrans->SetNextStateName("have_a_meal");
		}
		pState->AddTransition(pTrans);

		
	}
	// 위의 state 전이조건을 말로 풀어보면 아래와 같다
	// 1. 적이 있는가? -> combat
	// 2. 주사위값이 처음 10의 범위 내 -> idle
	// 3. 주사위값이 그다음 10의 범위 내 -> dance
	// 4. 주사위값이 그다음 20의 범위 내 -> have_a_meal
	// 주사위값은 미리 state가 이번 프레임에 구해놓은 랜덤값으로 사용한다.

	// 따라서 주사위 값을 매프레임 구하기 위해서는 미리 dice 조건에 붙은 확률치의 총합을 구해두어야 한다.

	// xml에서 읽은 dice 펑션엔 인자가 지정되어 있지 않다
	const ZFSMFunctionCall* pFuncCall =  pState->GetFunctionCall(0);
	ASSERT_EQ(ZFSMFunctionServer::FSMFUNC_DICE, pFuncCall->id);
	ASSERT_EQ(FSMMETHOD_ARG_NONE, pFuncCall->nArg[0]);
	// state에게 자신이 가진 dice조건을 모두 찾아서 그 인자의 총합을 구해 dice펑션의 인자를 수정하도록 한다
	pState->MakeDiceFuncHasMaxValue();
	// 이제 dice펑션은 1개의 인자를 가지게 되었다
	EXPECT_EQ(10+10+20, pFuncCall->nArg[0]);

	// dice조건들이 확률양이 아니라 확률범위를 가지도록 인자를 수정한다
	pState->MakeDiceConditionHasRange();
	// 인자들이 순차적인 범위로 바뀌었음을 확인할 수 있다
	ZFSMTransition* pDiceTrans;
	pDiceTrans = pState->GetTransition(1);	// idle transition
	EXPECT_EQ(0,  pDiceTrans->GetCondition(0)->GetArg(0));
	EXPECT_EQ(10, pDiceTrans->GetCondition(0)->GetArg(1));
	pDiceTrans = pState->GetTransition(2);	// dance transition
	EXPECT_EQ(10, pDiceTrans->GetCondition(0)->GetArg(0));
	EXPECT_EQ(20, pDiceTrans->GetCondition(0)->GetArg(1));
	pDiceTrans = pState->GetTransition(3);	// have_a_meal transition
	EXPECT_EQ(20, pDiceTrans->GetCondition(0)->GetArg(0));
	EXPECT_EQ(40, pDiceTrans->GetCondition(0)->GetArg(1));

	delete pState;
}


#endif
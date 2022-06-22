#include "stdafx.h"

#ifdef _TDD		// 모든 단위 테스트 관련 코드는 _TDD로 감싸도록 합니다
#include <gtest/gtest.h>
#include "ZFSM.h"
#include "ZFSMState.h"
#include "ZFSMTransition.h"


TEST(ZFSM, HasName)
{
	ZFSM* pFSM = new ZFSM;
	pFSM->SetName("knifeman");
	EXPECT_STREQ("knifeman", pFSM->GetName());
	delete pFSM;
}

TEST(ZFSM, HasEntryStateName)
{
	ZFSM* pFSM = new ZFSM;
	pFSM->SetEntryStateName("idle");
	EXPECT_STREQ("idle", pFSM->GetEntryStateName());
	delete pFSM;
}

TEST(ZFSM, HasStates)
{
	ZFSMState* pState1 = new ZFSMState;
	ZFSMState* pState2 = new ZFSMState;
	
	pState1->SetName("state1");
	pState2->SetName("state2");

	ZFSM* pFSM = new ZFSM;
	pFSM->AddState(pState1);
	pFSM->AddState(pState2);

	EXPECT_EQ(pState1, pFSM->GetState(pState1->GetName()));
	EXPECT_EQ(pState2, pFSM->GetState(pState2->GetName()));

	delete pFSM;
	// FSM에 주어진 state객체는 FSM이 스스로 해제한다
}

TEST(ZFSM, PrepareTransNextState)
{
	ZFSM* pFSM = new ZFSM;
	pFSM->SetName("knifeman");
	
	ZFSMState* pState1 = new ZFSMState;
	ZFSMState* pState2 = new ZFSMState;

	pState1->SetName("state1");
	pState2->SetName("state2");

	ZFSMTransition* pTrans1 = new ZFSMTransition;
	pTrans1->SetNextStateName("state2");
	pState1->AddTransition(pTrans1);

	ZFSMTransition* pTrans2 = new ZFSMTransition;
	pTrans2->SetNextStateName("state1");
	pState2->AddTransition(pTrans2);

	pFSM->AddState(pState1);
	pFSM->AddState(pState2);

	// 이 예에서, FSM은 state 2개를 갖고 있고 각 state는 서로가 next state이다
	// xml에서 FSM을 로딩한 직후에는 next state의 이름만 알고 있는 상태이다
	// ZFSM은 각 transition이 갖고 있는 next state의 이름을 실제 state의 포인터로 변환해 주어야 한다

	EXPECT_TRUE(NULL == pTrans2->GetNextState());
	EXPECT_TRUE(NULL == pTrans1->GetNextState());

	pFSM->PrepareTransNextState();

	EXPECT_EQ(pState1, pTrans2->GetNextState());
	EXPECT_EQ(pState2, pTrans1->GetNextState());

	delete pFSM;
}


/*
class ZActorActionMock : public IActorAction
{
public:
	
};

TEST(ZFSM, UpdateFrame)
{
	ZFSMCondition* pCondHpLessThan0 = new ZFSMCondition;
	pCondHpLessThan->Init(FSMCOND_HP_LESS, 0);
	ZFSMTransition* pTrans_HpLessThan0_Die = new ZFSMTransition;
	pTrans_HpLessThan0_Die->AddCondition(pCondHpLessThan0);
	pTrans_HpLessThan0_Die->SetNextState("")

	ZFSMState* pState;
	pState = new ZFSMState;
	pState->SetName("findTarget");
	pState->SetCooltime(0);


	ZFSM* pFSM = new ZFSM;
	pFSM->SetName("testFSM");
	pFSM->SetEntryStateName("findTarget");
}
*/

#endif
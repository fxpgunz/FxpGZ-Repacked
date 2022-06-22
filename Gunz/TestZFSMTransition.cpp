#include "stdafx.h"

#ifdef _TDD		// 모든 단위 테스트 관련 코드는 _TDD로 감싸도록 합니다
#include <gtest/gtest.h>
#include "ZFSMTransition.h"
#include "ZFSMConditionSourceMock.h"
#include "ZFSMState.h"


TEST(ZFSMTransition, CheckConditions)
{
	ZFSMState* pState = new ZFSMState;
	pState->SetName("someState");

	ZFSMConditionSourceMock condSrc;

	ZFSMCondition* pCondHpLess = new ZFSMCondition;
	pCondHpLess->Init(FSMCOND_HP_LESS, 15);
	ZFSMCondition* pCondGroggy = new ZFSMCondition;
	pCondGroggy->Init(FSMCOND_GROGGY_GREATER, 50);

	ZFSMTransition trans;
	trans.AddCondition(pCondHpLess);
	trans.AddCondition(pCondGroggy);
	trans.SetNextState(pState);

	// 조건 모두 만족할때
	condSrc.m_fHp = 1;
	condSrc.m_fGroggy = 99;
	EXPECT_TRUE(trans.Check(&condSrc));

	// 조건 일부만 만족할때
	condSrc.m_fHp = 1;
	condSrc.m_fGroggy = 0;
	EXPECT_FALSE(trans.Check(&condSrc));

	// 조건 모두 만족하지 못할때
	condSrc.m_fHp = 100;
	condSrc.m_fGroggy = 0;
	EXPECT_FALSE(trans.Check(&condSrc));

	delete pState;
	// 추가된 조건객체는 ZFSMTransition이 알아서 소멸시킴
}

TEST(ZFSMTransition, WhenHasNoConditions)
{
	ZFSMConditionSourceMock condSrc;

	ZFSMState* pState = new ZFSMState;
	pState->SetName("someState");

	ZFSMTransition trans;
	trans.SetNextState(pState);
	EXPECT_TRUE(trans.Check(&condSrc));	// 조건이 하나도 추가되어 있지 않으면 검사결과는 항상 만족이다

	delete pState;
}

TEST(ZFSMTransition, NextState)
{
	ZFSMTransition trans;

	// 처음에는 전이할 상태가 지정되어 있지 않다
	EXPECT_EQ(NULL, trans.GetNextState());

	// 외부에 존재하는 어떤 상태가 조건 만족시 전이할 상태라는 것을 알려줌
	ZFSMState* pSomeState = new ZFSMState;
	trans.SetNextState(pSomeState);
	EXPECT_EQ(pSomeState, trans.GetNextState());

	delete pSomeState;
}

TEST(ZFSMTransition, CheckCondition)
{
	ZFSMState* pState = new ZFSMState;
	pState->SetCooltime(100);
	pState->SetName("someState");

	// 항상 true를 반환하는 조건을 가진 전이객체
	ZFSMTransition trans;
	ZFSMCondition* pCond = new ZFSMCondition;
	pCond->Init(FSMCOND_DEFAULT);
	trans.AddCondition(pCond);
	trans.SetNextState(pState);

	ZFSMConditionSourceMock condSrc;
	EXPECT_TRUE(trans.Check(&condSrc));

	delete pState;
}

#endif
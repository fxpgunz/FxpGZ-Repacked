#include "stdafx.h"

#ifdef _TDD		// 모든 단위 테스트 관련 코드는 _TDD로 감싸도록 합니다
#include <gtest/gtest.h>
#include "ZFSMCondition.h"
#include "ZFSMConditionSourceMock.h"


TEST(ZFSMCondition, TestAccessor)
{
	ZFSMCondition cond;
	cond.Init(FSMCOND_HP_LESS, 5);
	EXPECT_EQ(FSMCOND_HP_LESS, cond.GetConditionId());
	EXPECT_EQ(5, cond.GetArg(0));

	// 여기서 크래쉬가 나면 안됩니다.
	cond.GetArg(100);
	cond.GetArg(-1);
}

TEST(ZFSMCondition, Default)
{
	ZFSMConditionSourceMock condSrc;

	ZFSMCondition* pCond = new ZFSMCondition;
	pCond->Init(FSMCOND_DEFAULT);
	EXPECT_TRUE(pCond->Check(&condSrc));	// 항상 true
	delete pCond;
}

TEST(ZFSMCondition, Dice)
{
	// dice는 액터 내부에서 마지막으로 구한 랜덤값을 확인해서 그것을 기준으로 참/거짓을 정한다
	ZFSMConditionSourceMock condSrc;
	
	ZFSMCondition* pCond = new ZFSMCondition;
	pCond->Init(FSMCOND_DICE, 10, 20);

	condSrc.m_nDice = 10;
	EXPECT_TRUE(pCond->Check(&condSrc));
	condSrc.m_nDice = 15;
	EXPECT_TRUE(pCond->Check(&condSrc));
	condSrc.m_nDice = 20;
	EXPECT_FALSE(pCond->Check(&condSrc));
	condSrc.m_nDice = 70;
	EXPECT_FALSE(pCond->Check(&condSrc));

	delete pCond;
}

TEST(ZFSMCondition, HpLessEqual)
{
	ZFSMConditionSourceMock condSrc;

	ZFSMCondition* pCond = new ZFSMCondition;
	pCond->Init(FSMCOND_HP_EQUAL, 0);

	condSrc.m_fHp = 0.f;
	EXPECT_TRUE(pCond->Check(&condSrc));
	condSrc.m_fHp = 1.f;
	EXPECT_FALSE(pCond->Check(&condSrc));
	condSrc.m_fHp = -1.f;
	EXPECT_FALSE(pCond->Check(&condSrc));

	delete pCond;
}

TEST(ZFSMCondition, HpLessThan)
{
	ZFSMConditionSourceMock condSrc;

	ZFSMCondition* pCond = new ZFSMCondition;
	pCond->Init(FSMCOND_HP_LESS, 10);

	condSrc.m_fHp = 8.f;
	EXPECT_TRUE(pCond->Check(&condSrc));
	condSrc.m_fHp = 11.f;
	EXPECT_FALSE(pCond->Check(&condSrc));
	condSrc.m_fHp = 10.f;
	EXPECT_FALSE(pCond->Check(&condSrc));		// '이하'가 아닌 '미만'이어야 만족

	delete pCond;
}

TEST(ZFSMCondition, HpGreaterThan)
{
	ZFSMConditionSourceMock condSrc;

	ZFSMCondition* pCond = new ZFSMCondition;
	pCond->Init(FSMCOND_HP_GREATER, 0);

	condSrc.m_fHp = 0.1f;
	EXPECT_TRUE(pCond->Check(&condSrc));
	condSrc.m_fHp = -0.1f;
	EXPECT_FALSE(pCond->Check(&condSrc));
	condSrc.m_fHp = 0.f;
	EXPECT_FALSE(pCond->Check(&condSrc));		// '이상'이 아닌 '초과'이어야 만족

	delete pCond;
}

TEST(ZFSMCondition, GroggyGreaterThan)
{
	ZFSMConditionSourceMock condSrc;

	ZFSMCondition* pCond = new ZFSMCondition;
	pCond->Init(FSMCOND_GROGGY_GREATER, 30);

	condSrc.m_fGroggy = 35;
	EXPECT_TRUE(pCond->Check(&condSrc));
	condSrc.m_fGroggy = 10;
	EXPECT_FALSE(pCond->Check(&condSrc));
	condSrc.m_fGroggy = 30;
	EXPECT_FALSE(pCond->Check(&condSrc));		// '이상'이 아닌 '초과'이어야 만족

	delete pCond;
}

TEST(ZFSMCondition, VelocityZLessThan)
{
	ZFSMConditionSourceMock condSrc;

	ZFSMCondition* pCond = new ZFSMCondition;
	pCond->Init(FSMCOND_VELOCITY_Z_LESS, 0);

	condSrc.m_vVelocity = rvector(0, 0, -1);
	EXPECT_TRUE(pCond->Check(&condSrc));
	condSrc.m_vVelocity = rvector(0, 0, 1);
	EXPECT_FALSE(pCond->Check(&condSrc));
	condSrc.m_vVelocity = rvector(0, 0, 0);
	EXPECT_FALSE(pCond->Check(&condSrc));		// '이하'가 아닌 '미만'이어야 만족

	delete pCond;
}

TEST(ZFSMCondition, VelocityMagnitudeLessThan)
{
	ZFSMConditionSourceMock condSrc;

	ZFSMCondition* pCond = new ZFSMCondition;
	pCond->Init(FSMCOND_VELOCITY_MAGNITUDE_LESS, 16);

	condSrc.m_vVelocity = rvector(0, 0, 10);
	EXPECT_TRUE(pCond->Check(&condSrc));
	condSrc.m_vVelocity = rvector(0, 0, 36);
	EXPECT_FALSE(pCond->Check(&condSrc));
	condSrc.m_vVelocity = rvector(0, 0, 16);
	EXPECT_FALSE(pCond->Check(&condSrc));		// '이하'가 아닌 '미만'이어야 만족

	delete pCond;
}

TEST(ZFSMCondition, EndOfAction)
{
	ZFSMConditionSourceMock condSrc;

	ZFSMCondition* pCond = new ZFSMCondition;
	pCond->Init(FSMCOND_END_ACTION);

	condSrc.m_bEndAction = true;
	EXPECT_TRUE(pCond->Check(&condSrc));

	condSrc.m_bEndAction = false;
	EXPECT_FALSE(pCond->Check(&condSrc));

	delete pCond;
}

TEST(ZFSMCondition, IsLanding)
{
	ZFSMConditionSourceMock condSrc;

	ZFSMCondition* pCond = new ZFSMCondition;
	pCond->Init(FSMCOND_IS_LANDING);

	condSrc.m_bLanding = true;
	EXPECT_TRUE(pCond->Check(&condSrc));

	condSrc.m_bLanding = false;
	EXPECT_FALSE(pCond->Check(&condSrc));

	delete pCond;
}

TEST(ZFSMCondition, HasTarget_and_HasNoTarget)
{
	ZFSMConditionSourceMock condSrc;

	ZFSMCondition* pCond = new ZFSMCondition;
	pCond->Init(FSMCOND_HAS_TARGET);

	condSrc.m_bHasTarget = true;
	EXPECT_TRUE(pCond->Check(&condSrc));
	condSrc.m_bHasTarget = false;
	EXPECT_FALSE(pCond->Check(&condSrc));

	pCond->Init(FSMCOND_HAS_NO_TARGET);

	condSrc.m_bHasTarget = false;
	EXPECT_TRUE(pCond->Check(&condSrc));
	condSrc.m_bHasTarget = true;
	EXPECT_FALSE(pCond->Check(&condSrc));

	delete pCond;
}

TEST(ZFSMCondition, DistanceToTargetLessThan)
{
	ZFSMConditionSourceMock condSrc;

	ZFSMCondition* pCond = new ZFSMCondition;
	pCond->Init(FSMCOND_DIST_TARGET_IN, 30, 80);

	condSrc.m_vMyPos = rvector(0, 0, 0);
	condSrc.m_bHasTarget = true;

	condSrc.m_vTargetPos = rvector(50, 0, 0);
	EXPECT_TRUE(pCond->Check(&condSrc));
	condSrc.m_vTargetPos = rvector(10, 0, 0);
	EXPECT_FALSE(pCond->Check(&condSrc));
	condSrc.m_vTargetPos = rvector(100, 0, 0);
	EXPECT_FALSE(pCond->Check(&condSrc));

	// 타겟을 갖고 있지 않을때 거리 조건체크가 통과해선 안된다
	pCond->Init(FSMCOND_DIST_TARGET_IN, 0, 100);
	condSrc.m_bHasTarget = false;
	condSrc.m_vMyPos = rvector(0, 0, 0);
	condSrc.m_vTargetPos = rvector(10, 0, 0);
	EXPECT_FALSE(pCond->Check(&condSrc));

	delete pCond;
}

TEST(ZFSMCondition, LookAtTargetInDegree)
{
	ZFSMConditionSourceMock condSrc;

	ZFSMCondition* pCond = new ZFSMCondition;
	pCond->Init(FSMCOND_LOOK_AT_TARGET, 90);	// 좌우 90도 즉 총 180도 내에 있으면 통과가 된다

	// 내가 보는 방향으로부터 지정된 각도 내에 적이 있는가
	condSrc.m_bHasTarget = true;
	condSrc.m_vMyPos = rvector(0, 0, 0);
	condSrc.m_vMyDir = rvector(1, 0, 0);
	condSrc.m_vTargetPos = rvector(50, 0, 0);
	EXPECT_TRUE(pCond->Check(&condSrc));

	// 실패 - 완전 등뒤에 있다
	condSrc.m_vTargetPos = rvector(-50, 0, 0);
	EXPECT_FALSE(pCond->Check(&condSrc));

	// 성공 - 대각선 방향
	condSrc.m_vTargetPos = rvector(30, 30, 0);
	EXPECT_TRUE(pCond->Check(&condSrc));

	// 실패 - 뒤쪽 대각선 방향
	condSrc.m_vTargetPos = rvector(-1, 50, 0);
	EXPECT_FALSE(pCond->Check(&condSrc));

	// 타겟을 갖고 있지 않을때 체크가 통과해선 안된다

	delete pCond;
}

TEST(ZFSMCondition, IsEmptySpace)
{
	ZFSMConditionSourceMock condSrc;

	ZFSMCondition* pCond = new ZFSMCondition;
	pCond->Init(FSMCOND_IS_EMPTY_SPACE, 0, 100);	// 각도0(=전방)방향으로 거리100 내에 아무것도 피킹되지 않아야 조건 통과

	// 이것은 테스트를 어찌만들어야 할지..;

	delete pCond;
}

TEST(ZFSMCondition, TimeElapsedSinceEntered)
{
	//todok TimeElapsedSinceEntered 테스트 작성
}

TEST(ZFSMCondition, EndLightningDamage)
{
	ZFSMConditionSourceMock condSrc;

	ZFSMCondition* pCond = new ZFSMCondition;
	pCond->Init(FSMCOND_END_LIGHTNINGDAMAGE);

	// 라이트닝대미지 모듈이 활성화된 상태면 안끝난 것으로 판정
	condSrc.m_bActiveLightningDamage = true;
	EXPECT_FALSE(pCond->Check(&condSrc));

	condSrc.m_bActiveLightningDamage = false;
	EXPECT_TRUE(pCond->Check(&condSrc));

	delete pCond;
}

#endif
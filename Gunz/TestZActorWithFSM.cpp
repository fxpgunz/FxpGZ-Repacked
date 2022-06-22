#include "stdafx.h"

#ifdef _TDD		// 모든 단위 테스트 관련 코드는 _TDD로 감싸도록 합니다
#include <gtest/gtest.h>
#include "ZGameMock.h"
#include "ZActorActionManager.h"
#include "ZActorWithFSM.h"

TEST(ZActorWithFSM, IsNpc)
{
	ZActorActionManager actionMgr;
	ZGameMock gameMock;
	ZActorWithFSM actor(&gameMock, &actionMgr);
	EXPECT_TRUE(actor.IsNPC());
}

TEST(ZActorWithFSM, FindTarget)
{
	ZActorActionManager actionMgr;
	ZGameMock gameMock;

	// 액터가 하나 있다
	ZActorWithFSM actor(&gameMock, &actionMgr);
	actor.SetPosition(rvector(100,0,0));

	// 처음엔 타겟을 갖고 있지 않다
	EXPECT_FALSE(actor.Cond_HasTarget());

	// 월드에 아무도 없기 때문에 타겟을 찾을 수도 없다
	actor.Func_FindTarget();
	EXPECT_FALSE(actor.Cond_HasTarget());

	// 이제 월드에 캐릭터가 하나 있다고 하자
	ZCharacterObject character(&gameMock);
	character.SetUID(MUID(0,1));
	character.SetPosition(rvector(0,0,0));
	gameMock.m_characterManager[MUID(0,1)] = &character;

	// 이제 타겟을 찾을 수 있다
	actor.Func_FindTarget();
	EXPECT_TRUE(actor.Cond_HasTarget());
	EXPECT_TRUE(MUID(0,1) == actor.GetTargetUID());

	// 월드에 캐릭터 하나를 더 넣는다
	ZCharacterObject character2(&gameMock);
	character2.SetUID(MUID(0,2));
	character2.SetPosition(rvector(50,0,0));
	gameMock.m_characterManager[MUID(0,2)] = &character2;

	// 새 캐릭터가 더 가까우므로 이 캐릭터가 선택된다
	actor.Func_FindTarget();
	EXPECT_TRUE(actor.Cond_HasTarget());
	EXPECT_TRUE(MUID(0,2) == actor.GetTargetUID());
}

TEST(ZActorWithFSM, RotateToDirection)
{
	ZActorActionManager actionMgr;
	ZGameMock gameMock;

	float fFrameDelta = 0.01f;

	// 액터가 하나 있다
	ZActorWithFSM actor(&gameMock, &actionMgr);
	actor.SetPosition(rvector(100,0,0));

	rvector vDir = rvector(1,0,0);
	actor.SetDirection(vDir);

	// 회전을 시도하자
	actor.RotateToDirection(rvector(-1,0,0), fFrameDelta);
	rvector vDir2 = actor.GetDirection();

	float fAngle1 = GetAngleOfVectors(vDir, vDir2);

	// 이번엔 프레임이 2배 느리게 돌아갔다
	actor.RotateToDirection(rvector(-1,0,0), fFrameDelta*2);
	rvector vDir3 = actor.GetDirection();
 
	float fAngle2 = GetAngleOfVectors(vDir2, vDir3);

	// 한번의 회전량은 프레임 시간에 비례한다
	EXPECT_NEAR(fAngle2, fAngle1*2, 0.001f);
}


#endif
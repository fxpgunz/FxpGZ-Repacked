#include "stdafx.h"

#ifdef _TDD		// 모든 단위 테스트 관련 코드는 _TDD로 감싸도록 합니다
#include <gtest/gtest.h>
#include "ZFSMFunctionServer.h"

TEST(ZFSMFunctionServer, Dice)
{
	ZFSMFunctionServer funcSvr;

	// dice펑션은 랜덤값을 생성한다
	ASSERT_TRUE(funcSvr.ExecuteFunction(0, ZFSMFunctionServer::FSMFUNC_DICE, 0, 100));
	EXPECT_TRUE(0 <= funcSvr.GetDice() && funcSvr.GetDice() < 100);
}

#endif
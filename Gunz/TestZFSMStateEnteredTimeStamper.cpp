#include "stdafx.h"

#ifdef _TDD		// 모든 단위 테스트 관련 코드는 _TDD로 감싸도록 합니다
#include <gtest/gtest.h>
#include "ZFSMStateEnteredTimeStamper.h"
#include "ZFSMState.h"


TEST(ZFSMStateEnteredTimeStamper, CheckCooltime)
{
	ZFSMStateEnteredTimeStamper stamper;

	DWORD dwCurrTime = 10000;

	// 한번도 진입한적 없는 상태의 쿨타임 체크 요청인 경우 바로 성공
	// (stamper가 "state1"이라는 상태의 진입 시각을 갖고 있지 않을 것이다)
	ZFSMState state1;
	state1.SetName("state1");
	state1.SetCooltime(3000);
	EXPECT_TRUE(stamper.CheckCooltime(&state1, dwCurrTime));
	stamper.SetEnteredTime(&state1, dwCurrTime);

	// 시간이 흘렀다
	dwCurrTime += 2000;

	// 다시 쿨타임 검사를 해보면 아직 쿨타임이 안끝났으므로 이번엔 실패할 것이다
	EXPECT_FALSE(stamper.CheckCooltime(&state1, dwCurrTime));

	// 다시 시간이 흘렀다
	dwCurrTime += 1500;

	// 다시 쿨타임 검사를 해보면 이번엔 성공할 것이다
	EXPECT_TRUE(stamper.CheckCooltime(&state1, dwCurrTime));
	stamper.SetEnteredTime(&state1, dwCurrTime);

	// 또 약간 시간이 흐르고
	dwCurrTime += 1000;
	
	// 쿨타임 검사를 다시 해보면 아까 진입시각이 갱신되었으므로 실패한다
	EXPECT_FALSE(stamper.CheckCooltime(&state1, dwCurrTime));
}

TEST(ZFSMStateEnteredTimeStamper, GetSetEnteredTime)
{
	ZFSMStateEnteredTimeStamper stamper;

	DWORD dwCurrTime = 10000;

	ZFSMState state1;
	stamper.SetEnteredTime(&state1, dwCurrTime);
	EXPECT_EQ(dwCurrTime, stamper.GetEnteredTime(&state1));
}


#endif
#include "stdafx.h"

#ifdef _TDD		// 모든 단위 테스트 관련 코드는 _TDD로 감싸도록 합니다
#include <gtest/gtest.h>

extern bool g_bTddNeverPostCmd;
extern DWORD g_nPseudoTimeForTDD;
extern bool g_bUsePseudoTimeForTDD;

int TestMain(int argc, _TCHAR* argv[])
{
	g_bTddNeverPostCmd = true;
	g_bUsePseudoTimeForTDD = true;

	OutputDebugString("\n!!-- Unit Test <begin> ------------------------\n");

	testing::InitGoogleTest(&argc, argv);
	int result =  RUN_ALL_TESTS();

	if (result==0)
		OutputDebugString("\nAll tests passed. \n\n");

	OutputDebugString("!!-- Unit Test <end> --------------------------\n\n");

	g_bTddNeverPostCmd = false;
	g_bUsePseudoTimeForTDD = false;

	

	return result;
}

TEST(TestTest, TestTest)
{
	ASSERT_EQ(1+1, 2);
	ASSERT_EQ(pow(2, 10), 1024);

	// fail test
	//ASSERT_EQ(1, 2);
}

#endif
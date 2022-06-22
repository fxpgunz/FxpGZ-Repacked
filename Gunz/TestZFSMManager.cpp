#include "stdafx.h"

#ifdef _TDD		// 모든 단위 테스트 관련 코드는 _TDD로 감싸도록 합니다
#include <gtest/gtest.h>
#include "ZFSMManager.h"
#include "ZFSM.h"

TEST(ZFSMManager, AddFsms_and_find)
{
	vector<ZFSM*> vec;
	ZFSM* pFSM;
	pFSM = new ZFSM;	pFSM->SetName("fsm1");	vec.push_back(pFSM);
	pFSM = new ZFSM;	pFSM->SetName("fsm2");	vec.push_back(pFSM);
	pFSM = new ZFSM;	pFSM->SetName("fsm3");	vec.push_back(pFSM);

	ZFSMManager mgr;
	mgr.AddFsm(&vec[0], (unsigned int)vec.size());
	EXPECT_EQ(3, mgr.GetFsmCount());

	// find
	pFSM = mgr.GetFsm("fsm1");
	EXPECT_STREQ("fsm1", pFSM->GetName());

	// 존재하지 않는 것 찾으면 null
	EXPECT_TRUE(NULL == mgr.GetFsm("invalidname"));
}

#endif
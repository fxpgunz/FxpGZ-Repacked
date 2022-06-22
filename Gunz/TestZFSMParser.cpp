#include "stdafx.h"

#ifdef _TDD		// 모든 단위 테스트 관련 코드는 _TDD로 감싸도록 합니다
#include <gtest/gtest.h>
#include "ZFSMParser.h"
#include "ZFSMCondition.h"
#include "ZFSMTransition.h"
#include "ZFSMFunctionServer.h"
#include "ZFSM.h"
#include "ZFSMState.h"
#include "ZActorAction.h"
#include "ZActorActionManager.h"
#include "ZActorActionManagerMock.h"


TEST(ZFSMParser, ParseMethodStatement)
{
	ZFSMParser parser;
	ZFSMMethodCall mc;
	
	// 메소드이름만으로 된 문장 파싱
	parser.ParseMethodStatement(&mc, "findTarget");
	EXPECT_STREQ("findTarget", mc.strMethodName.c_str());
	EXPECT_EQ(FSMMETHOD_ARG_NONE, mc.nArg[0]);
	
	// 메소드이름:인자 1개인 문장을 파싱
	parser.ParseMethodStatement(&mc, "hpLess:10");
	EXPECT_STREQ("hpLess", mc.strMethodName.c_str());
	EXPECT_EQ(10, mc.nArg[0]);
	EXPECT_EQ(FSMMETHOD_ARG_NONE, mc.nArg[1]);

	// 인자가 많은 메소드 호출 문장
	parser.ParseMethodStatement(&mc, "distTarget:100;250");
	EXPECT_STREQ("distTarget", mc.strMethodName.c_str());
	EXPECT_EQ(100, mc.nArg[0]);
	EXPECT_EQ(250, mc.nArg[1]);
	EXPECT_EQ(FSMMETHOD_ARG_NONE, mc.nArg[2]);

	// 사이사이에 공백문자를 넣었을 경우
	parser.ParseMethodStatement(&mc, " \t foo: \t333; 555  \t ");
	EXPECT_STREQ("foo", mc.strMethodName.c_str());
	EXPECT_EQ(333, mc.nArg[0]);
	EXPECT_EQ(555, mc.nArg[1]);
	EXPECT_EQ(FSMMETHOD_ARG_NONE, mc.nArg[2]);
}

TEST(ZFSMParser, DivideMultipleMethodStatement)
{
	// ','로 여러 메소드 호출을 연결한 문장을 파싱

	ZFSMParser parser;
	vector<string> v;

	// 두개의 호출이 담긴 문장
	parser.DevideMultipleMethodStatement(v, "dice:5,distTarget:7;9");
	EXPECT_EQ(2, (int)v.size());

	EXPECT_STREQ("dice:5",			v[0].c_str());
	EXPECT_STREQ("distTarget:7;9",	v[1].c_str());

	// 한개의 호출이 담겨있어도 잘 읽는다
	parser.DevideMultipleMethodStatement(v, "hpLess:15");
	EXPECT_EQ(1, (int)v.size());

	EXPECT_STREQ("hpLess:15", v[0].c_str());
}

// 펑션 실행문 1개를 파싱한다
TEST(ZFSMParser, ParseSingleFunctionStatement)
{
	ZFSMParser parser;
	ZFSMFunctionCall funcCall;

	// 실패
	EXPECT_FALSE(parser.ParseFunction(NULL, "dice"));			// condition 객체 없을때
	EXPECT_FALSE(parser.ParseFunction(&funcCall, "xxxxx"));		// 알수없는 문자열

	// 빈문자열도 에러가 아님
	EXPECT_TRUE(parser.ParseFunction(&funcCall, ""));
	EXPECT_EQ(ZFSMFunctionServer::FSMFUNC_DONOTHING, funcCall.id);

	EXPECT_TRUE(parser.ParseFunction(&funcCall, "dice"));
	EXPECT_EQ(ZFSMFunctionServer::FSMFUNC_DICE, funcCall.id);

	EXPECT_TRUE(parser.ParseFunction(&funcCall, "findTarget"));
	EXPECT_EQ(ZFSMFunctionServer::FSMFUNC_FIND_TARGET, funcCall.id);

	EXPECT_TRUE(parser.ParseFunction(&funcCall, "setTargetLastAttacker"));
	EXPECT_EQ(ZFSMFunctionServer::FSMFUNC_SET_TARGET_LAST_ATTACKER, funcCall.id);

	EXPECT_TRUE(parser.ParseFunction(&funcCall, "rotateToTarget"));
	EXPECT_EQ(ZFSMFunctionServer::FSMFUNC_ROTATE_TO_TARGET, funcCall.id);

	EXPECT_TRUE(parser.ParseFunction(&funcCall, "faceToTarget"));
	EXPECT_EQ(ZFSMFunctionServer::FSMFUNC_FACE_TO_TARGET, funcCall.id);

	EXPECT_TRUE(parser.ParseFunction(&funcCall, "reduceGroggy:10"));
	EXPECT_EQ(ZFSMFunctionServer::FSMFUNC_REDUCE_GROGGY, funcCall.id);
	EXPECT_EQ(10, funcCall.nArg[0]);

	EXPECT_TRUE(parser.ParseFunction(&funcCall, "buildWaypointsToTarget"));
	EXPECT_EQ(ZFSMFunctionServer::FSMFUNC_BUILD_WAYPOINTS_TO_TARGET, funcCall.id);

	EXPECT_TRUE(parser.ParseFunction(&funcCall, "runWaypoints"));
	EXPECT_EQ(ZFSMFunctionServer::FSMFUNC_RUN_WAYPOINTS, funcCall.id);
}

// 여러개로 이루어진 펑션 실행문을 파싱한다
TEST(ZFSMParser, ParseFunctionsStatement)
{
	ZFSMParser parser;
	vector<ZFSMFunctionCall> v;

	// 실패하는 경우
	EXPECT_FALSE(parser.ParseFunctionsStatement(v, "dice,xxxx,findTarget"));		// 알수없는 문자열이 포함됨
	ASSERT_TRUE(v.empty());		// 실패하면 빈 배열이 돌려진다

	// 성공
	EXPECT_TRUE(parser.ParseFunctionsStatement(v, "dice,reduceGroggy:33,rotateToTarget"));
	ASSERT_EQ(3, (int)v.size());
	EXPECT_EQ(ZFSMFunctionServer::FSMFUNC_DICE, v[0].id);
	EXPECT_EQ(ZFSMFunctionServer::FSMFUNC_REDUCE_GROGGY, v[1].id);
	EXPECT_EQ(33, v[1].nArg[0]);
	EXPECT_EQ(ZFSMFunctionServer::FSMFUNC_ROTATE_TO_TARGET, v[2].id);
	v.clear();

	// 펑션 실행문이 1개짜리여도 잘 된다
	EXPECT_TRUE(parser.ParseFunctionsStatement(v, "dice"));
	EXPECT_EQ(1, (int)v.size());
	EXPECT_EQ(ZFSMFunctionServer::FSMFUNC_DICE, v[0].id);
	v.clear();

	// 펑션 실행문이 비어있어도 에러가 아니다
	EXPECT_TRUE(parser.ParseFunctionsStatement(v, ""));
	EXPECT_EQ(1, (int)v.size());
	EXPECT_EQ(ZFSMFunctionServer::FSMFUNC_DONOTHING, v[0].id);
}

TEST(ZFSMParser, ParseConditionStatement)
{
	ZFSMParser parser;
	ZFSMCondition cond;

	// 실패하는 경우
	EXPECT_FALSE(parser.ParseCondition(NULL, "HpLess:10"));			// condition 객체 없을때
	EXPECT_FALSE(parser.ParseCondition(&cond, "zmxncbzbnz"));		// 알수없는 문자열
	EXPECT_FALSE(parser.ParseCondition(&cond, ""));					// 빈문자열

	// default
	EXPECT_TRUE(parser.ParseCondition(&cond, "Default"));
	EXPECT_EQ(FSMCOND_DEFAULT, cond.GetConditionId());

	// dice
	EXPECT_TRUE(parser.ParseCondition(&cond, "Dice:7"));
	EXPECT_EQ(FSMCOND_DICE, cond.GetConditionId());
	EXPECT_EQ(7, cond.GetArg(0));

	// hp equal
	EXPECT_TRUE(parser.ParseCondition(&cond, "HpEqual:11"));
	EXPECT_EQ(FSMCOND_HP_EQUAL, cond.GetConditionId());
	EXPECT_EQ(11, cond.GetArg(0));

	// hp less than
	EXPECT_TRUE(parser.ParseCondition(&cond, "HpLess:10"));
	EXPECT_EQ(FSMCOND_HP_LESS, cond.GetConditionId());
	EXPECT_EQ(10, cond.GetArg(0));

	// hp greater than
	EXPECT_TRUE(parser.ParseCondition(&cond, "HpGreater:50"));
	EXPECT_EQ(FSMCOND_HP_GREATER, cond.GetConditionId());
	EXPECT_EQ(50, cond.GetArg(0));

	// groggy greater than
	EXPECT_TRUE(parser.ParseCondition(&cond, "groggyGreater:4"));
	EXPECT_EQ(FSMCOND_GROGGY_GREATER, cond.GetConditionId());
	EXPECT_EQ(4, cond.GetArg(0));

	// velocity z less than
	EXPECT_TRUE(parser.ParseCondition(&cond, "velocityZLess:-10"));
	EXPECT_EQ(FSMCOND_VELOCITY_Z_LESS, cond.GetConditionId());
	EXPECT_EQ(-10, cond.GetArg(0));
	
	// velocity magnitude less than
	EXPECT_TRUE(parser.ParseCondition(&cond, "velocityMagnitudeLess:20"));
	EXPECT_EQ(FSMCOND_VELOCITY_MAGNITUDE_LESS, cond.GetConditionId());
	EXPECT_EQ(20, cond.GetArg(0));

	// is end of action
	EXPECT_TRUE(parser.ParseCondition(&cond, "endAction"));
	EXPECT_EQ(FSMCOND_END_ACTION, cond.GetConditionId());

	// is landing
	EXPECT_TRUE(parser.ParseCondition(&cond, "isLanding"));
	EXPECT_EQ(FSMCOND_IS_LANDING, cond.GetConditionId());

	// has target
	EXPECT_TRUE(parser.ParseCondition(&cond, "hasTarget"));
	EXPECT_EQ(FSMCOND_HAS_TARGET, cond.GetConditionId());

	// has no target
	EXPECT_TRUE(parser.ParseCondition(&cond, "hasNoTarget"));
	EXPECT_EQ(FSMCOND_HAS_NO_TARGET, cond.GetConditionId());

	// distance to target
	EXPECT_TRUE(parser.ParseCondition(&cond, "distTarget:20;99"));
	EXPECT_EQ(FSMCOND_DIST_TARGET_IN, cond.GetConditionId());
	EXPECT_EQ(20, cond.GetArg(0));
	EXPECT_EQ(99, cond.GetArg(1));

	// look at target in degree
	EXPECT_TRUE(parser.ParseCondition(&cond, "lookAtTarget:20"));
	EXPECT_EQ(FSMCOND_LOOK_AT_TARGET, cond.GetConditionId());
	EXPECT_EQ(20, cond.GetArg(0));

	// is empty space
	EXPECT_TRUE(parser.ParseCondition(&cond, "isEmptySpace:30;100"));
	EXPECT_EQ(FSMCOND_IS_EMPTY_SPACE, cond.GetConditionId());
	EXPECT_EQ(30, cond.GetArg(0));
	EXPECT_EQ(100, cond.GetArg(1));

	// time elapsed since entered into current state
	EXPECT_TRUE(parser.ParseCondition(&cond, "TimeElapsedSinceEntered:1000"));
	EXPECT_EQ(FSMCOND_TIME_ELAPSED_SINCE_ENTERED, cond.GetConditionId());
	EXPECT_EQ(1000, cond.GetArg(0));

	// is end of lightning damage
	EXPECT_TRUE(parser.ParseCondition(&cond, "EndLightningDamage"));
	EXPECT_EQ(FSMCOND_END_LIGHTNINGDAMAGE, cond.GetConditionId());
}

TEST(ZFSMParser, ParseTransition)
{
	ZFSMParser parser;

	ZFSMTransition* pTrans = parser.ParseTransition("hasTarget,distTarget:7;15,HpLess:10", "rotateToTarget");
	EXPECT_TRUE(NULL != pTrans);
	EXPECT_EQ(3, pTrans->CountCondition());
	EXPECT_EQ(FSMCOND_HAS_TARGET,		pTrans->GetCondition(0)->GetConditionId());
	EXPECT_EQ(FSMCOND_DIST_TARGET_IN,	pTrans->GetCondition(1)->GetConditionId());
	EXPECT_EQ(7,						pTrans->GetCondition(1)->GetArg(0));
	EXPECT_EQ(15,						pTrans->GetCondition(1)->GetArg(1));
	EXPECT_EQ(FSMCOND_HP_LESS,			pTrans->GetCondition(2)->GetConditionId());
	EXPECT_EQ(10,						pTrans->GetCondition(2)->GetArg(0));
	EXPECT_STREQ("rotateToTarget", pTrans->GetNextStateName());
	delete pTrans;

	// 실패하는 경우
	EXPECT_TRUE(NULL == parser.ParseTransition("hasTarget,xxxx", "idle"));	// 알수없는 컨디션이름
	EXPECT_TRUE(NULL == parser.ParseTransition("", "idle"));				// 컨디션이름 없음
	EXPECT_TRUE(NULL == parser.ParseTransition("hasTarget", ""));			// next state 이름 없음
}

// 파싱 테스트를 위한 견본 fsm xml 내용
const char g_szFsmXml[] = 
"\
<XML>\
	<FSM name=\"monster1\" entrystate=\"findTarget\">\
	\
		<STATE name=\"idle\" cooltime=\"0\" action=\"idle\" func=\"findTarget\">\
			<TRANS cond=\"default\" next=\"findTarget\"/>\
		</STATE>\
	\
		<STATE name=\"findTarget\" cooltime=\"0\" action=\"idle\" func=\"findTarget\">\
			<TRANS cond=\"hpLess:0\" next=\"die\"/>\
			<TRANS cond=\"hasTarget\" next=\"rotateToTarget\"/>\
		</STATE>\
	\
		<STATE name=\"rotateToTarget\" cooltime=\"0\" action=\"walk\" func=\"rotateToTarget\">\
			<TRANS cond=\"hpLess:0\" next=\"die\"/>\
			<TRANS cond=\"hasNoTarget\" next=\"findTarget\"/>\
			<TRANS cond=\"lookAtTarget:15\" next=\"combat\"/>\
		</STATE>\
	\
		<STATE name=\"combat\" cooltime=\"0\" enterfunc=\"buildWaypointsToTarget\" func=\"dice\">\
			<TRANS cond=\"hpLess:0\" next=\"die\"/>\
			<TRANS cond=\"hasNoTarget\" next=\"findTarget\"/>\
			<TRANS cond=\"dice:20,distTarget:1;3\" next=\"slashTwice\"/>\
			<TRANS cond=\"dice:20,distTarget:10;50\" next=\"sweepFire\"/>\
			<TRANS cond=\"default\" next=\"findTarget\"/>	<!-- default는 위의 조건에 하나도 맞지 않은 경우 -->\
		</STATE>\
	\
		<STATE name=\"slashTwice\" cooltime=\"3000\" action=\"slashTwice\">\
			<TRANS cond=\"hpLess:0\" next=\"die\"/>\
			<TRANS cond=\"endAction\" next=\"combat\"/>\
		</STATE>\
	\
		<STATE name=\"sweepFire\" cooltime=\"3000\" action=\"sweepFire\">\
			<TRANS cond=\"hpLess:0\" next=\"die\"/>\
			<TRANS cond=\"endAction\" next=\"combat\"/>\
		</STATE>\
	\
		<STATE name=\"die\" cooltime=\"0\" action=\"die\">\
		</STATE>\
	\
		<STATE name=\"afterblasted\" cooltime=\"0\" exitfunc=\"dice\">\
			<TRANS cond=\"default\" next=\"findTarget\"/>\
		</STATE>\
	</FSM>\
</XML>\
";

TEST(ZFSMParser, ReadXmlFromStr)
{
	vector<ZFSM*> v;
	ZFSMParser parser;
	ZActorActionManagerMock* pActionMgr = new ZActorActionManagerMock;
	parser.m_pActionMgr = pActionMgr;

	// 실패하는 경우
	EXPECT_FALSE(parser.ReadXmlFromStr(v, ""));
	EXPECT_TRUE(v.empty());
	EXPECT_FALSE(parser.ReadXmlFromStr(v, NULL));
	EXPECT_TRUE(v.empty());

	// 성공
	ASSERT_TRUE(parser.ReadXmlFromStr(v, g_szFsmXml));
	EXPECT_EQ(1, (int)v.size());
	ZFSM* pFsm = v[0];
	EXPECT_STREQ("monster1", pFsm->GetName());
	EXPECT_STREQ("findTarget", pFsm->GetEntryStateName());
	ZFSMState* pState = pFsm->GetState("findTarget");
	EXPECT_TRUE(NULL != pState);
	EXPECT_TRUE(NULL != pState->GetAction());
	EXPECT_STREQ("idle", pState->GetAction()->GetName());
	
	// func가 잘 읽혀졌나? 
	pState = pFsm->GetState("combat");
	EXPECT_TRUE(NULL != pState);
	EXPECT_EQ(1, pState->GetFunctionCallCount());
	EXPECT_EQ(ZFSMFunctionServer::FSMFUNC_DICE, pState->GetFunctionCall(0)->id);

	// enterfunc가 잘 읽혀졌나?
	pState = pFsm->GetState("combat");
	EXPECT_TRUE(NULL != pState);
	EXPECT_EQ(1, pState->GetEnterFunctionCallCount());
	EXPECT_EQ(ZFSMFunctionServer::FSMFUNC_BUILD_WAYPOINTS_TO_TARGET, pState->GetEnterFunctionCall(0)->id);

	// exitfunc가 잘 읽혀졌나?
	pState = pFsm->GetState("afterblasted");
	EXPECT_TRUE(NULL != pState);
	EXPECT_EQ(1, pState->GetExitFunctionCallCount());
	EXPECT_EQ(ZFSMFunctionServer::FSMFUNC_DICE, pState->GetExitFunctionCall(0)->id);

	// ReadXmlFromStr()에서 성공하면 모든 transition의 nextstate에 포인터가 세팅되어 있어야함
	ZFSMState* pNextState = pState->GetTransition(0)->GetNextState();
	EXPECT_TRUE(NULL != pNextState);
	EXPECT_STREQ("findTarget", pNextState->GetName());
	
	delete v[0];
	delete pActionMgr;
}


#endif
#include "stdafx.h"
#include "ZFSMParser.h"
#include "ZFSMTransition.h"
#include "ZFSMFunctionServer.h"
#include "ZFSMState.h"
#include "ZFSM.h"
#include "ZActorActionManager.h"
#include "ZActorAction.h"

ZFSMParser::ZFSMParser()
{
	m_pActionMgr = NULL;
}

void ZFSMParser::ParseMethodStatement(ZFSMMethodCall* pMc, const char* szStatement)
{
	if (!pMc) { _ASSERT(0); return; }
	pMc->strMethodName = "";

	for (int k=0; k<MAX_FSMMETHOD_ARG; ++k)
		pMc->nArg[k] = FSMMETHOD_ARG_NONE;
	
	enum { READ_METHOD, READ_ARG };

	int nReadMode = READ_METHOD;
	int iArg = 0;

	char szArg[32] = "";

	const char* szBegin = szStatement;
	const char* sz = szStatement;
	
	while (1)
	{
		// 메소드를 읽는 과정에서 ':'가 나오면 메소드 이름을 저장
		if (sz[0]==':' && nReadMode==READ_METHOD)
		{
			pMc->strMethodName.assign(szBegin, sz);
			++sz;	// ':'는 건너뜀
			szBegin = sz;
			nReadMode = READ_ARG;
		}

		// 인자를 읽는 과정에서 ';'가 나오면 인자를 저장
		else if (sz[0]==';' && nReadMode==READ_ARG)
		{
			if (iArg >= MAX_FSMMETHOD_ARG)
			{
				_ASSERT(0);
				return;
			}

			strncpy(szArg, szBegin, sz - szBegin);
			pMc->nArg[iArg] = atoi(szArg);
			++iArg;
			++sz;
			szBegin = sz;
		}

		// 문자열 끝이 나오면 현재 과정에 따라 처리
		else if (sz[0]==0)
		{
			switch (nReadMode)
			{
				// 이 경우는 메소드에 인자가 없이 끝난 경우
			case READ_METHOD:
				pMc->strMethodName.assign(szStatement);
				return;
				// 인자 나열이 끝난 경우
			case READ_ARG:
				strncpy(szArg, szBegin, sz - szBegin);
				pMc->nArg[iArg] = atoi(szArg);
				return;
			}
		}

		// 메소드 이름 읽는 도중 공백문자를 만난 경우 (메소드 이름 앞의 공백문자만 제대로 처리됨)
		else if (sz[0]=='\t' || sz[0]==' ')
		{
			if (nReadMode == READ_METHOD)
				++szBegin;
		}

		++sz;
	}
}

void ZFSMParser::DevideMultipleMethodStatement(vector<string>& out, const char* szStatement)
{
	out.clear();
	if (!szStatement) return;

	char szSingleStatement[128] = "";
	ZFSMMethodCall mc;

	const char* szBegin = szStatement;
	const char* sz = szStatement;

	while (1)
	{
		if (sz[0]==',')
		{
			strncpy(szSingleStatement, szBegin, sz - szBegin);
			out.push_back(szSingleStatement);

			++sz;	// ','를 건너뜀
			szBegin = sz;
		}

		else if (sz[0]==0)
		{
			int len = sz - szBegin;
			strncpy(szSingleStatement, szBegin, len);
			szSingleStatement[len] = 0;
			out.push_back(szSingleStatement);

		}

		if (sz[0]==0)
			return;

		++sz;
	}
}

bool ZFSMParser::ParseFunction(ZFSMFunctionCall* out_pFuncCall, const char* sz)
{
	if (!out_pFuncCall) return false;
	if (!sz) return false;

	ZFSMMethodCall mc;
	ParseMethodStatement(&mc, sz);

	int numArg = 0;
	for (int i=0; i<MAX_FSMMETHOD_ARG; ++i)
		if (mc.nArg[i] != FSMMETHOD_ARG_NONE) ++numArg;

	memset(out_pFuncCall, 0, sizeof(ZFSMFunctionCall));

	int funcId = ZFSMFunctionServer::FSMFUNC_INVALID;

	const char* szFuncName = mc.strMethodName.c_str();
		 if (!_stricmp(szFuncName, ""))					{ funcId = ZFSMFunctionServer::FSMFUNC_DONOTHING;			if(numArg!=0) return false; }
	else if (!_stricmp(szFuncName, "dice"))				{ funcId = ZFSMFunctionServer::FSMFUNC_DICE;				if(numArg!=0) return false; }
	
	else if (!_stricmp(szFuncName, "findTarget"))		{ funcId = ZFSMFunctionServer::FSMFUNC_FIND_TARGET;			if(numArg!=0) return false; }
	else if (!_stricmp(szFuncName, "setTargetLastAttacker"))		{ funcId = ZFSMFunctionServer::FSMFUNC_SET_TARGET_LAST_ATTACKER;	if(numArg!=0) return false; }

	else if (!_stricmp(szFuncName, "rotateToTarget"))	{ funcId = ZFSMFunctionServer::FSMFUNC_ROTATE_TO_TARGET;	if(numArg!=0) return false; }
	else if (!_stricmp(szFuncName, "faceToTarget"))		{ funcId = ZFSMFunctionServer::FSMFUNC_FACE_TO_TARGET;		if(numArg!=0) return false; }
	
	else if (!_stricmp(szFuncName, "reduceGroggy"))		{ funcId = ZFSMFunctionServer::FSMFUNC_REDUCE_GROGGY;		if(numArg!=1) return false; }
	
	else if (!_stricmp(szFuncName, "buildWaypointsToTarget"))	{ funcId = ZFSMFunctionServer::FSMFUNC_BUILD_WAYPOINTS_TO_TARGET;	if(numArg!=0) return false; }
	else if (!_stricmp(szFuncName, "runWaypoints"))		{ funcId = ZFSMFunctionServer::FSMFUNC_RUN_WAYPOINTS;		if(numArg!=0) return false; }
	else if (!_stricmp(szFuncName, "findTargetInHeight")) { funcId = ZFSMFunctionServer::FSMFUNC_FIND_TARGET_IN_HEIGHT; if (numArg != 1) return false; }
	else if (!_stricmp(szFuncName, "runAlongTargetOrbital")) { funcId = ZFSMFunctionServer::FSMFUNC_RUN_ALONG_TARGET_ORBITAL; if (numArg != 1) return false; }
	else if (!_stricmp(szFuncName, "clearWayPoints")) { funcId = ZFSMFunctionServer::FSMFUNC_CLEAR_WAYPOINTS; if (numArg != 0) return false; }
	else if (!_stricmp(szFuncName, "speedAccel")) { funcId = ZFSMFunctionServer::FSMFUNC_SPEED_ACCEL; if (numArg != 1) return false; }
	else if (!_stricmp(szFuncName, "turnOrbitalDirection")) { funcId = ZFSMFunctionServer::FSMFUNC_TURN_ORBITAL_DIR; if (numArg != 0) return false; }
	else if (!_stricmp(szFuncName, "runWaypointsAlongRoute")) { funcId = ZFSMFunctionServer::FSMFUNC_RUNWAYPOINTSALONGROUTE; if (numArg != 0) return false; }
	else if (!_stricmp(szFuncName, "findTargetInDist")) { funcId = ZFSMFunctionServer::FSMFUNC_FINDTARGETINDIST; if (numArg != 1) return false; }
	else if (!_stricmp(szFuncName, "faceToLastestAttacker")) { funcId = ZFSMFunctionServer::FSMFUNC_FACE_TO_LATEST_ATTACKER; if (numArg != 0) return false; }

	if (funcId == ZFSMFunctionServer::FSMFUNC_INVALID)
	{
		mlog("Unknown FSM function name : \'%s\'\n", szFuncName);
		return false;
	}

	out_pFuncCall->id = funcId;
	for (int i=0; i<MAX_FSMMETHOD_ARG; ++i)
		out_pFuncCall->nArg[i] = mc.nArg[i];

	return true;
}

bool ZFSMParser::ParseFunctionsStatement(vector<ZFSMFunctionCall>& out, const char* szStatement)
{
	vector<string> v;
	DevideMultipleMethodStatement(v, szStatement);

	int n = (int)v.size();
	for (int i=0; i<n; ++i)
	{
		ZFSMFunctionCall funcCall;
		if (false == ParseFunction(&funcCall, v[i].c_str()))
		{
			// 중간에 뭔가 잘못되면 지금까지 읽은 걸 다 삭제한다
			out.clear();
			return false;
		}
		out.push_back(funcCall);
	}

	return true;
}

bool ZFSMParser::ParseCondition( ZFSMCondition* out_pCond, const char* sz )
{
	if (!out_pCond) return false;

	ZFSMMethodCall mc;
	ParseMethodStatement(&mc, sz);

	int numArg = 0;
	for (int i=0; i<MAX_FSMMETHOD_ARG; ++i)
		if (mc.nArg[i] != FSMMETHOD_ARG_NONE) ++numArg;

	E_FSMCOND_ID condId = FSMCOND_INVALID;

	const char* szCondName = mc.strMethodName.c_str();
		 if (!_stricmp(szCondName, "default"))					{ condId = FSMCOND_DEFAULT;					if(numArg!=0) return false; }
	else if (!_stricmp(szCondName, "Dice"))						{ condId = FSMCOND_DICE;					if(numArg!=1) return false; }
	else if (!_stricmp(szCondName, "hpEqual"))					{ condId = FSMCOND_HP_EQUAL;				if(numArg!=1) return false; }
	else if (!_stricmp(szCondName, "HpLess"))					{ condId = FSMCOND_HP_LESS;					if(numArg!=1) return false; }
	else if (!_stricmp(szCondName, "HpGreater"))					{ condId = FSMCOND_HP_GREATER;				if(numArg!=1) return false; }
	else if (!_stricmp(szCondName, "GroggyGreater"))				{ condId = FSMCOND_GROGGY_GREATER;			if(numArg!=1) return false; }
	else if (!_stricmp(szCondName, "VelocityZLess"))				{ condId = FSMCOND_VELOCITY_Z_LESS;			if(numArg!=1) return false; }
	else if (!_stricmp(szCondName, "VelocityMagnitudeLess"))		{ condId = FSMCOND_VELOCITY_MAGNITUDE_LESS;	if(numArg!=1) return false; }
	else if (!_stricmp(szCondName, "EndAction"))					{ condId = FSMCOND_END_ACTION;				if(numArg!=0) return false; }
	else if (!_stricmp(szCondName, "IsLanding"))					{ condId = FSMCOND_IS_LANDING;				if(numArg!=0) return false; }
	
	else if (!_stricmp(szCondName, "HasTarget"))					{ condId = FSMCOND_HAS_TARGET;				if(numArg!=0) return false; }
	else if (!_stricmp(szCondName, "HasNoTarget"))				{ condId = FSMCOND_HAS_NO_TARGET;			if(numArg!=0) return false; }
	else if (!_stricmp(szCondName, "DistTarget"))				{ condId = FSMCOND_DIST_TARGET_IN;			if(numArg!=2) return false; }
	else if (!_stricmp(szCondName, "LookAtTarget"))				{ condId = FSMCOND_LOOK_AT_TARGET;			if(numArg!=1) return false; }

	else if (!_stricmp(szCondName, "IsEmptySpace"))				{ condId = FSMCOND_IS_EMPTY_SPACE;			if(numArg!=2) return false; }
	
	else if (!_stricmp(szCondName, "TimeElapsedSinceEntered"))	{ condId = FSMCOND_TIME_ELAPSED_SINCE_ENTERED;	if(numArg!=1) return false; }
	else if (!_stricmp(szCondName, "EndLightningDamage"))		{ condId = FSMCOND_END_LIGHTNINGDAMAGE;		if(numArg!=0) return false; }
	else if (!_stricmp(szCondName, "canSeeTarget")) { condId = FSMCOND_CAN_SEE_TARGET; if (numArg != 0) return false; }
	else if (!_stricmp(szCondName, "FailedBuildWayPoints")) { condId = FSMCOND_FAILED_BUILD_WAYPOINTS; if (numArg != 0) return false; }
	else if (!_stricmp(szCondName, "angleTargetHeight")) { condId = FSMCOND_ANGLE_TARGET_HEIGHT; if (numArg != 2) return false; }
	else if (!_stricmp(szCondName, "TargetHeightHigher")) { condId = FSMCOND_TARGET_HEIGHT_HIGHER; if (numArg != 1) return false; }
	else if (!_stricmp(szCondName, "cannotSeeTarget")) { condId = FSMCOND_CANT_SEE_TARGET; if (numArg != 0) return false; }
	else if (!_stricmp(szCondName, "SummonLess")) { condId = FSMCOND_SUMMONLESS; if (numArg != 1) return false; }

	//todok distTarget:x;y 를 파싱할 때 y가 x보다 큰 경우 경고를 내야 할까?

	if (condId == FSMCOND_INVALID)
	{
		mlog("Unknown FSM condition name : \'%s\'\n", szCondName);
		return false;
	}

	out_pCond->Init(condId, mc.nArg[0], mc.nArg[1], mc.nArg[2], mc.nArg[3]);
	return true;
}

ZFSMTransition* ZFSMParser::ParseTransition(const char* szCondition, const char* szNextState)
{
	if (!szCondition || szCondition[0]==0) return NULL;
	if (!szNextState || szNextState[0]==0) return NULL;

	vector<string> v;
	DevideMultipleMethodStatement(v, szCondition);
	
	if (v.empty()) return NULL;

	ZFSMTransition* pTrans = new ZFSMTransition;
	int n = (int)v.size();
	for (int i=0; i<n; ++i)
	{
		ZFSMCondition* pCond = new ZFSMCondition;
		if (ParseCondition(pCond, v[i].c_str()))
			pTrans->AddCondition(pCond);
		else
		{
			delete pCond;
			delete pTrans;	// 뭔가 잘못되면 전부 삭제하고 중지
			return NULL;
		}
	}

	pTrans->SetNextStateName(szNextState);
	return pTrans;
}

bool ZFSMParser::ReadXml(MZFileSystem* pFileSystem, const char* szFileName, vector<ZFSM*>& out, ZActorActionManager* pActionMgr)
{
	m_pActionMgr = pActionMgr;


	char *buffer;
	MZFile mzf;

	if (pFileSystem) {
		if (!mzf.Open(szFileName, pFileSystem)) {
			if (!mzf.Open(szFileName)) {
				return false;
			}
		}
	}
	else {

		if (!mzf.Open(szFileName)) {

			return false;
		}
	}

	buffer = new char[mzf.GetLength() + 1];
	buffer[mzf.GetLength()] = 0;

	mzf.Read(buffer, mzf.GetLength());

	bool bResult = ReadXmlFromStr(out, buffer);

	delete[] buffer;
	mzf.Close();

	return bResult;
}

bool ZFSMParser::ReadXmlFromStr(vector<ZFSM*>& out, const char* szXmlContent)
{
	if (!szXmlContent) return NULL;

	rapidxml::xml_document<> doc;
	if (!doc.parse<0>((char*)szXmlContent)) {
		mlog("error reading fsm xml.\n");
		return false;
	}
	bool bResult = true;;

	rapidxml::xml_node<>* rootNode = doc.first_node();
	for (auto itor = rootNode->first_node(); itor; itor = itor->next_sibling())
	{
		if (_stricmp(itor->name(), "FSM") == 0)
		{
			ZFSM* pFSM = ReadXmlNode_FSM(itor);
			if (pFSM)
				out.push_back(pFSM);
			else {
				for (int i = 0; i<(int)out.size(); ++i)
					delete out[i];
				out.clear();
				bResult = false;
				break;
			}
		}
	}

	doc.clear();
	return bResult;
}

ZFSM* ZFSMParser::ReadXmlNode_FSM(rapidxml::xml_node<>* elem)
{
	if (elem->first_attribute("name") == nullptr || elem->first_attribute("entrystate") == nullptr) {
		mlog("error : FSM name or entrystate is empty.\n");
	}

	ZFSM* pFsm = new ZFSM;
	pFsm->SetName(elem->first_attribute("name")->value());
	pFsm->SetEntryStateName(elem->first_attribute("entrystate")->value());

	rapidxml::xml_node<>* childNode = elem->first_node();
	for (auto itor = childNode; itor; itor = itor->next_sibling())
	{
		if (itor->name()[0] == '#')
			continue;
		if (_stricmp(itor->name(), "STATE") == 0)
		{
			ZFSMState* pState = ReadXmlNode_STATE(itor);
			if (pState) {
				pFsm->AddState(pState);
			}
			else {
				delete pFsm;
				return nullptr;
			}
		}
	}

	// built-in 스테이트 추가
	ZFSMState* pBuildinState;
	pBuildinState = ZFSMState::CreateBuiltInState_Die(m_pActionMgr);
	pFsm->AddState(pBuildinState);
	pBuildinState = ZFSMState::CreateBuiltInState_WaitDelete(m_pActionMgr);
	pFsm->AddState(pBuildinState);
	pBuildinState = ZFSMState::CreateBuiltInState_BlastedSoar(m_pActionMgr);
	pFsm->AddState(pBuildinState);
	pBuildinState = ZFSMState::CreateBuiltInState_BlastedFall(m_pActionMgr);
	pFsm->AddState(pBuildinState);
	pBuildinState = ZFSMState::CreateBuiltInState_BlastedDown(m_pActionMgr);
	pFsm->AddState(pBuildinState);
	pBuildinState = ZFSMState::CreateBuiltInState_BlastedStandup(m_pActionMgr);
	pFsm->AddState(pBuildinState);
	pBuildinState = ZFSMState::CreateBuiltInState_BlastedThrust(m_pActionMgr);
	pFsm->AddState(pBuildinState);
	pBuildinState = ZFSMState::CreateBuiltInState_BlastedThrustDown(m_pActionMgr);
	pFsm->AddState(pBuildinState);
	pBuildinState = ZFSMState::CreateBuiltInState_LightningStun(m_pActionMgr);
	pFsm->AddState(pBuildinState);

	// 전이가능한 스테이트를 포인터로 연결
	pFsm->PrepareTransNextState();
	return pFsm;
}

ZFSMState* ZFSMParser::ReadXmlNode_STATE(rapidxml::xml_node<>* elem)
{
	if (strlen(elem->first_attribute("name")->value()) == 0 || elem->first_attribute("name") == nullptr)
		return false;

	ZFSMState* pState = new ZFSMState;
	pState->SetName(elem->first_attribute("name")->value());
	if (elem->first_attribute("cooltime") != nullptr)
		pState->SetCooltime(atoi(elem->first_attribute("cooltime")->value()));
	else
		pState->SetCooltime(0);

	vector<ZFSMFunctionCall> vecFuncCall;
	if (elem->first_attribute("func") != nullptr) {
		if (!ParseFunctionsStatement(vecFuncCall, elem->first_attribute("func")->value()))
		{
			delete pState;
			return nullptr;
		}
		pState->SetFunctionCalls(vecFuncCall);

	}

	vector<ZFSMFunctionCall> vecEnterFuncCall;
	if (elem->first_attribute("enterfunc") != nullptr) {
		if (!ParseFunctionsStatement(vecEnterFuncCall, elem->first_attribute("enterfunc")->value()))
		{
			delete pState;
			return nullptr;
		}
		pState->SetEnterFunctionCalls(vecEnterFuncCall);

	}

	vector<ZFSMFunctionCall> vecExitFuncCall;
	if (elem->first_attribute("exitfunc") != nullptr) {
		if (!ParseFunctionsStatement(vecExitFuncCall, elem->first_attribute("exitfunc")->value()))
		{
			delete pState;
			return nullptr;
		}
		pState->SetExitFunctionCalls(vecExitFuncCall);
	}

	// action 찾아서 연결하기
	if (!m_pActionMgr)
	{
		_ASSERT(0);
		delete pState;
		return NULL;
	}

	ZActorAction* pAction = m_pActionMgr->GetAction(elem->first_attribute("action") == nullptr ? "" : elem->first_attribute("action")->value());
	pState->SetAction(pAction);	// pAction == NULL도 가능

	rapidxml::xml_node<>* childNode = elem->first_node();
	for (auto itor = childNode; itor; itor = itor->next_sibling())
	{
		if (itor->name()[0] == '#')
			continue;
		if (_stricmp(itor->name(), "TRANS") == 0)
		{
			ZFSMTransition* ptrans = ReadXmlNode_TRANS(itor);
			if (ptrans) {
				pState->AddTransition(ptrans);
			}
			else {
				delete pState;
				return nullptr;
			}
		}
	}


	// dice 전처리
	pState->MakeDiceFuncHasMaxValue();
	pState->MakeDiceConditionHasRange();

	return pState;
}

ZFSMTransition* ZFSMParser::ReadXmlNode_TRANS(rapidxml::xml_node<>* elem)
{
	ZFSMTransition* pTrans = ParseTransition(elem->first_attribute("cond")->value(), elem->first_attribute("next")->value());
	return pTrans;
}
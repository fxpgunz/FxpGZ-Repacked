#pragma once

#include "ZFSMCondition.h"

class ZFSMTransition;
struct ZFSMFunctionCall;
class ZFSM;
class ZFSMState;
class ZFSMTransition;
class ZActorActionManager;

struct ZFSMMethodCall 
{
	string strMethodName;
	int nArg[MAX_FSMMETHOD_ARG];
};

class ZFSMParser
{
public:
	ZFSMParser();

	// 함수 실행문을 함수명과 인자로 분리한다
	void ParseMethodStatement(ZFSMMethodCall* pMc, const char* szStatement);
	// 함수 실행문 여러개가 연결되어 있는 문자열을 하나씩 분리해낸다
	void DevideMultipleMethodStatement(vector<string>& out, const char* szStatement);

	// 여러개의 펑션실행이 표현된 문자열을 펑션실행 객체들의 배열로 변환
	bool ParseFunctionsStatement(vector<ZFSMFunctionCall>& out, const char* szStatement);
	// 펑션실행 문자열을 펑션실행 객체로 변환
	bool ParseFunction(ZFSMFunctionCall* out_pFuncCall, const char* sz);

	// 조건문 문자열을 조건객체로 변환
	bool ParseCondition(ZFSMCondition* out_pCond, const char* sz);
	// 문자열을 전이객체로 변환
	ZFSMTransition* ParseTransition(const char* szCondition, const char* szNextState);

	// xml로부터 파싱
	ZActorActionManager* m_pActionMgr;

	bool ReadXml(MZFileSystem* pFileSystem, const char* szFileName, vector<ZFSM*>& out, ZActorActionManager* pActionMgr);
	bool ReadXmlFromStr(vector<ZFSM*>& out, const char* szXmlContent);
	ZFSM* ReadXmlNode_FSM(rapidxml::xml_node<>* elem);
	ZFSMState* ReadXmlNode_STATE(rapidxml::xml_node<>* elem);
	ZFSMTransition* ReadXmlNode_TRANS(rapidxml::xml_node<>* elem);
};

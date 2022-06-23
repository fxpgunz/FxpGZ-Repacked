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

	// �Լ� ���๮�� �Լ���� ���ڷ� �и��Ѵ�
	void ParseMethodStatement(ZFSMMethodCall* pMc, const char* szStatement);
	// �Լ� ���๮ �������� ����Ǿ� �ִ� ���ڿ��� �ϳ��� �и��س���
	void DevideMultipleMethodStatement(vector<string>& out, const char* szStatement);

	// �������� ��ǽ����� ǥ���� ���ڿ��� ��ǽ��� ��ü���� �迭�� ��ȯ
	bool ParseFunctionsStatement(vector<ZFSMFunctionCall>& out, const char* szStatement);
	// ��ǽ��� ���ڿ��� ��ǽ��� ��ü�� ��ȯ
	bool ParseFunction(ZFSMFunctionCall* out_pFuncCall, const char* sz);

	// ���ǹ� ���ڿ��� ���ǰ�ü�� ��ȯ
	bool ParseCondition(ZFSMCondition* out_pCond, const char* sz);
	// ���ڿ��� ���̰�ü�� ��ȯ
	ZFSMTransition* ParseTransition(const char* szCondition, const char* szNextState);

	// xml�κ��� �Ľ�
	ZActorActionManager* m_pActionMgr;

	bool ReadXml(MZFileSystem* pFileSystem, const char* szFileName, vector<ZFSM*>& out, ZActorActionManager* pActionMgr);
	bool ReadXmlFromStr(vector<ZFSM*>& out, const char* szXmlContent);
	ZFSM* ReadXmlNode_FSM(rapidxml::xml_node<>* elem);
	ZFSMState* ReadXmlNode_STATE(rapidxml::xml_node<>* elem);
	ZFSMTransition* ReadXmlNode_TRANS(rapidxml::xml_node<>* elem);
};

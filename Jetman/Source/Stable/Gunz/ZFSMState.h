#pragma once

#include "ZFSMCondition.h"
#include "ZFSMFunctionServer.h"

class ZActorActionManager;
class ZFSMTransition;
class IActorAction;

//todok �� �Ʒ��� �ּ��� �޷��ִ� mandatory���� ������ üũ�ϴ� �ڵ带 �ۼ��ؾ� ��

// FSM Built-in State, Action, Animation
#define FSM_BISTATE_DIE					"__die"						// �״� ����
#define FSM_BISTATE_BLASTEDSOAR			"__blastedsoar"				// ��� ���� ���
#define FSM_BISTATE_BLASTEDFALL			"__blastedfall"				// ��� ���� �߶�
#define FSM_BISTATE_BLASTEDDOWN			"__blasteddown"				// ��� ���� �ٴڿ� ������
#define FSM_BISTATE_BLASTEDTHRUST		"__blastedthrust"			// �ܰ� �뽬���� �з���
#define FSM_BISTATE_BLASTEDTHRUSTDOWN	"__blastedthrustdown"		// �ܰ� �뽬���� �ٴڿ� ������
#define FSM_BISTATE_BLASTEDSTANDUP		"__blastedstandup"			// ��� ����/�ܰ� �뽬���� ������ �Ͼ
#define FSM_BISTATE_LIGHTNINGSTUN		"__lightningstun"			// ����Ʈ�� ������ �°� ����
// ����Ʈ ���¿� ����Ʈ�׽����� ��� ������ ������ ���� �̸�. xml������ �� �̸��� ���¸� �ݵ�� �����ؾ� �Ѵ�
#define FSM_STATE_AFTERBLASTED		"afterblasted"

#define FSM_BIACTION_DIE				"__die"
#define FSM_BISTATE_WAIT_DELETE			"__waitdelete"
#define FSM_BIACTION_BLASTEDSOAR		"__blastedsoar"
#define FSM_BIACTION_BLASTEDFALL		"__blastedfall"
#define FSM_BIACTION_BLASTEDDOWN		"__blasteddown"
#define FSM_BIACTION_BLASTEDTHRUST		"__blastedthrust"
#define FSM_BIACTION_BLASTEDTHRUSTDOWN	"__blastedthrustdown"
#define FSM_BIACTION_BLASTEDSTANDUP		"__blastedstandup"
#define FSM_BIACTION_LIGHTNINGSTUN		"__lightningstun"

// npc�� �Ʒ� �̸��� animation�� �ݵ�� ������ �־�� �Ѵ�. (�ش� ���ݿ� �������ִ� npc�� ������� (neverblasted��)
#define FSM_ANIMATION_DIE				"die"
#define FSM_ANIMATION_BLASTEDSOAR		"blast_soar"
#define FSM_ANIMATION_BLASTEDFALL		"blast_fall"
#define FSM_ANIMATION_BLASTEDDOWN		"blast_down"
#define FSM_ANIMATION_BLASTEDTHRUST		"blast_thrust"
#define FSM_ANIMATION_BLASTEDTHRUSTDOWN	"blast_thrustdown"
#define FSM_ANIMATION_STANDUP			"standup"
#define FSM_ANIMATION_LIGHTNINGSTUN		"lightningstun"


// NPC�� ���±�迡�� ���¸� ��Ÿ���� Ŭ����
class ZFSMState
{
	string m_strName;
	int m_nCooltime;
	IActorAction* m_pAction;
	
	vector<ZFSMFunctionCall> m_vecFuncCall;			// ���� ������Ʈ �� ������ ���
	vector<ZFSMFunctionCall> m_vecEnterFuncCall;	// ���� ���Խ� ������ ���
	vector<ZFSMFunctionCall> m_vecExitFuncCall;		// ���� Ż��� ������ ���

	vector<ZFSMTransition*> m_vecTrans;

public:
	ZFSMState();
	~ZFSMState();

	void SetName(const char* sz) { m_strName=sz; }
	const char* GetName() { return m_strName.c_str(); }

	void SetCooltime(int n);
	int GetCooltime() { return m_nCooltime; }

	void SetAction(IActorAction* pAction);
	IActorAction* GetAction() { return m_pAction; }

	void AddTransition(ZFSMTransition* pTrans);
	ZFSMState* CheckTransition(IFSMConditionSource* pCondSrc);
	ZFSMTransition* GetTransition(int idx);
	int GetTransitionCount() { return (int)m_vecTrans.size(); }

	void SetFunctionCalls(vector<ZFSMFunctionCall>& v);
	const ZFSMFunctionCall* GetFunctionCall( unsigned int i );
	int GetFunctionCallCount() { return (int)m_vecFuncCall.size(); }

	void SetEnterFunctionCalls(vector<ZFSMFunctionCall>& v);
	const ZFSMFunctionCall* GetEnterFunctionCall( unsigned int i );
	int GetEnterFunctionCallCount() { return (int)m_vecEnterFuncCall.size(); }

	void SetExitFunctionCalls(vector<ZFSMFunctionCall>& v);
	const ZFSMFunctionCall* GetExitFunctionCall( unsigned int i );
	int GetExitFunctionCallCount() { return (int)m_vecExitFuncCall.size(); }

	// �ݵ�� ������� ȣ��Ǿ�� ��  MakeDiceFuncHasMaxValue() ���Ŀ� MakeDiceConditionHasRange()
	void MakeDiceFuncHasMaxValue();
	void MakeDiceConditionHasRange();


public:
	static ZFSMState* CreateBuiltInState_Die(ZActorActionManager* pActionMgr);
	static ZFSMState* CreateBuiltInState_WaitDelete(ZActorActionManager* pActionMgr);
	static ZFSMState* CreateBuiltInState_BlastedSoar(ZActorActionManager* pActionMgr);
	static ZFSMState* CreateBuiltInState_BlastedFall(ZActorActionManager* pActionMgr);
	static ZFSMState* CreateBuiltInState_BlastedDown(ZActorActionManager* pActionMgr);
	static ZFSMState* CreateBuiltInState_BlastedStandup(ZActorActionManager* pActionMgr);
	static ZFSMState* CreateBuiltInState_BlastedThrust(ZActorActionManager* pActionMgr);
	static ZFSMState* CreateBuiltInState_BlastedThrustDown(ZActorActionManager* pActionMgr);
	static ZFSMState* CreateBuiltInState_LightningStun(ZActorActionManager* pActionMgr);
};

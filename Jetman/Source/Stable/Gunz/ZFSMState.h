#pragma once

#include "ZFSMCondition.h"
#include "ZFSMFunctionServer.h"

class ZActorActionManager;
class ZFSMTransition;
class IActorAction;

//todok 이 아래에 주석에 달려있는 mandatory들을 실제로 체크하는 코드를 작성해야 함

// FSM Built-in State, Action, Animation
#define FSM_BISTATE_DIE					"__die"						// 죽는 상태
#define FSM_BISTATE_BLASTEDSOAR			"__blastedsoar"				// 장검 띄우기 상승
#define FSM_BISTATE_BLASTEDFALL			"__blastedfall"				// 장검 띄우기 추락
#define FSM_BISTATE_BLASTEDDOWN			"__blasteddown"				// 장검 띄우기 바닥에 쓰러짐
#define FSM_BISTATE_BLASTEDTHRUST		"__blastedthrust"			// 단검 대쉬어택 밀려남
#define FSM_BISTATE_BLASTEDTHRUSTDOWN	"__blastedthrustdown"		// 단검 대쉬어택 바닥에 쓰러짐
#define FSM_BISTATE_BLASTEDSTANDUP		"__blastedstandup"			// 장검 띄우기/단검 대쉬어택 끝나고 일어남
#define FSM_BISTATE_LIGHTNINGSTUN		"__lightningstun"			// 라이트닝 강베기 맞고 스턴
// 블래스트 상태와 라이트닝스턴이 모두 끝나면 전이할 상태 이름. xml에서는 이 이름의 상태를 반드시 제공해야 한다
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

// npc는 아래 이름의 animation을 반드시 가지고 있어야 한다. (해당 공격에 내성이있는 npc는 상관없음 (neverblasted등)
#define FSM_ANIMATION_DIE				"die"
#define FSM_ANIMATION_BLASTEDSOAR		"blast_soar"
#define FSM_ANIMATION_BLASTEDFALL		"blast_fall"
#define FSM_ANIMATION_BLASTEDDOWN		"blast_down"
#define FSM_ANIMATION_BLASTEDTHRUST		"blast_thrust"
#define FSM_ANIMATION_BLASTEDTHRUSTDOWN	"blast_thrustdown"
#define FSM_ANIMATION_STANDUP			"standup"
#define FSM_ANIMATION_LIGHTNINGSTUN		"lightningstun"


// NPC의 상태기계에서 상태를 나타내는 클래스
class ZFSMState
{
	string m_strName;
	int m_nCooltime;
	IActorAction* m_pAction;
	
	vector<ZFSMFunctionCall> m_vecFuncCall;			// 상태 업데이트 중 실행할 펑션
	vector<ZFSMFunctionCall> m_vecEnterFuncCall;	// 상태 진입시 실행할 펑션
	vector<ZFSMFunctionCall> m_vecExitFuncCall;		// 상태 탈출시 실행할 펑션

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

	// 반드시 순서대로 호출되어야 함  MakeDiceFuncHasMaxValue() 이후에 MakeDiceConditionHasRange()
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

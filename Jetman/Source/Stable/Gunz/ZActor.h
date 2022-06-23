#ifndef _ZACTOR_H
#define _ZACTOR_H

#include "MRTTI.h"
#include "ZCharacterObject.h"
#include "ZCharacter.h"
#include "MUID.h"
//#include "RVisualMeshMgr.h"
#include "MObjectTypes.h"
#include "ZStateMachine.h"
#include "ZAI_Base.h"
#include "MBaseQuest.h"
#include "IBrain.h"
#include "ZActorAnimation.h"
#include "ZTask.h"
#include "ZTaskManager.h"
#include "ZModule_HPAP.h"
#include "MMatchTransDataType.h"
#include "ZNPCInfoFromServer.h"
#include "IGame.h"
#include "ZActorBase.h"

#include <list>
#include <string>
using namespace std;

_USING_NAMESPACE_REALSPACE2


enum ZACTOR_FLAG
{
	AF_NONE				= 0,
	AF_LAND				= 0x1,				// 발이 땅에 닿아 있는지 여부
	AF_BLAST			= 0x2,				// 띄움을 당한 상태인지 여부
	AF_MOVING			= 0x4,				// 이동여부
	AF_DEAD				= 0x8,				// 죽었다
	AF_REQUESTED_DEAD	= 0x10,				// 서버에 죽음 요청을 했는지 여부
	AF_BLAST_DAGGER		= 0x20,				// 띄움을 당한 상태인지 여부	

	AF_MY_CONTROL		= 0x100,			// 내가 컨트롤하는지 여부


	AF_SOUND_WOUNDED	= 0x1000,			// 한번 맞았을때 울었는지 여부
};

// 일정 시간간격으로 NPC정보 계속 주고받는 데이터
struct ZACTOR_BASICINFO {
	float			fTime;
	MUID			uidNPC;
	short			posx,posy,posz;
	short			velx,vely,velz;
	short			dirx,diry,dirz;
	BYTE			anistate;
};

struct MQuestNPCInfo;


/// 기존 퀘스트/서바이벌에서 사용하는 NPC
class ZActor : public ZActorBase
{
	MDeclareRTTI;

private:
	unsigned long int		m_nFlags;
	void UpdateHeight(float fDelta);
	void UpdatePosition(float fDelta);
protected:
	float					m_fTC;				// 난이도 조절 계수
	int						m_nQL;				// 이넘의 QL - 게임정보의 QL과 같다.
	MMatchItemDesc			m_ItemDesc;			// npc의 무기(혹은 아이템 디스크립션)
	MQuestNPCInfo*			m_pNPCInfo;
	ZActorAnimation			m_Animation;		// Animation부분을 관장
	IBrain*					m_pBrain;			// AI 부분을 관장
	ZTaskManager			m_TaskManager;
	float					m_timeDieAnimationDone;
	float					m_fSpeed;			// 속도는 인스턴스 개개마다 다 다르다.
	int						m_nDamageCount;
	bool					m_bReserveStandUp;	// 쓰러지고 나서 일어나는데 지연 시간을 가짐
	DWORD					m_dwStandUp;		// 쓰러지고 나서 일어나는데까지 걸리는 시간
private:
	void InitFromNPCType(MQUEST_NPC nNPCType, float fTC, int nQL, IBrain* pBrain);
	void InitMesh(char* szMeshName, MQUEST_NPC nNPCType);
	void OnTaskFinished(ZTASK_ID nLastID);
	static void OnTaskFinishedCallback(ZActor* pActor, ZTASK_ID nLastID);
	inline static int CalcMaxHP(int nQL, int nSrcHP);
	inline static int CalcMaxAP(int nQL, int nSrcAP);
protected:
	enum ZACTOR_LASTTIME
	{
		ACTOR_LASTTIME_HPINFO		= 0,
		ACTOR_LASTTIME_BASICINFO,
		ACTOR_LASTTIME_MAX
	};
	unsigned long int	m_nLastTime[ACTOR_LASTTIME_MAX];

	ZModule_HPAP			*m_pModule_HPAP;
	ZModule_Resistance		*m_pModule_Resistance;
	ZModule_FireDamage		*m_pModule_FireDamage;
	ZModule_ColdDamage		*m_pModule_ColdDamage;
	ZModule_PoisonDamage	*m_pModule_PoisonDamage;
	ZModule_LightningDamage	*m_pModule_LightningDamage;
	ZModule_Skills			*m_pModule_Skills;

	rvector				m_vAddBlastVel;
	float				m_fAddBlastVelTime;
	rvector				m_TargetDir;
	rvector				m_Accel;
	float				m_fDelayTime;

//	ZBasicInfoHistory	m_BasicHistory;			///< 판정을 위해 몇초간의 데이터를 가지고있는다

protected:
	bool				m_bTestControl;
	void TestControl(float fDelta);

	virtual void InitProperty();
	virtual void InitStatus();

	virtual void OnDraw();
	virtual void OnUpdate(float fDelta);
	virtual bool ProcessMotion(float fDelta);
	virtual void ProcessNetwork(float fDelta);

	// task
	virtual void ProcessAI(float fDelta);
	void ProcessMovement(float fDelta);
	void CheckDead(float fDelta);

	// basicinfo
	void PostBasicInfo();
	void PostBossHpAp();

public:
	ZActor(IGame* pGame);
	virtual ~ZActor();
	static ZActor* CreateActor(MQUEST_NPC nNPCType, float fTC, int nQL, bool bForceCollRadius35, ZActor* pExternalAllocBody, IBrain* pExternalAllocBrain);
	virtual void InputBasicInfo(ZBasicInfo* pni, BYTE anistate);
	virtual void InputBossHpAp(float fHp, float fAp);
	void Input(AI_INPUT_SET nInput);
	void DebugTest();
	virtual void SetMyControl(bool bMyControl);

	inline ZA_ANIM_STATE	GetCurrAni();
	inline void SetFlag(unsigned int nFlag, bool bValue);
	inline bool CheckFlag(unsigned int nFlag);
	inline void SetFlags(unsigned int nFlags);
	inline unsigned long GetFlags();
	virtual bool IsMyControl() { return CheckFlag(AF_MY_CONTROL); }
	inline int GetHP();
	inline int GetAP();
	inline float GetTC();
	inline int GetQL();
	inline float GetHitRate();
	virtual int GetActualHP();
	virtual int GetActualAP();
	virtual int GetActualMaxHP();
	virtual int GetActualMaxAP();
	virtual bool IsNeverPushed() { if (m_pNPCInfo) return m_pNPCInfo->bNeverPushed; return false; }
	virtual float GetCollideRadius() { if (m_pNPCInfo) return m_pNPCInfo->fCollRadius; return 0;}
	virtual float ModifyAttackDamage(float fDamage)
	{
		return fDamage * (GetQL() * 0.2f + 1);
	}
public:
	void RunTo(rvector& dir);
	void Stop(bool bWithAniStop=true);		// bWithAniStop이 false이면 애니메이션은 여전히 달리는 동작이다.
	void RotateTo(rvector& dir);
	void OnNeglect( int nNum);

	virtual void OnBlast(rvector &dir);
	virtual void OnBlastDagger(rvector &dir,rvector& pos);
	virtual bool IsCollideable();
	virtual bool IsAttackable();
	virtual void Attack_Melee();
	virtual void Attack_Range(rvector& dir);
	virtual void Skill(int nSkill);

	bool isThinkAble();

	IBrain* GetBrain()					{ return m_pBrain; }
	MQuestNPCInfo* GetNPCInfo()			{ return m_pNPCInfo; }
	ZTaskManager* GetTaskManager()		{ return &m_TaskManager; }

	// 특정시점의 hit test 를 리턴해줘야 한다, pOutPos 가 있으면 hit된 위치를 리턴해줘야 한다
	virtual ZOBJECTHITTEST HitTest( const rvector& origin, const rvector& to, float fTime, rvector *pOutPos=NULL );

	// 동작/이벤트 상속
	virtual bool IsDie();

	virtual MMatchTeam GetTeamID() { return MMT_BLUE; }			// NPC는 무조건 BLUE팀

//	virtual void OnDamage(int damage, float fRatio = 1.0f);
	virtual void OnDamaged(ZObject* pAttacker, rvector srcPos, ZDAMAGETYPE damageType, MMatchWeaponType weaponType, float fDamage, float fPiercingRatio=1.f, int nMeleeType=-1);

	// knockback을 적용받아야한다
	virtual void OnKnockback(rvector& dir, float fForce);
	virtual void OnDie();
	virtual void OnPeerDie(MUID& uidKiller);

	bool IsDieAnimationDone();
	bool CanSee(ZObject* pTarget);				///< 시야에 보이는지 여부
	bool CanAttackRange(ZObject* pTarget);		///< 원거리 공격 가능한지 여부
	bool CanAttackMelee(ZObject* pTarget, ZSkillDesc *pSkillDesc=NULL);	///< 근거리 공격 가능한지 여부, 만약 pSkillDesc=NULL이면 기본 근접공격

	MUID GetLastAttacker() { return m_pModule_HPAP->GetLastAttacker(); }

	// 외부객체에 의존하고 있는 코드를 가상함수로 감싸서 testable하도록 만듬
protected:
	virtual MQuestNPCInfo* GetQuestNPCInfo(MQUEST_NPC nNPCType) { return ZGetQuest()->GetNPCInfo(nNPCType); }
	virtual MUID GetQuestBossUid() { return ZGetQuest()->GetGameInfo()->GetBoss(); }
	virtual bool GetQuestCheat(ZQuestCheetType cheat);
	virtual const MTD_NPCINFO* GetMyActorServerNPCInfo( const MQUEST_NPC nNPCID );
	virtual void AddLandingEffect(rvector vPos, rvector vDir);
	virtual void ShockBossGauge(float fDamage);
	virtual void LinkAniEventSet(MQUEST_NPC nNPCType);
	virtual float MakeSpeed( float fSpeed);

public:
	virtual float MakePathFindingUpdateTime( char nIntelligence);
	virtual float MakeAttackUpdateTime( char nAgility);
	virtual float MakeDefaultAttackCoolTime();

	virtual void GetDebugInfoString(char* szBuf1, char* szBuf2, int index);
};



void SetClientNPCInfoFromServerNPCInfo( MQuestNPCInfo* pClientNPCInfo, const MTD_NPCINFO* pServerNPCInfo );



// inline /////////////////////////////////////////////////////////////////////////////////////////
inline void ZActor::SetFlags(unsigned int nFlags)
{
	if (m_nFlags != nFlags)
	{
		m_nFlags = nFlags;
	}
}

inline void ZActor::SetFlag(unsigned int nFlag, bool bValue)
{
	if (bValue) m_nFlags |= nFlag;
	else m_nFlags &= ~nFlag;
}

inline bool ZActor::CheckFlag(unsigned int nFlag)
{
	return ((m_nFlags & nFlag) != 0);
}

inline unsigned long ZActor::GetFlags() 
{ 
	return m_nFlags; 
}

inline ZA_ANIM_STATE ZActor::GetCurrAni()
{ 
	return m_Animation.GetCurrState();
}

inline int ZActor::GetHP()	
{ 
	return m_pModule_HPAP->GetHP(); 
}

inline int ZActor::GetAP()	
{
	return m_pModule_HPAP->GetAP(); 
}

inline float ZActor::GetTC()
{ 
	return m_fTC; 
}

inline int ZActor::GetQL()
{
	return m_nQL;
}

inline float ZActor::GetHitRate()
{
	return (m_fTC * m_pNPCInfo->fAttackHitRate);
}

inline int ZActor::CalcMaxHP(int nQL, int nSrcHP)
{
	return (int)(((float)nQL * 0.2f + 1) * nSrcHP);
}

inline int ZActor::CalcMaxAP(int nQL, int nSrcAP)
{
	return (int)(((float)nQL * 0.2f + 1) * nSrcAP);
}

////////////////////////////////////////////////
// ○ NPC 정책
//
// * 모든 NPC는 같은 구조의 애니메이션 세트를 가진다. -> ZActorAnimation
// * 모든 NPC는 같은 구조의 Brain을 가지고, 단지 수치로서 NPC의 아이덴티티를 확립한다. 수치는 MBaseQuest에 기술되어 있음.
// * NPC구조는 크게 AI를 관장하는 ZBrain과 몸체인 ZActor로 구분한다.
// * ZTask란? : NPC의 일련의 행동 구분은 작게 Task로 구분된다. ZTask는 Command 패턴으로 되어있다. Task는 몸체에 달려 있으며, 
//    ZBrain이 Task를 입력해줌으로써 ZActor를 행동하게 한다.
// * ZBehabior란? : NPC 객체의 가장 큰 행동 분류이다. State 패턴으로 되어있다.
//
//
//
//


// sendDelayedMsgToMe







#endif
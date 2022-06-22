#include "stdafx.h"

#ifdef _TDD		// 모든 단위 테스트 관련 코드는 _TDD로 감싸도록 합니다
#include <gtest/gtest.h>
#include "GlobalSubtitutes.h"
#include "ZGameMock.h"
#include "ZActor.h"
#include "ZObjectVMeshMock.h"


class ZActorMock : public ZActor
{
	MQuestNPCInfo m_npcInfo;
	MTD_NPCINFO m_mtdNpcInfo;	// 서버에서 받은 npc 정보

public:
	ZActorMock(IGame* pGame) : ZActor(pGame)
	{
		//strcpy(m_pNPCInfo->szMeshName, "testMesh");
	}

	virtual MQuestNPCInfo* GetQuestNPCInfo(MQUEST_NPC nNPCType)
	{
		return &m_npcInfo;
	}
	virtual MUID GetQuestBossUid()
	{
		return MUID(0,0);
	}
	virtual bool GetQuestCheat(ZQuestCheetType cheat)
	{
		return false;
	}
	virtual const MTD_NPCINFO* GetMyActorServerNPCInfo( const MQUEST_NPC nNPCID )
	{
		// m_npcInfo와 같은 값을 돌려준다
		m_mtdNpcInfo.m_nNPCTID = m_npcInfo.nID;
		m_mtdNpcInfo.m_nMaxHP = m_npcInfo.nMaxHP;
		m_mtdNpcInfo.m_nMaxAP = m_npcInfo.nMaxAP;
		m_mtdNpcInfo.m_nInt = m_npcInfo.nIntelligence;
		m_mtdNpcInfo.m_nAgility = m_npcInfo.nAgility;
		m_mtdNpcInfo.m_fAngle = m_pNPCInfo->fViewAngle;
		m_mtdNpcInfo.m_fDyingTime = m_pNPCInfo->fDyingTime;

		m_mtdNpcInfo.m_fCollisonRadius = m_pNPCInfo->fCollRadius;
		m_mtdNpcInfo.m_fCollisonHight = m_pNPCInfo->fCollHeight;

		m_mtdNpcInfo.m_nAttackType = m_npcInfo.nNPCAttackTypes;
		m_mtdNpcInfo.m_fAttackRange = m_npcInfo.fAttackRange;
		m_mtdNpcInfo.m_nWeaponItemID = m_npcInfo.nWeaponItemID;
		m_mtdNpcInfo.m_fDefaultSpeed = m_pNPCInfo->fSpeed;

		return &m_mtdNpcInfo;
	}
	virtual void AddLandingEffect(rvector vPos, rvector vDir) { }
	virtual void ShockBossGauge(float fDamage) { }
	virtual void LinkAniEventSet(MQUEST_NPC nNPCType) { }
	virtual void AllocObjectVMesh() { m_pVMesh = new ZObjectVMeshMock; }
	virtual float MakeSpeed( float fSpeed) { return fSpeed; }
	virtual float MakePathFindingUpdateTime( char nIntelligence) { return 1.f; }
	virtual float MakeAttackUpdateTime( char nAgility) { return 1.f; }
	virtual float MakeDefaultAttackCoolTime() { return 1.f; }
};

class ZBrainMock : public IBrain
{
public:
	virtual void Init( ZActor* pBody) {}
	virtual void Think( float fDelta) {}
	virtual void OnDamaged() {}

	virtual void OnBody_AnimEnter( ZA_ANIM_STATE nAnimState) {}
	virtual void OnBody_AnimExit( ZA_ANIM_STATE nAnimState) {}
	virtual void OnBody_OnTaskFinished( ZTASK_ID nLastID) {}
};

// 단독으로 ZActor 객체가 생성될 수 있는지 테스트한다 (전역변수들에 의존하는 부분을 제대로 분리했는지)
// 그러나 컴파일이 되는지, 초기화 되지 않은 전역변수들로 인해 크래쉬가 나지 않는지 정도만 체크하는 차원이므로
// 단위 테스트를 입히기 위한 최소 조건을 검증하는 수준이다.
TEST(TestZActor, CreateZActor)
{
	MMatchItemDescMgrMock matchItemDescMgrMock;
	ZTddSetGlobalSub(GI_MatchItemDescMgr, &matchItemDescMgrMock);

	ZGameMock game;

	ZActorMock* pActor = new ZActorMock(&game);
	ZBrainMock* pBrain = new ZBrainMock;
	ZActor* pActorRet = ZActor::CreateActor(NPC_GOBLIN, 1.f, 0, false, pActor, pBrain);
	ASSERT_EQ(pActor, pActorRet);

	pActor->SetMyControl(true);

	pActor->Update(1.f);

	delete pActor;

	ZTddRemoveGlobalSub(GI_MatchItemDescMgr);
}


// ※ 기존 ZActor에서 새로운 퀘스트를 위해 변경되어야 할 점
// - 스킬 갯수 제한을 늘린다 -> 스킬용 애니메이션(nCastingAnimation)이 달랑 4개밖에 없다는 점(ZA_EVENT_SPECIAL1~4)도 수정해야 한다
// - ZBrain이 FSM으로 작성된 AI 패턴을 실행할 수 있어야 한다
// - 애니메이션에 이동이 포함된 경우 실제 월드 위치를 이동해줄 수 있어야 한다 (캐릭터 쪽엔 이미 되어 있는 그것)


#endif
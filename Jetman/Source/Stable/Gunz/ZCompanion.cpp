#include "stdafx.h"
#include "ZCompanion.h"
#include "ZModule_HPAP.h"
#include "ZModule_Resistance.h"
#include "ZModule_FireDamage.h"
#include "ZModule_ColdDamage.h"
#include "ZModule_LightningDamage.h"
#include "ZModule_PoisonDamage.h"
#include "ZEffectManager.h"
#include "../CSCommon/MCompanionDef.h"


ZCompanion::ZCompanion(IGame* pGame) : ZActorBase(pGame)
,m_RealPositionBefore(rvector(0,0,0))
{

	m_bIsNPC = true;
	SetVisible(true);
	m_bInitialized = true;
	m_lastTargetSearch = timeGetTime();
	m_pModule_HPAP = new ZModule_HPAP;
	m_pModule_Resistance = new ZModule_Resistance;
	m_pModule_FireDamage = new ZModule_FireDamage;
	m_pModule_ColdDamage = new ZModule_ColdDamage;
	m_pModule_PoisonDamage = new ZModule_PoisonDamage;
	m_pModule_LightningDamage = new ZModule_LightningDamage;

	AddModule(m_pModule_HPAP);
	AddModule(m_pModule_Resistance);
	AddModule(m_pModule_FireDamage);
	AddModule(m_pModule_ColdDamage);
	AddModule(m_pModule_PoisonDamage);
	AddModule(m_pModule_LightningDamage);
	///TODO: proper logic for this
	shotType = ST_NONE;
	
	//SetPosition(rvector(GetOwner()->GetPosition().x - 100, GetOwner()->GetPosition().y - 20, GetOwner()->GetPosition().z));
}

ZCompanion::~ZCompanion()
{
	if (m_companionDef->IsDrawShadow())
	DestroyShadow();

	RemoveModule(m_pModule_HPAP);
	RemoveModule(m_pModule_Resistance);
	RemoveModule(m_pModule_FireDamage);
	RemoveModule(m_pModule_ColdDamage);
	RemoveModule(m_pModule_PoisonDamage);
	RemoveModule(m_pModule_LightningDamage);

	delete m_pModule_HPAP;
	delete m_pModule_Resistance;
	delete m_pModule_FireDamage;
	delete m_pModule_ColdDamage;
	delete m_pModule_PoisonDamage;
	delete m_pModule_LightningDamage;
}

void ZCompanion::InitProperties(MCompanionDef* companionName,MUID& companionUID)
{
	m_companionDef = companionName;
	AllocObjectVMesh();
	RMesh* pNPCMesh = ZGetNpcMeshMgr()->Get(companionName->GetCompanionModel().c_str());
	if (pNPCMesh)
	{
		if (!pNPCMesh->m_isMeshLoaded)
		{
			ZGetNpcMeshMgr()->Load(companionName->GetCompanionModel().c_str());
			ZGetNpcMeshMgr()->ReloadAllAnimation();
		}
	}

	m_pVMesh->InitNpcMesh(this, companionName->GetCompanionModel().c_str());

	SetUID(companionUID);

	if (companionName->IsDrawShadow())
		CreateShadow();

	m_Collision.SetRadius(companionName->GetCollRadius());
	m_Collision.SetHeight(companionName->GetCollHeight());


	m_pModule_HPAP->SetMaxHP(companionName->GetMaxHP());
	m_pModule_HPAP->SetHP(companionName->GetMaxHP());

	m_pModule_HPAP->SetMaxAP(companionName->GetMaxAP());
	m_pModule_HPAP->SetAP(companionName->GetMaxAP());

	m_pModule_HPAP->SetRealDamage(true);


	m_pVMesh->SetAnimation(ani_mode_lower, "idle", false);

	m_pModule_Movable->Active(false);

	SetTeamID(GetOwner()->GetTeamID());
	SetPosition(rvector(GetOwner()->GetPosition().x - 40, GetOwner()->GetPosition().y - 40, GetOwner()->GetPosition().z ));
	ZGetEffectManager()->AddReBirthEffect(GetPosition());
	ZGetObjectManager()->Add(this);

}

void ZCompanion::OnDraw()
{
	if (!IsVisible())
		return;

	if (!m_bInitialized)
		return;


	Draw_SetLight(GetPosition());

	if (IsDie())
		OnDie();
//	if (!IsDie())
	//	m_pVMesh->SetVisibility(1.f);
//	else
	//	m_pVMesh->SetVisibility(0);

	m_bRendered = m_pVMesh->IsRender();

	m_pVMesh->Render();
}

///TODO: handle attacking
void ZCompanion::OnUpdate(float fDelta)
{
//	if (m_pVMesh) {
	//	m_pVMesh->SetVisibility(1.f);
	//	m_pVMesh->Frame();
//	}	
	if (!IsDie()) {
		float distanceToOwner = Magnitude(GetOwner()->GetPosition() - GetPosition());
		rvector dif = GetOwner()->GetPosition() - GetPosition();
		if (distanceToOwner > 325.f) {
			m_pVMesh->SetAnimation(ani_mode_lower, "run", false);
		}
		else
			m_pVMesh->SetAnimation(ani_mode_lower, "idle", false);
		UpdatePosition(fDelta);
		UpdateHeight(fDelta);
		ProcessMotion(fDelta);

	}

	///TODO: good logic, this is just a test
	if (m_lastTargetSearch + 3000 < timeGetTime()) {
		FindTarget();
		if (targetUID.IsValid())
			ProcessShot(fDelta);
		m_lastTargetSearch = timeGetTime();
	}


}

void ZCompanion::ProcessShot(float fDelta)
{
	float currTime = ZGetGame()->GetTime();
	ZCharacterObject* targetObj = ZGetCharacterManager()->Find(targetUID);
	if (targetObj == nullptr)
		return;

	if (shotType == ST_MELEE) {
		m_pVMesh->SetAnimation(ani_mode_lower, "neko_scratch.elu.ani", false);
		targetObj->OnDamaged(this, this->GetPosition(), ZD_MELEE, MWT_DAGGER, 30, 1, 0);

	}
	else if (shotType == ST_RANGE) {
		targetObj->OnDamaged(this, this->GetPosition(), ZD_BULLET, MWT_REVOLVER, 30, 1);
	}
}

void ZCompanion::OnDamaged(ZObject* pAttacker, rvector srcPos, ZDAMAGETYPE damageType, MMatchWeaponType weaponType, float fDamage, float fPiercingRatio, int nMeleeType)
{
	if (!pAttacker)
		return;

	if (ZGetGameTypeManager()->IsTeamGame(ZGetGame()->GetMatch()->GetMatchType())) {
		if (pAttacker->GetTeamID() == GetTeamID())
			return;
	}
	if (pAttacker == GetOwner())
		return;

	if (damageType == ZDAMAGETYPE::ZD_KATANA_SPLASH) {
		ZCharacterObject* pCObj = MDynamicCast(ZCharacterObject, pAttacker);
		if (pCObj)
		{
			ZC_ENCHANT etype = pCObj->GetEnchantType();
			if (etype == ZC_ENCHANT_LIGHTNING)
			{
				m_pVMesh->SetAnimation(ani_mode_lower, "lightningstun", false);
			}
		}
	}


	ZObject::OnDamaged(pAttacker, srcPos, damageType, weaponType, fDamage, fPiercingRatio, nMeleeType);

}

void ZCompanion::FindTarget()
{
	if (!m_pGame) { _ASSERT(0); return; }

	MUID uidTarget = MUID(0, 0);
	float fDist = FLT_MAX;

	for (auto itor = ZGetObjectManager()->begin(); itor != ZGetObjectManager()->end(); ++itor)
	{
		ZObject* pObj = (*itor).second;
		if (pObj->IsDie())
			continue;

		if (this->GetUID() == pObj->GetUID())
			continue;

		if (GetOwner()->GetUID() == pObj->GetUID())
			continue;
	//	if (pObj->GetTeamID() == GetTeamID())
		//	continue;

		// 거리를 구한다.
		float dist = MagnitudeSq(pObj->GetPosition() - this->GetPosition());

		// 더 가까운 놈이면 이놈을 타겟으로 정한다.
		if (dist < fDist)
		{
			fDist = dist;
			uidTarget = pObj->GetUID();
		}
	}
	targetUID = uidTarget;
}

void ZCompanion::OnDie()
{
	m_pVMesh->SetAnimation(ani_mode_lower,"die", false);
	m_Collision.SetCollideable(false);

	if (IsDie())
	{
#define TRAN_AFTER		3.f		// 이 시간 이후부터
#define VANISH_TIME		2.f		// 이 시간동안 투명해진다

		float fOpacity = max(0.f, min(1.0f, (
			VANISH_TIME - (ZGetGame()->GetTime() - GetDeadTime() - TRAN_AFTER)) / VANISH_TIME));
		m_pVMesh->SetVisibility(fOpacity);
	}
	else
		m_pVMesh->SetVisibility(1.f);
}


void ZCompanion::UpdatePosition(float fDelta)
{
	rvector posDif = GetOwner()->GetPosition() - GetPosition();
	if (posDif.x > 325.f || posDif.y > 325.f) {
		m_pModule_Movable->Move(posDif / 3);
	}
	m_pModule_Movable->Update(fDelta);

	// 착지 순간의 처리
	if (m_pModule_Movable->isLanding())
	{
		//SetFlag(AF_LAND, true);
		//m_Animation.Input(ZA_EVENT_REACH_GROUND);
		SetVelocity(0, 0, 0);

		if (GetPosition().z + 100.f < m_pModule_Movable->GetFallHeight())
		{
			float fSpeed = fabs(GetVelocity().z);

			// 점프 착지시 먼지..
			rvector vPos = GetPosition();
			rvector vDir = rvector(0.f, 0.f, -1.f);
			vPos.z += 50.f;

			RBSPPICKINFO pInfo;
			if (m_pGame->PickWorld(vPos, vDir, &pInfo))
			{
				vPos = pInfo.PickPos;

				vDir.x = pInfo.pInfo->plane.a;
				vDir.y = pInfo.pInfo->plane.b;
				vDir.z = pInfo.pInfo->plane.c;

				ZGetEffectManager()->AddLandingEffect(vPos, vDir);
			}
		}
	}	
}

void ZCompanion::UpdateHeight(float fDelta)
{
	// 발이 뭍혔거나 내려가야 하는데 못간경우
	rvector diff = fDelta*GetVelocity();

	if (IsAir()/* && !m_pModule_Movable->isLanding()*/)
		m_pModule_Movable->UpdateGravity(fDelta);

	bool bUp = (diff.z>0.01f);
	bool bDownward = (diff.z<0.01f);

	if (GetDistToFloor()<0 || (bDownward && m_pModule_Movable->GetLastMove().z >= 0))
	{
		if (GetVelocity().z<1.f && GetDistToFloor()<1.f)
		{
			if (GetVelocity().z<0)
				SetVelocity(GetVelocity().x, GetVelocity().y, 0);
		}
	}

	// 계단 같은곳에서 스르륵 올라오도록..
	if (GetDistToFloor()<0 && !IsDie())
	{
		float fAdjust = 400.f*fDelta;	// 초당 이만큼
		rvector diff = rvector(0, 0, min(-GetDistToFloor(), fAdjust));
		Move(diff);
	}
}

bool ZCompanion::IsDie()
{
	return m_pModule_HPAP->GetHP() <= 0;
}

bool ZCompanion::ProcessMotion(float fDelta)
{
	// 메쉬 애니메이션 업데이트를 담당하는 함수!

	if (!m_pVMesh) return false;
	m_pVMesh->Frame();

	rvector vProxyPosition = GetPosition();
	rvector vProxyDirection = GetDirection();

	D3DXMATRIX world;

	rvector MeshPosition;	// 이동량만큼을 뺀 메쉬 렌더링 위치
	rvector vAnimationPositionDiff(0, 0, 0);

	// 움직임이 있는 애니메이션은 위치조절을 해줘야 한다.
	// 이전 프레임과 현재 프레임에서의 애니메이션 발 위치 변화량을 구해서 실제 액터의 위치를 추가 이동시켜주고
	// 메쉬는 그만큼 오프셋을 반대로 걸어서 렌더링해야 한다.
	//if (IsMovingAnimation())
	{
		MakeWorldMatrix(&world, rvector(0, 0, 0), vProxyDirection, rvector(0, 0, 1));

		// 발의 위치를 로컬 좌표계로 얻어낸다
		rvector footPosition = m_pVMesh->GetFootPosition();
		rvector realPosition = footPosition * world;

		MeshPosition = vProxyPosition - realPosition;

		vAnimationPositionDiff = footPosition - m_RealPositionBefore;	// 이전 프레임과 현재의 발 위치 변화량

																		// 애니메이션의 움직임을 월드의 움직임으로 변환한다
		vAnimationPositionDiff = vAnimationPositionDiff * world;

		m_RealPositionBefore = footPosition;
	}
	//else
	//	MeshPosition = vProxyPosition;

	MakeWorldMatrix(&world, MeshPosition, vProxyDirection, rvector(0, 0, 1));
	m_pVMesh->SetWorldMatrix(world);

	if (IsMyControl())
	{
		// 애니메이션에 포함된 이동량만큼 액터 위치를 이동
		rvector origdiff = fDelta*GetVelocity();

		rvector diff = vAnimationPositionDiff;
		diff.z += origdiff.z;

		if (GetDistToFloor()<0 && diff.z<0) diff.z = -GetDistToFloor();

		Move(diff);
	}


	return true;
}

bool ZCompanion::IsCollideable()
{
	return !IsDie();
}

ZOBJECTHITTEST ZCompanion::HitTest(const rvector& origin, const rvector& to, float fTime, rvector *pOutPos)
{
	return HitTest_Actor(m_companionDef->IsMeshPicking(), origin, to, fTime, pOutPos);
}
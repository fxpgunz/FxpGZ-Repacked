#include "stdafx.h"
#include "ZActorWithFSM.h"
#include "MActorDef.h"
#include "ZFSMManager.h"
#include "ZFSM.h"
#include "ZFSMState.h"
#include "ZActorActionManager.h"
#include "ZActorAction.h"
#include "ZNavigationMesh.h"
#include "ZScreenEffectManager.h"
#include "ZModule_HPAP.h"
#include "ZModule_Resistance.h"
#include "ZModule_FireDamage.h"
#include "ZModule_ColdDamage.h"
#include "ZModule_LightningDamage.h"
#include "ZModule_PoisonDamage.h"
#include "../CSCommon/Blitz.h"
MImplementRTTI(ZActorWithFSM, ZActorBase);

static float currTime = 0.f;
ZActorWithFSM::ZActorWithFSM(IGame* pGame, ZActorActionManager* pActionMgr)
: ZActorBase(pGame)
, m_pActionMgr(pActionMgr)
, m_pActorDef(NULL)
, m_pFsm(NULL)
, m_pCurrState(NULL)
, m_pCurrAction(NULL)
, m_pCurrActionStartTime(0)
, m_uidTarget(0,0)
, m_fGroggy(0)
, m_bPostedActionExecute(false)
, m_bPostedRangeShotInAction(false)
, m_bMoving(false)
, m_bSuffering(false)
, m_RealPositionBefore(0,0,0)
, m_bMyControl(false)
, m_nLastTimePostBasicInfo(0)
, m_teamID(MMT_BLUE)
, m_routeID(0)
, m_wayPointReached(0)
, m_summonCount(0)
, m_actorName("")
, m_bIsStationary(false)
{
	m_bIsNPC = true;
	m_speedAccel = 0;

	SetVisible(true);
	m_bInitialized = true;
	m_pModule_HPAP				= new ZModule_HPAP;
	m_pModule_Resistance		= new ZModule_Resistance;
	m_pModule_FireDamage		= new ZModule_FireDamage;
	m_pModule_ColdDamage		= new ZModule_ColdDamage;
	m_pModule_PoisonDamage		= new ZModule_PoisonDamage;
	m_pModule_LightningDamage	= new ZModule_LightningDamage;

	AddModule(m_pModule_HPAP);
	AddModule(m_pModule_Resistance);
	AddModule(m_pModule_FireDamage);
	AddModule(m_pModule_ColdDamage);
	AddModule(m_pModule_PoisonDamage);
	AddModule(m_pModule_LightningDamage);

	// �̵� ����� ���� ������Ʈ �� ���̹Ƿ� ��Ȱ�� ���·� ������ ZModuleContainer::UpdateModules()���� �ߺ� update���� �ʴ´�
	m_pModule_Movable->Active(false);

}

ZActorWithFSM::~ZActorWithFSM()
{
	EmptyHistory();
	if (!m_pActorDef->IsNoShadow())
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

void ZActorWithFSM::InitMesh(char* szMeshName)
{
	//_ASSERT(m_pVMesh == NULL);

	//AllocObjectVMesh();
	//m_pVMesh->InitNpcMesh(this, szMeshName);
}

void ZActorWithFSM::OnDraw()
{
	if (!IsVisible())
		return;

	if (!m_bInitialized)
		return;


	Draw_SetLight(GetPosition() );

//	RGetDevice()->SetRenderState(D3DRS_LIGHTING, TRUE);
	
	if (!IsDie())
		m_pVMesh->SetVisibility(1.f);

	m_bRendered = m_pVMesh->IsRender();

	m_pVMesh->Render();
}

void ZActorWithFSM::OnUpdate( float fDelta )
{
	//todok ������ ������ �����ؾ��Ѵ�. ���� �׾ �Ⱥ��̴� �� ������Ʈ�� ��� ���ư��� ����
	if (ZGetGame()->IsReplay())
	{
		UpdateVelocity(fDelta);
		UpdatePosition(fDelta);
		UpdateHeight(fDelta);
		UpdateGroggy(fDelta);

		ProcessMotion(fDelta);

		ProcessShotInAction(fDelta);
		return;
	}

	if (IsMyControl())
	{
		if (timeGetTime() - m_nLastUpdate >= 10)
		{
			m_nLastUpdate = timeGetTime();
			UpdateFsm(fDelta);
		}
		PostBasicInfo();

		if (!IsStationary())
		{
			UpdateVelocity(fDelta);
			UpdatePosition(fDelta);
			UpdateHeight(fDelta);
		}
		if (IsStationary() && IsAir())
		{
			UpdateVelocity(fDelta);
			UpdatePosition(fDelta);
			UpdateHeight(fDelta);
		}

		UpdateGroggy(fDelta);
	}
	///Idk if i shoudl use this or not
	//
	//else
	//{
	//	// ���� �����ϴ� npc�� �ƴ� ���� �ܼ��ϰ� ���� velocity ������� ��ġ ������Ʈ
	//	m_pModule_Movable->Update(fDelta);
	//	if (!IsStationary())
	//		UpdateHeight(fDelta);
	//}

	ProcessMotion(fDelta);

	ProcessShotInAction(fDelta);
}

void ZActorWithFSM::SetMyControl(bool bMyControl)
{
	m_bMyControl = bMyControl;
}

bool ZActorWithFSM::IsMyControl()
{
	return m_bMyControl;
}

bool ZActorWithFSM::IsMovingAnimation()
{
	/*if (!m_pCurrState) return false;
	IActorAction* pAction = m_pCurrState->GetAction();
	if (!pAction) return false;
	return pAction->IsMovingAnimation();*/

	if (!m_pCurrAction) return false;
	return m_pCurrAction->IsMovingAnimation();
}

bool ZActorWithFSM::IsCollideable()
{
	if (!m_Collision.IsCollideable())
		return false;

	return !IsDie();
}

bool ZActorWithFSM::IsDie() 
{ 
	if (!m_pFsm) { _ASSERT(0); return true; }

	if (m_pFsm->GetState(FSM_BISTATE_DIE) == m_pCurrState)
		return true;

	if (m_pFsm->GetState(FSM_BISTATE_WAIT_DELETE) == m_pCurrState)
		return true;

	return false;
}

bool ZActorWithFSM::InitWithActorDef( MActorDef* pActorDef, ZFSMManager* pFsmMgr )
{
	m_pActorDef = pActorDef;

	if (m_pActorDef == nullptr)
		return false;
	// init mesh
	_ASSERT(m_pVMesh == NULL);
	AllocObjectVMesh();
	m_pVMesh->InitNpcMesh(this, m_pActorDef->GetModelName());
	if (pActorDef->GetScale() > 0.f)
		m_pVMesh->SetScale(m_pVMesh->GetScale() * m_pActorDef->GetScale());

	if (m_pActorDef->IsNoShadow() == false)
		CreateShadow();

	m_Collision.SetHeight(m_pActorDef->GetCollisionHeight() + (m_pActorDef->GetForcedCollup120() ? 120 : 0));
	m_Collision.SetRadius(m_pActorDef->GetCollisionRadius());

	// init hp ap
	m_pModule_HPAP->SetMaxHP(pActorDef->GetMaxHp());
	m_pModule_HPAP->SetHP(pActorDef->GetMaxHp());

	m_pModule_HPAP->SetMaxAP(pActorDef->GetMaxAp());
	m_pModule_HPAP->SetAP(pActorDef->GetMaxAp());
	
	m_pModule_HPAP->SetRealDamage(true);

	// init fsm
	m_pFsm = pFsmMgr->GetFsm( pActorDef->GetFsmName());
	if (!m_pFsm)
	{
		_ASSERT(0);
		return false;
	}

	m_pCurrState = m_pFsm->GetState( m_pFsm->GetEntryStateName());
	if (!m_pCurrState)
	{
		_ASSERT(0);
		return false;
	}
	OnEnterState();

	return true;
}

bool ZActorWithFSM::ProcessMotion(float fDelta)
{
	// �޽� �ִϸ��̼� ������Ʈ�� ����ϴ� �Լ�!
	if (!m_pVMesh) return false;
	m_pVMesh->Frame();

	rvector vProxyPosition = GetPosition();
	rvector vProxyDirection = GetDirection();

	D3DXMATRIX world;

	rvector MeshPosition;	// �̵�����ŭ�� �� �޽� ������ ��ġ
	rvector vAnimationPositionDiff(0,0,0);

	// �������� �ִ� �ִϸ��̼��� ��ġ������ ����� �Ѵ�.
	// ���� �����Ӱ� ���� �����ӿ����� �ִϸ��̼� �� ��ġ ��ȭ���� ���ؼ� ���� ������ ��ġ�� �߰� �̵������ְ�
	// �޽��� �׸�ŭ �������� �ݴ�� �ɾ �������ؾ� �Ѵ�.
	if(IsMovingAnimation())
	{
		MakeWorldMatrix(&world,rvector(0,0,0),vProxyDirection,rvector(0,0,1));

		// ���� ��ġ�� ���� ��ǥ��� ����
		rvector footPosition = m_pVMesh->GetFootPosition();
		rvector realPosition = footPosition * world;

		MeshPosition = vProxyPosition - realPosition ;

		vAnimationPositionDiff = footPosition - m_RealPositionBefore;	// ���� �����Ӱ� ������ �� ��ġ ��ȭ��

		// �ִϸ��̼��� �������� ������ ���������� ��ȯ�Ѵ�
		vAnimationPositionDiff = vAnimationPositionDiff * world;

		m_RealPositionBefore = footPosition;
	}
	else
		MeshPosition = vProxyPosition;

	MakeWorldMatrix(&world, MeshPosition, vProxyDirection, rvector(0,0,1));
	m_pVMesh->SetWorldMatrix(world);

	if (!IsMyControl())
		UpdatePosition(fDelta);

	if(IsMyControl() && IsMovingAnimation())
	{
		// �ִϸ��̼ǿ� ���Ե� �̵�����ŭ ���� ��ġ�� �̵�
		rvector origdiff=fDelta*GetVelocity();

		rvector diff = vAnimationPositionDiff;
		diff.z+=origdiff.z;

		if(GetDistToFloor()<0 && diff.z<0) diff.z=-GetDistToFloor();

		Move(diff);
	}

	return true;
}

void ZActorWithFSM::UpdateVelocity(float fDelta)
{
	///TODO: fix ismovinganimation?
	if (IsAir())
	{
		m_pModule_Movable->UpdateGravity(fDelta);
	}
	//if ( CheckFlag(AF_MOVING) && CheckFlag(AF_LAND) &&
	//	((GetCurrAni() == ZA_ANIM_WALK) || (GetCurrAni() == ZA_ANIM_RUN)))

	// npc�� ������� ���� ������ �� �ִ� ��Ȳ�ΰ�
	if (!IsSuffering() && !IsAir())
	{
		// �޸��� �;��ϴ� ���ΰ�
		if (IsMoving())
		{
			// ����ó��
			const float fAccel = 10000.f;
			AddVelocity(m_Direction * (fAccel * fDelta));

			float fSpeed = GetSpeedAccel() > 1 ? GetSpeedAccel() : m_pActorDef->GetSpeed();

			rvector vel = GetVelocity();
			if (Magnitude(vel) > fSpeed)
			{
				Normalize(vel);
				vel *= fSpeed;
				SetVelocity(vel);
			}
		}
		else
		{
			// ����ó��
			rvector vVelocity = GetVelocity();
			vVelocity.x = 0;
			vVelocity.y = 0;
			SetVelocity(vVelocity);
		}
	}
}

void ZActorWithFSM::UpdatePosition(float fDelta)
{
	//todok Ư���ǰ�
/*	if( CheckFlag(AF_MY_CONTROL) ) 
	{
		if ((CheckFlag(AF_BLAST) == true) && (GetCurrAni() == ZA_ANIM_BLAST) && (GetVelocity().z < 0.0f))
		{
			m_Animation.Input(ZA_EVENT_REACH_PEAK);
		}

		if ((CheckFlag(AF_BLAST_DAGGER) == true) && (GetCurrAni() == ZA_ANIM_BLAST_DAGGER) )
		{
			if( Magnitude(GetVelocity()) < 20.f )
			{
				m_Animation.Input(ZA_EVENT_REACH_GROUND_DAGGER);
				SetFlag(AF_BLAST_DAGGER,false);
			}
		}
	}
*/

	////todok del
	//if (IsSuffering())
	//{
	//	mlog("suffering vel:%f %f %f\n", GetVelocity().x, GetVelocity().y, GetVelocity().z);
	//}

	//todok ������ �ΰ��� �κ��̴�.  �̰��� ���� update�Ѵٴ� ���� UpdateModule���� �ߺ�������� �ʵ���
	// m_pModule_Movable->Active(false)�� �صдٴ� ���̴�. ���� �Ǿ ��Ʈ���ϴ� npc�� ��쿣 ��� �� ������ �����ؾ� �Ѵ�.
	m_pModule_Movable->Update(fDelta);

	////todok del
	//if (IsSuffering())
	//{
	//	mlog("suffering vel:%f %f %f\n", GetVelocity().x, GetVelocity().y, GetVelocity().z);
	//}

	//todok del
	/*if (m_pModule_Movable->isLanding()) {
		mlog("actor isLanding = true, position.z = %f, distToFloor = %f (%f)\n", GetPosition().z, GetDistToFloor(), fDelta);
	}*/

	// ���� ������ ó��
	if(m_pModule_Movable->isLanding())
	{
		//SetFlag(AF_LAND, true);
		//m_Animation.Input(ZA_EVENT_REACH_GROUND);
		SetVelocity(0,0,0);

		if(GetPosition().z + 100.f < m_pModule_Movable->GetFallHeight())
		{
			float fSpeed=fabs(GetVelocity().z);

			// ���� ������ ����..
			rvector vPos = GetPosition();
			rvector vDir = rvector(0.f,0.f,-1.f);
			vPos.z += 50.f;

			RBSPPICKINFO pInfo;
			if (m_pGame->PickWorld(vPos, vDir, &pInfo))
			{
				vPos = pInfo.PickPos;

				vDir.x = pInfo.pInfo->plane.a;
				vDir.y = pInfo.pInfo->plane.b;
				vDir.z = pInfo.pInfo->plane.c;

				AddLandingEffect(vPos, vDir);
			}
		}
	}
}

void ZActorWithFSM::UpdateHeight(float fDelta)
{
	// ���� �����ų� �������� �ϴµ� �������
	rvector diff=fDelta*GetVelocity();

	bool bUp = (diff.z>0.01f);
	bool bDownward= (diff.z<0.01f);

	if(GetDistToFloor()<0 || (bDownward && m_pModule_Movable->GetLastMove().z>=0))
	{
		if(GetVelocity().z<1.f && GetDistToFloor()<1.f)
		{
			if(GetVelocity().z<0)
				SetVelocity(GetVelocity().x,GetVelocity().y,0);
		}
	}

	// ��� ���������� ������ �ö������..
	if(GetDistToFloor()<0 && !IsDie())
	{
		float fAdjust=400.f*fDelta;	// �ʴ� �̸�ŭ
		rvector diff=rvector(0,0,min(-GetDistToFloor(),fAdjust));
		Move(diff);
	}
}

void ZActorWithFSM::UpdateGroggy(float fDelta)
{
	// �׷α��� �ڿ� ȸ���� ����
	if (!m_pActorDef) { _ASSERT(0); return; }
	float fRecoverSpeed = m_pActorDef->GetGroggyRecoverySpeed();
	m_fGroggy -= fRecoverSpeed * fDelta;
	if (m_fGroggy < 0)
		m_fGroggy = 0;

	//todok del
	//mlog("��ACTOR_GROGGY : %f\n", m_fGroggy);
}

ZFSMState* ZActorWithFSM::CheckGlobalTransition()
{
	/* todok ���� ���̴� fsm ���� xml���� �������� �ϵ��� �Ѵ�.
	// todok ���� ������ ���� �뺸�ϴ� ���� func���� �����ؾ� �Ѵ�.
	// �켱 ���� �˻����.. todok ���⿡ ���� ���� �Լ��� ���� �ű���
	ZFSMState* pDieState = m_pFsm->GetState(FSM_BISTATE_DIE);
	if (m_pCurrState != pDieState)
	{
		if (Cond_GetHp() <= 0)
		{
			//todok ������ ������ �Ѵ�. ���� ���� ���Խ� �ٷ� ������ �ٵ� �̰� enter function���� �����ؾ� �ϳ�?
			//ZPostQuestRequestNPCDead(uidAttacker, GetUID(), GetPosition());
			return pDieState;
		}
	}*/
	return NULL;
}

void ZActorWithFSM::SetAnimationInAction(IActorAction* pAction)
{
	if (!pAction) return;

	const char* szAnimName = pAction->GetAnimationName();
	if (!m_pVMesh->SetAnimation(ani_mode_lower, szAnimName, false))
		_ASSERT(0);

}

void ZActorWithFSM::UpdateFsm(float fDelta)
{
	if (!IsMyControl()) return;
	if (!m_pFsm) { _ASSERT(0); return; }
	if (!m_pCurrState) { _ASSERT(0); return; }

	if (ZGetGame()->GetMatch()->GetRoundState() == MMATCH_ROUNDSTATE_FINISH)
		return;

	ValidateTarget();

	// ���� �˻�
	ZFSMState* pNextState = NULL;

	pNextState = CheckGlobalTransition();
	if (!pNextState)
		///TODO: check what's taking up so much time in this!
		pNextState = m_pCurrState->CheckTransition(this);

	if (pNextState)
	{
		OnExitState();
		m_pCurrState = pNextState;
		OnEnterState();
	}

	// �׼� ����
	IActorAction* pAction = m_pCurrState->GetAction();
	if (pAction)
	{
		SetAnimationInAction(pAction);
	}

	// function ����
	int numFunc = m_pCurrState->GetFunctionCallCount();
	for (int i=0; i<numFunc; ++i)
	{
		const ZFSMFunctionCall* pFunc = m_pCurrState->GetFunctionCall(i);
		///TODO: check what's taking up somuch time in this!
		ExecuteFunction(fDelta, pFunc->id, pFunc->nArg[0], pFunc->nArg[1], pFunc->nArg[2], pFunc->nArg[3]);
	}
}

void ZActorWithFSM::ProcessShotInAction(float fDelta)
{
	if (!m_pCurrAction) return;

	float fCurrTime = ZGetGame()->GetTime();

	int numMeleeShot = m_pCurrAction->GetNumMeleeShot();
	for (int i = 0; i < numMeleeShot; ++i)
	{
		const ZActorActionMeleeShot* pMeleeShot = m_pCurrAction->GetMeleeShot(i);
		if (pMeleeShot)
		{
			if (fCurrTime >= m_pCurrActionStartTime + pMeleeShot->GetDelay() &&
				!IsProcessedShot(pMeleeShot))
			{
				ZCharacterObject* pObj = dynamic_cast<ZCharacterObject*>(ZGetObjectManager()->GetObject(m_uidTarget));

				pMeleeShot->ProcessShot(ZGetGame(), m_UID, fCurrTime);
				SetProcessedShot(pMeleeShot);
			}
		}
	}

	int numRangeShot = m_pCurrAction->GetNumRangeShot();
	for (int i=0; i<numRangeShot; ++i)
	{
		const ZActorActionRangeShot* pRangeShot = m_pCurrAction->GetRangeShot(i);
		if (pRangeShot)
		{
			if (fCurrTime >= m_pCurrActionStartTime + pRangeShot->GetDelay() &&
				!IsProcessedShot(pRangeShot))
			{
				rvector vDir,vPos;
				rmatrix mat;
				ZCharacterObject* pChar = dynamic_cast<ZCharacterObject*>(ZGetObjectManager()->GetObject(m_uidTarget));
				if (pChar == NULL)
					vDir = GetDirection() + pRangeShot->GetDirectionMod();
				else
				{
					if (pChar->GetPosition().z >= GetPosition().z)
						vDir = (pChar->GetPosition() - GetPosition()) + pRangeShot->GetDirectionMod();
					else
						vDir = -((GetPosition() - pChar->GetPosition()) + pRangeShot->GetDirectionMod());
				}

				if (pRangeShot->GetYAxis() != 0) {
					D3DXMatrixRotationY(&mat, ToRadian(pRangeShot->GetYAxis()));
					vDir = vDir * mat;
				}
				if (pRangeShot->GetZAxis() != 0) {
					D3DXMatrixRotationZ(&mat, ToRadian(pRangeShot->GetZAxis()));
					vDir = vDir * mat;
				}

				Normalize(vDir);

				if (pRangeShot->GetDirTarget()) {
					Func_FaceToTarget();
				}



				_RMeshPartsPosInfoType posPartsType = pRangeShot->GetPosPartsType();
				if (posPartsType == eq_parts_pos_info_end)
					vPos = GetPosition();
				else
					vPos = m_pVMesh->GetBipTypePosition(posPartsType);

				pRangeShot->ProcessShot(vPos, vDir, this);
				SetProcessedShot(pRangeShot);
			}
		}
	}
	int numGrenadeShot = m_pCurrAction->GetNumGrenadeShot();
	for (int i = 0; i < numGrenadeShot; ++i)
	{
		const ZActorActionGrenadeShot* pGrenadeShot = m_pCurrAction->GetGrenadeShot(i);
		if (pGrenadeShot)
		{
			if (fCurrTime >= m_pCurrActionStartTime + pGrenadeShot->GetDelay() &&
				!IsProcessedShot(pGrenadeShot))
			{
				rvector vDir;
				ZObject* pObj = ZGetObjectManager()->GetObject(m_uidTarget);
				if (pObj)
				{
					if (pObj->GetPosition().z >= GetPosition().z)
						vDir = (pObj->GetPosition() - GetPosition()) + pGrenadeShot->GetDirMod();
					else
						vDir = -((GetPosition() - pObj->GetPosition()) + pGrenadeShot->GetDirMod());
				}
				else
					vDir = GetDirection() + pGrenadeShot->GetDirMod();



				rmatrix mat;
				if (pGrenadeShot->GetYAxis() != 0)
				{
					D3DXMatrixRotationY(&mat, ToRadian(pGrenadeShot->GetYAxis()));
					vDir = vDir * mat;
				}
				if (pGrenadeShot->GetZAxis() != 0)
				{
					D3DXMatrixRotationZ(&mat, ToRadian(pGrenadeShot->GetZAxis()));
					vDir = vDir * mat;
				}

				///why would youw ant them thrown at the ground?
				if (vDir.z < 0)
					vDir.z = vDir.z * -1;

				Normalize(vDir);

	


				rvector vPos;
				_RMeshPartsPosInfoType posPartsType = pGrenadeShot->GetPosPartsType();
				if (posPartsType == eq_parts_pos_info_end)
					vPos = GetPosition() + pGrenadeShot->GetPosMod();
				else
					vPos = m_pVMesh->GetBipTypePosition(posPartsType) + pGrenadeShot->GetPosMod();

				pGrenadeShot->ProcessShot(vPos, rvector(vDir.x * pGrenadeShot->GetForce(),vDir.y * pGrenadeShot->GetForce(),vDir.z * pGrenadeShot->GetForce()),this);
				SetProcessedShot(pGrenadeShot);
			}
		}
	}
	//summon
	int nNumSummon = m_pCurrAction->GetNumSummon();
	for (int i = 0; i < nNumSummon; ++i)
	{
		const ZActorActionSummon* pSummon = m_pCurrAction->GetSummon(i);
		if (pSummon && fCurrTime >= m_pCurrActionStartTime + pSummon->GetDelay() && !IsProcessedSummon(pSummon))
		{
			rvector dir = GetDirection();
			dir.z = 0;
			D3DXMATRIX mat;
			D3DXMatrixRotationZ(&mat, ToRadian(pSummon->GetAngle()));
			dir = dir*mat;
			Normalize(dir);

			rvector pos = GetPosition() + (dir * pSummon->GetRange());

			pSummon->ProcessSummon(m_uidTarget, m_UID, i, pSummon->GetName(), pos, dir, pSummon->GetRoute());
			SetProcessedSummon(pSummon);
			m_summonCount++;
		}
	}

	//sound
	int numSound = m_pCurrAction->GetNumSound();
	for (int i = 0; i < numSound; ++i)
	{
		const ZActorActionSound* pSound = m_pCurrAction->GetSound(i);
		if (pSound && fCurrTime >= m_pCurrActionStartTime + pSound->GetDelay() && !IsProcessedSound(pSound))

		{
			pSound->ProcessSound(GetPosition(), pSound->GetName());
			SetProcessedSound(pSound);

		}
	}

	//effect
	int numEffect = m_pCurrAction->GetNumEffect();
	for (int i = 0; i < numEffect; ++i)
	{
		const ZActorActionEffect* pEffect = m_pCurrAction->GetEffect(i);
		if (pEffect && fCurrTime >= m_pCurrActionStartTime + pEffect->GetDelay() && !IsProcessedEffect(pEffect))
		{

			rvector vDir = GetDirection() + pEffect->GetDirMod();
			rmatrix mat;
			if (pEffect->GetZAxis() > 0)
			{
				D3DXMatrixRotationZ(&mat, ToRadian(pEffect->GetZAxis()));
				vDir = vDir * mat;
			}
			Normalize(vDir);

			rvector vPos;
			_RMeshPartsPosInfoType partsType = pEffect->GetPosPartsType();
			if (partsType != _RMeshPartsPosInfoType::eq_parts_pos_info_end)
				vPos = m_pVMesh->GetBipTypePosition(partsType) + pEffect->GetPosMod();
			else
				vPos = GetPosition() + pEffect->GetPosMod();
			pEffect->ProcessEffect(vPos, vDir, pEffect->GetScale(), pEffect->GetMeshName(), GetUID(),pEffect->GetDelay());
			SetProcessedEffect(pEffect);
		}
	}
}

void ZActorWithFSM::ValidateTarget()
{
	// �������ų� ���� ��� Ÿ���� �����Ѵ�
	ZCharacterObject* pChar = (ZCharacterObject*)ZGetObjectManager()->GetObject(m_uidTarget);
	//dont want the treasure chests getting attacked.
	if (!pChar || pChar->IsDie()|| pChar->GetTeamID() == GetTeamID() || (pChar->GetTeamID() == MMT_ALL && pChar->IsNPC()))
	{
		m_uidTarget.SetInvalid();
		return;
	}
}

void ZActorWithFSM::PostBasicInfo()
{

	if (ZGetGame()->IsReplay())
		return;
	DWORD nNowTime = timeGetTime();
	if (GetInitialized() == false) return;

	// Dont send info 5 seconds after the npc dies.
	if(IsDie() && m_pGame->GetTime() - GetDeadTime()>5.f) return;
	int nMoveTick = 25;///TODO: figure out owhat's making this so CPU intensive???? -__-

	if ((int)(nNowTime - m_nLastTimePostBasicInfo) >= nMoveTick)
	{
		m_nLastTimePostBasicInfo = nNowTime;

		ZACTOR_WITHFSM_BASICINFO pbi;
		//pbi.fTime = m_pGame->GetTime();//todok del
		pbi.uidNPC = GetUID();

		pbi.posx = GetPosition().x;
		pbi.posy = GetPosition().y;
		pbi.posz = GetPosition().z;
		/*pbi.posx = (int(GetPosition().x * 0.1f)) * 10.f;
		pbi.posy = (int(GetPosition().y * 0.1f)) * 10.f;
		pbi.posz = (int(GetPosition().z * 0.1f)) * 10.f;*/	// ������ ���� ���������� ���� �ϱ� ���� 1�� �ڸ����� ������ ����

		pbi.velx = GetVelocity().x;
		pbi.vely = GetVelocity().y;
		pbi.velz = GetVelocity().z;

		pbi.dirx = GetDirection().x*32000.0f;
		pbi.diry = GetDirection().y*32000.0f;
		pbi.dirz = GetDirection().z*32000.0f;

		pbi.nActionIndex = -1;
		if (m_pCurrAction)
			pbi.nActionIndex = m_pActionMgr->GetIndexOfAction(m_pCurrAction);


		ZPOSTCMD1(MC_NEWQUEST_PEER_NPC_BASICINFO, MCommandParameterBlob(&pbi,sizeof(ZACTOR_WITHFSM_BASICINFO)));


		// history �� ����
#define ACTOR_HISTROY_COUNT 100
		ZBasicInfoItem *pitem=new ZBasicInfoItem;
		pitem->info.position = GetPosition();
		pitem->info.direction = GetDirection();
		pitem->info.velocity = GetVelocity();

		pitem->fSendTime = pitem->fReceivedTime = m_pGame->GetTime();
		m_BasicHistory.push_back(pitem);

		while(m_BasicHistory.size()>ACTOR_HISTROY_COUNT)
		{
			delete *m_BasicHistory.begin();
			m_BasicHistory.pop_front();
		}
	}
}

void ZActorWithFSM::OnBasicInfo(ZACTOR_WITHFSM_BASICINFO* pbi)
{
	if (!IsMyControl())
	{
		SetPosition(rvector(pbi->posx, pbi->posy, pbi->posz));
		SetVelocity(rvector(pbi->velx, pbi->vely, pbi->velz));
		SetDirection(1.f / 32000.f * rvector(pbi->dirx, pbi->diry, pbi->dirz));

		IActorAction* pAction = m_pActionMgr->GetActionByIndex(pbi->nActionIndex);
		m_pCurrAction = pAction;
		if (m_pCurrAction == nullptr)
			return;
		SetAnimationInAction(pAction);
	}
}

void ZActorWithFSM::OnExitState()
{
	// Ż�� func ����
	int numExitFunc = m_pCurrState->GetExitFunctionCallCount();
	for (int i=0; i<numExitFunc; ++i)
	{
		const ZFSMFunctionCall* pFunc = m_pCurrState->GetExitFunctionCall(i);
		ExecuteFunction(0, pFunc->id, pFunc->nArg[0], pFunc->nArg[1], pFunc->nArg[2], pFunc->nArg[3]);
	}

	// �̹� ���¿��� �ѵ� �̵��� �÷��װ� ���� �� ������ ����
	SetMoving(false);
}

void ZActorWithFSM::OnEnterState()
{
	m_stateEnteredTimeStamper.SetEnteredTime(m_pCurrState, timeGetTime());

	// ���� func ����
	int numEnterFunc = m_pCurrState->GetEnterFunctionCallCount();
	for (int i=0; i<numEnterFunc; ++i)
	{
		const ZFSMFunctionCall* pFunc = m_pCurrState->GetEnterFunctionCall(i);
		ExecuteFunction(0, pFunc->id, pFunc->nArg[0], pFunc->nArg[1], pFunc->nArg[2], pFunc->nArg[3]);
	}

	m_bPostedActionExecute = false;
	m_bPostedRangeShotInAction = false;
	m_RealPositionBefore = rvector(0,0,0);

	// �׼� ������ �˷��� �� peer�� �� �׼ǿ� ���Ե� ������ ó���ϵ��� �Ѵ�
	int nActionIndex = -1;
	IActorAction* pAction = m_pCurrState->GetAction();
	if (pAction)
		nActionIndex = m_pActionMgr->GetIndexOfAction(pAction);
	ZPostNewQuestPeerNpcActionExecute(m_UID, nActionIndex);
}

void ZActorWithFSM::RotateToDirection(rvector vTargetDir, float fFrameDelta)
{
	float fRotSpeed = 1.0f;
	if (m_pActorDef)
		fRotSpeed = m_pActorDef->GetRotSpeed();

	rmatrix mat;
	rvector vMyDir = GetDirection();
	float fAngle=GetAngleOfVectors(vTargetDir, vMyDir);
	float fRotAngle = fFrameDelta * fRotSpeed;

	if (fAngle > 0.0f) fRotAngle = -fRotAngle;
	if (fabs(fRotAngle) > fabs(fAngle)) 
	{
		fRotAngle = -fAngle;
	}
	D3DXMatrixRotationZ(&mat, fRotAngle);

	SetDirection(vMyDir * mat);
}



void ZActorWithFSM::AddLandingEffect( rvector vPos, rvector vDir )
{
	ZGetEffectManager()->AddLandingEffect(vPos,vDir);
}

ZOBJECTHITTEST ZActorWithFSM::HitTest( const rvector& origin, const rvector& to, float fTime, rvector *pOutPos )
{
	//todok ������ ��� �޽���ŷ�ϵ��� �ؾ� �Ѵ�. def xml �� �޽���ŷ ���θ� ������ �� �ֵ��� �ϱ�
	return HitTest_Actor(m_pActorDef->GetMeshPicking(), origin, to, fTime, pOutPos);
}

void ZActorWithFSM::OnBlast(rvector &dir)
{
	if (!IsMyControl()) return;
	if (!m_pActorDef) { _ASSERT(0); return; }
	if (m_pActorDef->IsNeverBlasted()) return;
	
	if (!m_pFsm) return;
	ZFSMState* pStateBlastedSoar = m_pFsm->GetState(FSM_BISTATE_BLASTEDSOAR);
	if (!pStateBlastedSoar) { _ASSERT(0); return; }

	m_blastedDir = dir;

	OnExitState();
	m_pCurrState = pStateBlastedSoar;
	OnEnterState();
}

void ZActorWithFSM::OnBlastDagger(rvector &dir,rvector& pos)
{
	if (!IsMyControl()) return;
	if (!m_pActorDef) { _ASSERT(0); return; }
	if (m_pActorDef->IsNeverBlasted()) return;

	if (!m_pFsm) return;
	ZFSMState* pStateBlastedThrust = m_pFsm->GetState(FSM_BISTATE_BLASTEDTHRUST);
	if (!pStateBlastedThrust) { _ASSERT(0); return; }

	m_daggerBlastedDir = dir;
	m_daggerBlastedPos = pos;

	OnExitState();
	m_pCurrState = pStateBlastedThrust;
	OnEnterState();
}

void ZActorWithFSM::OnPeerActionExecute(int nActionIndex)
{
	m_pCurrAction = NULL;

	if (!m_pActionMgr) { _ASSERT(0); return; }

	IActorAction* pAction = m_pActionMgr->GetActionByIndex(nActionIndex);
	if (!pAction) { return; }

	m_pCurrAction = pAction;
	m_pCurrActionStartTime = ZGetGame()->GetTime();
	m_setProcessedShotInAction.clear();
	m_setProcessedEffectInAction.clear();
	m_setProcessedSoundInAction.clear();
	m_setProcessedSummonInAction.clear();
}

void ZActorWithFSM::OnDamaged(ZObject* pAttacker, rvector srcPos, ZDAMAGETYPE damageType, MMatchWeaponType weaponType, float fDamage, float fPiercingRatio, int nMeleeType)
{
	if (!pAttacker)
		return;

	if (pAttacker->GetTeamID() == GetTeamID())
		return;
	if (IsMyControl())
	{
		// ����Ʈ�� �������� ���
		if (damageType==ZD_KATANA_SPLASH)
		{
			ZCharacterObject* pCObj = MDynamicCast(ZCharacterObject, pAttacker);
			if (pCObj)
			{
				ZC_ENCHANT etype = pCObj->GetEnchantType();
				if( etype == ZC_ENCHANT_LIGHTNING )
				{
					OnDamaged_LightningSplash();
				}
			}
		}

		// �׷α���� ����
		m_fGroggy += fDamage;
		//todok del
		//mlog("��ACTOR_GROGGY : %f\n", m_fGroggy);
	}
	///TODO: figure out what the checkdelay does in the xml
	if (_stricmp(this->GetActorDef()->GetActorType(), "barricade") == 0 ||
		_stricmp(this->GetActorDef()->GetActorType(), "radar") == 0)
	{
		fDamage = fDamage - (fDamage * MGetBlitz()->GetBuildingEvents().damageReduction);
	}

	ZObject::OnDamaged(pAttacker,srcPos,damageType,weaponType,fDamage,fPiercingRatio,nMeleeType);
	ZCharacter* pCharacter = dynamic_cast<ZCharacter*>(pAttacker);
	if (pAttacker && !pAttacker->IsNPC())
	{
		pCharacter->SetLastTarget(this->GetUID());
	}
}

void ZActorWithFSM::OnDamaged_LightningSplash()
{
	if (!IsMyControl()) return;
	if (!m_pFsm) { _ASSERT(0); return; }
	
	ZFSMState* pStatetLightningStun = m_pFsm->GetState(FSM_BISTATE_LIGHTNINGSTUN);
	if (!pStatetLightningStun) { _ASSERT(0); return; }

	OnExitState();
	m_pCurrState = pStatetLightningStun;
	OnEnterState();

	SetVelocity(0,0,0);
};

//////////////////////////////////////////////////////////////////////////
//		IFSMConditionSource ����
//////////////////////////////////////////////////////////////////////////

bool ZActorWithFSM::Cond_IsEndAction()
{
	if (m_pVMesh)
	{
		if (m_pVMesh->isOncePlayDone())
			return true;
	}
	return false;
}

bool ZActorWithFSM::Cond_IsLanding()
{
	return m_pModule_Movable->isLanding();
}


bool ZActorWithFSM::Cond_HasTarget()
{
	if (m_uidTarget.IsValid())
		return true;

	return false;
}

void ZActorWithFSM::Cond_GetTargetPos( rvector& out )
{
	out = rvector(0,0,0);

	if (!m_pGame) { _ASSERT(0); return; }
	if (m_uidTarget.IsInvalid()) return;

	ZCharacterObject* pChar = dynamic_cast<ZCharacterObject*>(ZGetObjectManager()->GetObject(m_uidTarget));
	if (!pChar) return;

	out = pChar->GetPosition();
}

bool ZActorWithFSM::Cond_CheckStateCooltime( ZFSMState* pState, DWORD dwCooltime )
{
	if (!m_pFsm) { _ASSERT(0); return false; }

	return m_stateEnteredTimeStamper.CheckCooltime(pState, dwCooltime);
}

DWORD ZActorWithFSM::Cond_GetStateEnteredTime()
{
	if (!m_pFsm) { _ASSERT(0); return false; }
	if (!m_pCurrState) { _ASSERT(0); return false; }

	return m_stateEnteredTimeStamper.GetEnteredTime(m_pCurrState);
}

bool ZActorWithFSM::IsActiveLightningDamageModule()
{
	return IsActiveModule(ZMID_LIGHTNINGDAMAGE);
}
bool ZActorWithFSM::CanSee(ZObject* pTarget)
{
	// �þ߿� ���̴��� Ȯ��

	if (pTarget && !ZGetGame()->IsWallBlocked(pTarget, this, true))
	{
		rvector vTargetDir = pTarget->GetPosition() - GetPosition();
		rvector vBodyDir = GetDirection();
		vBodyDir.z = vTargetDir.z = 0.0f;

		float angle = fabs(GetAngleOfVectors(vTargetDir, vBodyDir));
		if (angle <= 25) return true;
	}

	return false;
}


bool ZActorWithFSM::Cond_CanSeeTarget()
{
	if (!m_pGame) { _ASSERT(0); return false; }
	if (m_uidTarget.IsInvalid()) return false;

	ZObject* pChar = ZGetObjectManager()->GetObject(m_uidTarget);
	if (!pChar) return false;

	return CanSee(pChar);
}

bool ZActorWithFSM::Cond_CantSeeTarget()
{
	if (!m_pGame) { _ASSERT(0); return false; }
	if (m_uidTarget.IsInvalid()) return false;
	ZObject* pChar = ZGetObjectManager()->GetObject(m_uidTarget);
	if (!CanSee(pChar))
	{
		return true;
	}
	return false;
}


bool ZActorWithFSM::Cond_Pick(float fAngleDegree, float fDist)
{
	rvector pos = GetPosition() + rvector(0,0,100);
	rvector dir = GetDirection();
	dir.z = 0;
	D3DXMATRIX mat;
	D3DXMatrixRotationZ(&mat, ToRadian(fAngleDegree));
	dir = dir*mat;
	Normalize(dir);

	ZPICKINFO pickinfo;
	memset(&pickinfo,0,sizeof(ZPICKINFO));

	const DWORD dwPickPassFlag=RM_FLAG_ADDITIVE | RM_FLAG_HIDE;

	if (m_pGame->Pick(this,pos, dir, &pickinfo, dwPickPassFlag)) 
	{
		rvector pickpos;

		if (pickinfo.bBspPicked)
		{
			pickpos = pickinfo.bpi.PickPos;
		}
		else if (pickinfo.pObject)
		{
			pickpos = pickinfo.pObject->GetPosition();
			pickpos.z += 100;
		}

		float fDistSq = fDist * fDist;
		if (MagnitudeSq(pos - pickpos) < fDistSq)
			return true;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////
//		ZFSMFunctionServer ����
//////////////////////////////////////////////////////////////////////////

void ZActorWithFSM::Func_FindTarget()
{
	if (!m_pGame) { _ASSERT(0); return; }

	MUID uidTarget	= MUID(0,0);
	float fDist		= FLT_MAX;

	for (auto itor = ZGetObjectManager()->begin(); itor != ZGetObjectManager()->end(); ++itor)
	{
		ZObject* pObj = (*itor).second;
		if (pObj->IsDie())
			continue;
		if (pObj->GetTeamID() == GetTeamID())
			continue;

		// �Ÿ��� ���Ѵ�.
		float dist = MagnitudeSq(pObj->GetPosition() - this->GetPosition());

		// �� ����� ���̸� �̳��� Ÿ������ ���Ѵ�.
		if (dist < fDist)
		{
			fDist = dist;
			uidTarget = pObj->GetUID();
		}
	}
	m_uidTarget = uidTarget;
}

void ZActorWithFSM::Func_SetTargetLastAttacker()
{
	m_uidTarget = m_pModule_HPAP->GetLastAttacker();
}

void ZActorWithFSM::Func_RotateToTarget(float fFrameDelta)
{
	if (!m_pGame) { _ASSERT(0); return; }

	if (m_uidTarget.IsInvalid()) return;

	ZCharacterObject* pChar = dynamic_cast<ZCharacterObject*>(ZGetObjectManager()->GetObject(m_uidTarget));
	if (!pChar) return;

	// �̹� �����ӿ� ȸ���� ��ŭ�� Ÿ���� ���� ȸ��
	rvector targetDir(pChar->GetPosition() - GetPosition());
	Normalize(targetDir);

	RotateToDirection(targetDir, fFrameDelta);
}

void ZActorWithFSM::Func_FaceToTarget()
{
	if (!m_pGame) { _ASSERT(0); return; }

	if (m_uidTarget.IsInvalid()) return;

	ZCharacterObject* pChar = dynamic_cast<ZCharacterObject*>(ZGetObjectManager()->GetObject(m_uidTarget));
	if (!pChar) return;

	// �̹� �����ӿ� ȸ���� ��ŭ�� Ÿ���� ���� ȸ��
	rvector targetDir(pChar->GetPosition() - GetPosition());
	targetDir.z = 0;
	Normalize(targetDir);

	SetDirection(targetDir);
}

void ZActorWithFSM::Func_ReduceGroggy(float f)
{
	m_fGroggy -= f;

	//todok del
	//mlog("��ACTOR_GROGGY : %f\n", m_fGroggy);
}

void ZActorWithFSM::Func_BuildWaypointsToTarget()
{
	if (!m_pGame) { _ASSERT(0); return; }

	if (m_uidTarget.IsInvalid()) return;
	m_listWaypoint.clear();

	ZCharacterObject* pTarget = dynamic_cast<ZCharacterObject*>(ZGetObjectManager()->GetObject(m_uidTarget));
	if (!pTarget) return;

	// Make path
	ZNavigationMesh navMesh = m_pGame->GetNavigationMesh();
	if ( navMesh.IsNull())
		return;

	// Make navigation path
	rvector tarpos = pTarget->GetPosition();
	if ( !navMesh.BuildNavigationPath( GetPosition(), tarpos))
		return;

	for (list<rvector>::iterator it=navMesh.GetWaypointList().begin(); it!=navMesh.GetWaypointList().end(); ++it)
		m_listWaypoint.push_back(*it);

	//AdjustWayPointWithBound(m_WayPointList, navMesh);
}

void ZActorWithFSM::Func_RunWaypoints(float fFrameDelta)
{
	if (m_listWaypoint.empty()) return;
	if (!m_pActorDef) return;

	rvector nextWaypoint = *m_listWaypoint.begin();
	rvector myPos = GetPosition();

	nextWaypoint.z = 0;
	myPos.z = 0;

	float diffSq = MagnitudeSq(nextWaypoint - myPos);
	if (diffSq <= 20.f*20.f)
	{
		m_listWaypoint.pop_front();
		this->Func_RunWaypoints(fFrameDelta);	// ��������Ʈ�� ���������� ���ȣ��� ���� ��������Ʈ�� ���� ó���� �ϰ� �Ѵ�
		return;
	}

	//rvector dir = nextWaypoint - myPos;
	//Normalize(dir);
	//SetDirection(dir);

	rvector vWaypointDir = nextWaypoint - myPos;
	Normalize(vWaypointDir);

	// ���� ��������Ʈ�� ���� ���� ��ȯ
	RotateToDirection(vWaypointDir, fFrameDelta);

	// ��������Ʈ ���� �̵�
	//float fSpeed = m_pActorDef->GetSpeed();
	//rvector newPos = vWaypointDir * (fSpeed * fFrameDelta) + myPos;
	//SetPosition(newPos);
	SetMoving(true);
}

void ZActorWithFSM::Func_Disappear(float fFrameDelta, DWORD nDelay, float fRatio)
{
	if (!m_pVMesh) { _ASSERT(0); return; }
	if (!m_pCurrState) { _ASSERT(0); return; }

	if (!stricmp(this->GetActorDef()->GetActorType(), "barricade")) return;
	if (!stricmp(this->GetActorDef()->GetActorType(), "radar")) return;

	DWORD elapsed = timeGetTime() - Cond_GetStateEnteredTime();

	// �����̸�ŭ ��ٷȴٰ� ����ȭ ����
	if (elapsed >= nDelay)
	{
		elapsed -= nDelay;

		float fOpacity = 1.f - fRatio * (elapsed*0.001f);

		if (fOpacity > 1.f) fOpacity = 1.f;
		if (fOpacity < 0) fOpacity = 0;

		m_pVMesh->SetVisibility(fOpacity);
	}
}

void ZActorWithFSM::Func_PostDead()
{
	if (IsMyControl())
		ZPostNewQuestRequestNPCDead(m_pModule_HPAP->GetLastAttacker(), GetUID(), GetPosition());
	else
		_ASSERT(0);
}

void ZActorWithFSM::Func_OnEnterBlastedSoar()
{
	rvector dir = GetDirection();
	dir.x = -m_blastedDir.x;
	dir.y = -m_blastedDir.y;
	SetDirection(dir);

	SetVelocity(m_blastedDir * 300.f + rvector(0,0, (float)RandomNumber( 1500, 1900)));

	//m_bBlasting = true;
	//SetFlag(AF_LAND, false); //todok land �÷��� ��������� ó������
}

void ZActorWithFSM::Func_OnEnterBlastedThrust()
{
	rvector dir = GetDirection();
	dir.x = -m_daggerBlastedDir.x;
	dir.y = -m_daggerBlastedDir.y;
	SetDirection(dir);

	Normalize(m_daggerBlastedDir);
	AddVelocity(m_daggerBlastedDir * (RUN_SPEED * 4));

	m_bSuffering = true;

	/*rvector dir = GetDirection();
	dir.x = -m_daggerBlastedDir.x;
	dir.y = -m_daggerBlastedDir.y;
	SetDirection(dir);

	SetVelocity(m_daggerBlastedDir * 300.f + rvector(0,0,100.f));

	m_vAddBlastVel = GetPosition() - m_daggerBlastedPos;
	m_vAddBlastVel.z = 0.f;

	Normalize(m_vAddBlastVel);

	m_fAddBlastVelTime = m_pGame->GetTime();

	m_bSuffering = true;
	//SetFlag(AF_LAND, false); //todok land �÷��� ��������� ó������*/
}

float accum_fBlastedThrustVelSubtract = 0;

void ZActorWithFSM::Func_OnUpdateBlastedThrust(float fFrameDelta)
{
	_ASSERT(IsSuffering());

	// �ð��� �帧�� ���� �ӵ����� x, y ������ ���ӽ�Ų��
	rvector dir=rvector(GetVelocity().x,GetVelocity().y,0);
	float fSpeed = Magnitude(dir);
	Normalize(dir);

	const float fBlastThrustBreak = 4000.f;

	fSpeed -= fBlastThrustBreak * fFrameDelta;
	if (fSpeed < 0)
		fSpeed = 0;

	SetVelocity(dir.x*fSpeed, dir.y*fSpeed, GetVelocity().z);

	//// �ð��� �帧�� ���� �ӵ����� x, y ������ ���ӽ�Ų��
	//rvector vel = GetVelocity();
	//vel.x *= 0.9;
	//vel.y *= 0.9;
	//SetVelocity(vel);

	/*float subtractRatio = 0.9f * fFrameDelta;
	accum_fBlastedThrustVelSubtract += subtractRatio;
	if (accum_fBlastedThrustVelSubtract > 0.1f)
	{
		vel.x -= vel.x * subtractRatio;
		vel.y -= vel.y * subtractRatio;

		SetVelocity(vel);

		accum_fBlastedThrustVelSubtract = 0;
	}*/
	
	//vel.x -= 500.f * fFrameDelta;
	//vel.y -= 500.f * fFrameDelta;

	//todok del
	if (IsSuffering())
	{
		///idc if it's set, stop sspamming my mlog maiet -_-
	//	mlog("* suffering vel setted :%f %f %f\n", GetVelocity().x, GetVelocity().y, GetVelocity().z);
	}

/*
#define BLAST_DAGGER_MAX_TIME 0.8f

	float fTime = max((1.f - (m_pGame->GetTime() - m_fAddBlastVelTime) / BLAST_DAGGER_MAX_TIME),0.0f);

	if( fTime < 0.4f )
		fTime = 0.f;

	float fPower = 400.f * fTime * fTime * fFrameDelta * 80.f;

	//todok �Ʒ� �ּ��� �ʿ��ҷ���? Ȯ���� �ʿ�
//	if(fPower==0.f)
//		SetFlag(AF_BLAST_DAGGER,false);//���� �ٵȰ�

	rvector vel = m_vAddBlastVel * fPower;

	SetVelocity(vel);*/
}

void ZActorWithFSM::Func_ExitSuffering()
{
	m_bSuffering = false;
}

void ZActorWithFSM::Func_OnEnterDie()
{
	SetDeadTime(m_pGame->GetTime());
	if (m_summonCount > 0)
		m_summonCount--;
}

//Custom: The missing conditionals
bool ZActorWithFSM::Cond_FailedBuildWaypoints()
{
	if (m_listWaypoint.size() < 0)
		return true;

	return false;
}

bool ZActorWithFSM::Cond_AngleTargetHeight(int low, int high)
{
	rvector out = rvector(0, 0, 0);
	rvector out2 = rvector(0, 0, 0);

	if (!ZGetGame()) { _ASSERT(0); return false; }
	if (m_uidTarget.IsInvalid()) return false;

	ZObject* pObj = ZGetGame()->m_ObjectManager.GetObject(m_uidTarget);
	if (!pObj) return false;

	out = pObj->GetPosition() - GetPosition();
	out2 = GetPosition();

	float angle = fabs(GetAngleOfVectors(out, out2));
	if (angle <= high && angle >= low) return true;
	return false;
}

bool ZActorWithFSM::Cond_TargetHeightHigher(int diff)
{
	ZCharacterObject* pCharacter = (ZCharacterObject*)ZGetObjectManager()->GetObject(m_uidTarget);
	if (pCharacter)
	{
		float diffheight = pCharacter->GetPosition().z - GetPosition().z;
		if (diffheight >= diff)
			return true;
	}
	return false;
}

bool ZActorWithFSM::Cond_SummonLess(int Count)
{
	if (m_summonCount < Count)
		return true;

	return false;
}

void ZActorWithFSM::Func_FindTargetInHeight(int height)
{
	if (!m_pGame) { _ASSERT(0); return; }

	MUID uidTarget = MUID(0, 0);
	float fDist = FLT_MAX;

	for (auto itor = ZGetObjectManager()->begin(); itor != ZGetObjectManager()->end(); ++itor)
	{
		ZCharacterObject* pObj = (ZCharacterObject*)(*itor).second;
		if (pObj->IsDie())
			continue;
		if (pObj->GetTeamID() == GetTeamID())
			continue;

		// �Ÿ��� ���Ѵ�.
		float dist = Magnitude(pObj->GetPosition() - this->GetPosition());
		int diff = pObj->GetPosition().z - GetPosition().z;

		// �� ����� ���̸� �̳��� Ÿ������ ���Ѵ�.
		if ((dist < fDist) && diff <= height)
		{
			fDist = dist;
			uidTarget = pObj->GetUID();
		}
	}

	m_uidTarget = uidTarget;
}

void ZActorWithFSM::Func_RunAlongTargetOrbital(int dist,float fDelta)
{
	if (!m_pGame) { _ASSERT(0); return; }

	if (m_uidTarget.IsInvalid()) return;

	ZCharacter* pTarget = (ZCharacter*)ZGetCharacterManager()->Find(m_uidTarget);
	if (!pTarget) return;
	m_listWaypoint.clear();

	// Make path

	ZNavigationMesh navMesh = m_pGame->GetNavigationMesh();
	if (navMesh.IsNull())
		return;

	// Make navigation path
	rmatrix mat;
	rvector tarpos = pTarget->GetPosition() - GetPosition();
	tarpos.z = 0;
	if (tarpos.x < 0)
		D3DXMatrixRotationZ(&mat, -dist);
	else
		D3DXMatrixRotationZ(&mat, dist);
	tarpos = tarpos * mat;
	tarpos.z = 0;//we dont care about the height difference
	if (tarpos.x < -dist || tarpos.x > dist)
	{
		m_listWaypoint.clear();
		Func_BuildWaypointsToTarget();
		Func_RunWaypoints(fDelta);
	}
	else
	{
		if (navMesh.BuildNavigationPath(GetPosition(), tarpos))
		{

			m_listWaypoint.clear();
			for (list<rvector>::iterator it = navMesh.GetWaypointList().begin(); it != navMesh.GetWaypointList().end(); ++it)
				m_listWaypoint.push_back(*it);
			Func_RunWaypoints(fDelta);
		}
	}
}

void ZActorWithFSM::Func_ClearWayPoints()
{
	m_listWaypoint.clear();
}

void ZActorWithFSM::Func_SpeedAccel(int speedAccel)
{
	SetSpeedAccel(speedAccel);
}

void ZActorWithFSM::Func_TurnOrbitalDirection()
{
	//TODO:
	ZCharacterObject* pObj = dynamic_cast<ZCharacterObject*>(ZGetObjectManager()->GetObject(m_uidTarget));
	if (!pObj)
		return;
	//????
	rvector dir = pObj->GetPosition() - GetPosition();
	Normalize(dir);
	SetDirection(dir);
}

//blitzkrieg

void ZActorWithFSM::Func_FindTargetInDist(int nArg1)
{
	if (!ZGetGame())
		return;

	if (m_uidTarget.IsValid())
	{
		ZObject* target = ZGetObjectManager()->GetObject(m_uidTarget);
		float currDistance = Magnitude(GetPosition() - target->GetPosition());
		if (currDistance > nArg1)
			m_uidTarget.SetInvalid();
		else
			return;
	}

	MUID uidTarget = MUID(0, 0);

	for (auto it = ZGetObjectManager()->begin(); it != ZGetObjectManager()->end(); ++it)
	{
		ZCharacterObject* pObj = dynamic_cast<ZCharacterObject*>(it->second);
		if (pObj->IsDie())
			continue;

		if (pObj->GetTeamID() == GetTeamID() || pObj->GetTeamID() == MMT_ALL && pObj->IsNPC())
			continue;

		float currDistance = Magnitude(GetPosition() - pObj->GetPosition());
		if (currDistance <= nArg1)
		{
			uidTarget = pObj->GetUID();

			rvector targetDir = pObj->GetPosition() - GetPosition();
			targetDir.z = 0;
			Normalize(targetDir);
			SetDirection(targetDir);

			m_uidTarget = uidTarget;
			return;
		}
	}
}

void ZActorWithFSM::Func_FaceToLatestAttacker()
{
	if (!m_pGame) { _ASSERT(0); return; }

//	if (m_uidTarget.IsInvalid()) return;

	ZCharacterObject* pChar = dynamic_cast<ZCharacterObject*>(ZGetObjectManager()->GetObject(m_pModule_HPAP->GetLastAttacker()));
	if (!pChar) return;

	// �̹� �����ӿ� ȸ���� ��ŭ�� Ÿ���� ���� ȸ��
	rvector targetDir(pChar->GetPosition() - GetPosition());
	targetDir.z = 0;
	Normalize(targetDir);

	SetDirection(targetDir);
	m_uidTarget = pChar->GetUID();
}

#include "../Blitz.h"
void ZActorWithFSM::Func_RunWayPointsAlongRoute(float fDelta)
{
	ZNavigationMesh navMesh = ZGetGame()->GetNavigationMesh();
	if (navMesh.IsNull())
		return;

	//you dont want them to continue running their routes unless they have no target
	if (m_uidTarget.IsValid())
		return;

	m_listWaypoint.clear();

	rvector vTargetPos = ProcessRouteLower(GetRoute());
	if (navMesh.BuildNavigationPath(GetPosition(), vTargetPos))
	{
		m_listWaypoint.clear();
		for (list<rvector>::iterator it = navMesh.GetWaypointList().begin(); it != navMesh.GetWaypointList().end(); ++it)
			m_listWaypoint.push_back(*it);

		Func_RunWaypoints(fDelta);
	}
}

rvector ZActorWithFSM::ProcessRouteLower(int nMyRoute)
{
	rvector CurrentPosition = GetPosition();

	RouteList routeList = MGetBlitz()->GetRouteListByID(nMyRoute);

	ZMapSpawnManager* pMSM = ZGetGame()->GetMapDesc()->GetSpawnManager();
	if (!pMSM) return rvector(0, 0, 0);

	rvector vRoutePos;

	switch (GetWayPointReached())
	{
	case 0:
	{
		vRoutePos = pMSM->GetRoutePos(ZMST_ROUTE, routeList.route1);
		if (Magnitude(vRoutePos - CurrentPosition) < 250)
		{
			SetWayPointReached(1);
			return pMSM->GetRoutePos(ZMST_ROUTE, routeList.route2);
		}
		return vRoutePos;
	}
	break;
	case 1:
	{

		vRoutePos = pMSM->GetRoutePos(ZMST_ROUTE, routeList.route2);
		if (Magnitude(vRoutePos - CurrentPosition) < 250)
		{
			SetWayPointReached(2);
			return pMSM->GetRoutePos(ZMST_ROUTE, routeList.route3);
		}
		return vRoutePos;
	}
	break;
	case 2:
	{
		vRoutePos = pMSM->GetRoutePos(ZMST_ROUTE, routeList.route3);
		if (Magnitude(vRoutePos - CurrentPosition) < 250)
		{
			SetWayPointReached(3);
			return pMSM->GetRoutePos(ZMST_ROUTE, routeList.route4);
		}
		return vRoutePos;
	}
	break;
	case 3:
	{
		vRoutePos = pMSM->GetRoutePos(ZMST_ROUTE, routeList.route4);
		if (Magnitude(vRoutePos - CurrentPosition) < 250)
		{
			SetWayPointReached(4);
			return pMSM->GetRoutePos(ZMST_ROUTE, routeList.route5);
		}
		return vRoutePos;
	}
	break;
	case 4:
	{
		vRoutePos = pMSM->GetRoutePos(ZMST_ROUTE, routeList.route5);
		if (Magnitude(vRoutePos - CurrentPosition) < 250)
		{
			SetWayPointReached(5);
			return pMSM->GetRoutePos(ZMST_ROUTE, routeList.route6);
		}
		return vRoutePos;
	}
	break;
	case 5:
	{
		vRoutePos = pMSM->GetRoutePos(ZMST_ROUTE, routeList.route6);
		if (Magnitude(vRoutePos - CurrentPosition) < 250)
		{
			SetWayPointReached(6);
			return pMSM->GetRoutePos(ZMST_ROUTE, routeList.route7);
		}
		return vRoutePos;
	}
	break;
	case 6:
	{
		vRoutePos = pMSM->GetRoutePos(ZMST_ROUTE, routeList.route7);
		if (Magnitude(vRoutePos - CurrentPosition) < 250)
		{
			SetWayPointReached(7);
		}
		return vRoutePos;
	}
	break;
	case 7:
	{
		return pMSM->GetRoutePos(ZMST_ROUTE, routeList.route7);
	}
	break;
	default:
		return CurrentPosition;
		break;
	}

}

void ZActorWithFSM::SpawnSpecialNpc(int nBarricadeCount)
{
	ZFSMState* pState = NULL;
	switch (nBarricadeCount)
	{
	case 9:
		pState = m_pFsm->GetState("summon_zealot"); break;
	case 6:
		pState = m_pFsm->GetState("summon_cleric"); break;
	case 3:
		pState = m_pFsm->GetState("summon_knight"); break;
	case 0:
		pState = m_pFsm->GetState("summon_terminator"); break;
	default:
		return;
	}
	OnExitState();
	m_pCurrState = pState;
	OnEnterState();

	IActorAction* pAction = m_pCurrState->GetAction();
	if (pAction)
	{
		SetAnimationInAction(pAction);
	}

}

//Custom:: bitz damage mods
float ZActorWithFSM::ModifyAttackDamage(float fDamage)
{
	///TODO: handle player damage from this
	//float dist = 0;
	//ZCharacter* characterObj = dynamic_cast<ZCharacter*>(ZGetCharacterManager()->Find(GetLastAttacker()));
	//if (characterObj) {
	//	for (auto itor = ZGetObjectManager()->GetNPCObjectMap()->begin(); itor != ZGetObjectManager()->GetNPCObjectMap()->end(); ++itor) {
	//		if (itor->second->GetUID() == GetUID())
	//			continue;
	//		ZActorWithFSM* fsmActor = dynamic_cast<ZActorWithFSM*>((*itor).second);
	//		if (_stricmp(fsmActor->GetActorDef()->GetActorType(), "barricade") != 0 ||
	//			_stricmp(fsmActor->GetActorDef()->GetActorType(), "radar") != 0)
	//			continue;

	//		float dist = Magnitude(characterObj->GetPosition() - fsmActor->GetPosition());
	//	}
	//}
	return fDamage;//todo: handle modification of damage
}

void ZActorWithFSM::OnDie()
{
}

//TODO: Get Challengequest replays functioning
bool ZActorWithFSM::Save(ZFile* file)
{
	size_t n;

	MTD_ZActorWithFSMInfo_v0 info;
	strcpy_s(info.actorName, GetActorDef()->GetName());
	strcpy_s(info.modelName, GetActorDef()->GetModelName());
	info.uid = m_UID;
	info.fHP = m_pModule_HPAP->GetHP();
	info.fAP = m_pModule_HPAP->GetAP();
	info.Position = GetPosition();
	info.Direction = m_Direction;
	info.Velocity = GetVelocity();
	int currAction = m_pActionMgr->GetIndexOfAction(m_pCurrAction);
	info.currAction = currAction;
	info.teamID = m_teamID;

	n = zfwrite(&info, sizeof(MTD_ZActorWithFSMInfo_v0), 1, file);

	return true;
}

bool ZActorWithFSM::Load(ZFile* file, MTD_ZActorWithFSMInfo_v0 info)
{
	size_t n;

	n = zfread(&info, sizeof(MTD_ZActorWithFSMInfo_v0),1, file);

	SetPosition(info.Position);
	SetDirection(info.Direction);
	m_pModule_HPAP->SetHP(info.fHP);
	m_pModule_HPAP->SetAP(info.fAP);
	SetTeamID(info.teamID);
	SetVelocity(info.Velocity);
	m_UID = info.uid;

	//if (info.currAction != -1)
	//{
	//	ZActorAction* pAction = m_pActionMgr->GetActionByIndex(info.currAction);
	//	this->SetAnimationInAction(pAction);
	//}
	return true;
}
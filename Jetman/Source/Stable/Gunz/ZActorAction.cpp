#include "stdafx.h"
#include "ZActorAction.h"

ZActorAction::~ZActorAction()
{
	int numMelee = GetNumMeleeShot();
	for (int i = 0; i < numMelee; ++i)
		delete m_vecMeleeShot[i];
	
	int num = GetNumRangeShot();
	for (int i=0; i<num; ++i)
		delete m_vecRangeShot[i];

	int num1 = GetNumGrenadeShot();
	for (int i = 0; i < num1; ++i)
		delete m_vecGrenadeShot[i];

	int num2 = GetNumSummon();
	for (int i = 0; i < num2; ++i)
		delete m_vecSummon[i];

	int num3 = GetNumSound();
	for (int i = 0; i < num3; ++i)
		delete m_vecSound[i];

	int num4 = GetNumEffect();
	for (int i = 0; i < num4; ++i)
		delete m_vecEffect[i];
}

void ZActorActionMeleeShot::ProcessShot( IGame* pGame, const MUID& uidOwner, float fShotTime ) const
{
	// 디버그 레지스터 핵 방어 구문을 모두 제거했습니다 필요하면 추가

	ZObjectManager* pObjectManager = ZGetObjectManager();
	if (!pObjectManager) return;

	// 공격자 정보를 구함
	ZObject *pAttacker = pObjectManager->GetObject(uidOwner);
	if (!pAttacker) return;

	if (m_fRange == 0) return;

	float fRadius = cosf( ToRadian( m_fAngle * 0.5f));

	// 공격자의 위치와 방향에 대한 정규화
	rvector AttackerPos		= pAttacker->GetPosition();
	rvector AttackerNorPos	= AttackerPos;
	AttackerNorPos.z		= 0;

	rvector AttackerDir		= pAttacker->m_Direction;
	rvector AttackerNorDir	= AttackerDir;
	AttackerNorDir.z		= 0; 
	Normalize( AttackerNorDir);

	// 사운드 출력
	bool bPlayer = false;
	rvector Pos = pAttacker->GetPosition();
	ZApplication::GetSoundEngine()->PlaySoundElseDefault("blade_swing","blade_swing",rvector(Pos.x,Pos.y,Pos.z),bPlayer);

	// 공격이 타겟에 히트 했는지 체크한다.
	bool bHit = false;
	///TODO: fix lag!
	// 타켓에 대한 검사를 수행한다.
	for (auto itor = ZGetObjectManager()->begin(); itor != ZGetObjectManager()->end(); ++itor)
	{
		// 타겟에 대한 정보를 얻는다.
		ZCharacterObject* pVictim = static_cast<ZCharacterObject*>((*itor).second);

		// 타켓이 죽은 상태면 다음 타겟으로 넘어간다.
		ZModule_HPAP* pModule = (ZModule_HPAP*)pVictim->GetModule(ZMID_HPAP);
		if ( pVictim->IsDie())
			continue;

		// 타겟이 공격자 자신이면 다음 타겟으로 넘어간다.
		if ( pAttacker == pVictim)
			continue;

		// 공격 가능한 상대가 아니면 다음 타겟으로 넘어간다.
		//if (!CanAttack( pAttacker, pVictim))

		if (pAttacker->GetTeamID() == pVictim->GetTeamID())
			continue;

		// 타겟의 정보를 구한다.
		rvector VictimPos, VictimDir;
		if (!pVictim->GetHistory( &VictimPos, &VictimDir, fShotTime))
			continue;

		// 공격자에 대한 타켓의 위치와 방향 정규화
		rvector VictimNorPos	= VictimPos;
		VictimNorPos.z			= 0;

		rvector VictimNorDir	= VictimPos - ( AttackerPos - ( AttackerNorDir * 50.f));
		VictimNorDir.z			= 0;
		Normalize( VictimNorDir);


		// 공격자와 타겟간의 x,y 평면 상의 거리를 구한 다음, 거리가 공격 가능 거리보다 멀면 다음 타겟으로 넘어간다.
		float fDist = Magnitude( AttackerNorPos - VictimNorPos);
		if (fDist > m_fRange)
			continue;

		// 타켓이 공격 가능한 각도 밖이면 다음 타겟으로 넘어간다.
		float fDot = D3DXVec3Dot( &AttackerNorDir, &VictimNorDir);
		if ( fDot < fRadius)
			continue;

		// 타겟이 해당 공격 범위 안에서 일정 영역 높이에 있는지 확인한다.
		if (!pGame->InRanged(pAttacker, pVictim))
			continue;

		// 공격자와 타겟 사이에 벽이 막고 있으면 다음 타겟으로 넘어간다.
		int nDebugRegister = 0;
		if (pGame->IsWallBlocked( pAttacker, pVictim, nDebugRegister, true))
			continue;

		if (GetThrust())
		{
			bool bDamage = false;

			if (fDist < GetRange())
			{
				float add_time = 0.3f * (fDist / 600.f);
				float time = fShotTime + add_time;

				ZGetEffectManager()->AddSwordUppercutDamageEffect(VictimPos, pVictim->GetUID(), (DWORD)(add_time * 1000));

				if (pVictim == ZGetGame()->m_pMyCharacter)
				{
					ZGetGame()->m_pMyCharacter->ReserveDashAttacked(pAttacker->GetUID(), time, AttackerDir);
				}
				pVictim->OnBlastDagger(AttackerDir, AttackerPos);

				if (ZGetGame()->CanAttack(pAttacker, pVictim))
				{
					pVictim->OnDamagedSkill(pAttacker, AttackerPos, ZD_MELEE, MWT_DAGGER, m_fDamage, m_fPierce);
				}

				if (!m_strSound.empty())
					ZGetSoundEngine()->PlaySound((char*)m_strSound.c_str(), VictimPos);
			}
		}
		else if (GetUpperCut())
		{
			if (pVictim == ZGetGame()->m_pMyCharacter)
			{
				ZGetGame()->m_pMyCharacter->SetLastThrower(pAttacker->GetUID(), fShotTime + 1.0f);
				ZPostReaction(fShotTime, 2);
				ZGetGame()->m_pMyCharacter->AddVelocity(rvector(0, 0, 1700));
			}
			pVictim->OnBlast(AttackerDir);

			if (ZGetGame()->CanAttack(pAttacker, pVictim))
				pVictim->OnDamagedSkill(pAttacker, AttackerPos, ZD_MELEE, MWT_DAGGER, m_fDamage, m_fPierce);


			if (!m_strSound.empty())
				ZGetSoundEngine()->PlaySound((char*)m_strSound.c_str(), VictimPos);
		}
		else
		{
			if (pVictim->IsGuard() && (DotProduct(pVictim->m_Direction, AttackerNorDir) < 0))
			{
				rvector pos = pVictim->GetPosition();
				pos.z += 120.f;

				ZGetEffectManager()->AddSwordDefenceEffect(pos + (pVictim->m_Direction * 50.f), pVictim->m_Direction);
				pVictim->OnMeleeGuardSuccess();
				return;
			}

			rvector pos = pVictim->GetPosition();
			pos.z += 130.f;
			pos -= AttackerDir * 50.f;

			ZGetEffectManager()->AddBloodEffect(pos, -VictimNorDir);
			ZGetEffectManager()->AddSlashEffect(pos, -VictimNorDir, 0);

			float fActualDamage = m_fDamage;
			float fPierceRatio = m_fPierce;
			pVictim->OnDamaged(pAttacker, pAttacker->GetPosition(), ZD_MELEE, MWT_DAGGER, fActualDamage, fPierceRatio, 0);

			if (!m_strSound.empty())
				ZGetSoundEngine()->PlaySound((char*)m_strSound.c_str(), pos);
		}
	}
	return;
}

void ZActorActionRangeShot::ProcessShot(const rvector& pos, const rvector& dir, ZObject* pOwner) const
{
	//rvector vMissilePos, vMissileDir;
	//GetMissilePosDir( vMissileDir, vMissilePos, targetPos);
	//ZGetGame()->m_WeaponManager.AddNewQuestProjectile(this, pos, dir, pOwner);

	ZGetGame()->m_WeaponManager.AddNewQuestProjectile(this, pos, dir, pOwner);
}

//todo:

void ZActorActionGrenadeShot::ProcessShot(rvector& pos, rvector& dir, ZObject* pOwner) const
{
	switch (GetGrenadeType())
	{
	case 0:
		ZGetGame()->m_WeaponManager.AddDynamite(pos, dir, pOwner);
	case 3:
		ZGetGame()->m_WeaponManager.AddTrap(pos, dir, GetItemID(), pOwner);
	}
}


void ZActorActionSound::ProcessSound(const rvector& pos, const char* soundName) const
{
	if (soundName == "")
	{
		mlog("error getting soundname");
		return;
	}
	ZGetSoundEngine()->PlaySound((char*)soundName, pos);
}

void ZActorActionEffect::ProcessEffect(const rvector& pos, const rvector& dir, const float scale, const char* meshName,const MUID& uidOwner,
	const float delay) const
{
	ZGetEffectManager()->AddWithScale(meshName, pos, dir, uidOwner, scale,delay);
}

void ZActorActionSummon::ProcessSummon(const MUID& uidOwner, const MUID& uidTarget, int Count, 
	const char* name, const rvector& pos, const rvector& dir, int route) const
{
	if (name == "")
	{
		mlog("Error in ZActorAction, no summoning npc name could be found.\n");
		return;
	}

	ZPostNewQuestRequestNpcSpawn(uidOwner, uidTarget, Count, name, pos, dir,route);
}
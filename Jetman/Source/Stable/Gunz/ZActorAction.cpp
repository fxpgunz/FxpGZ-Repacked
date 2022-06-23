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
	// ����� �������� �� ��� ������ ��� �����߽��ϴ� �ʿ��ϸ� �߰�

	ZObjectManager* pObjectManager = ZGetObjectManager();
	if (!pObjectManager) return;

	// ������ ������ ����
	ZObject *pAttacker = pObjectManager->GetObject(uidOwner);
	if (!pAttacker) return;

	if (m_fRange == 0) return;

	float fRadius = cosf( ToRadian( m_fAngle * 0.5f));

	// �������� ��ġ�� ���⿡ ���� ����ȭ
	rvector AttackerPos		= pAttacker->GetPosition();
	rvector AttackerNorPos	= AttackerPos;
	AttackerNorPos.z		= 0;

	rvector AttackerDir		= pAttacker->m_Direction;
	rvector AttackerNorDir	= AttackerDir;
	AttackerNorDir.z		= 0; 
	Normalize( AttackerNorDir);

	// ���� ���
	bool bPlayer = false;
	rvector Pos = pAttacker->GetPosition();
	ZApplication::GetSoundEngine()->PlaySoundElseDefault("blade_swing","blade_swing",rvector(Pos.x,Pos.y,Pos.z),bPlayer);

	// ������ Ÿ�ٿ� ��Ʈ �ߴ��� üũ�Ѵ�.
	bool bHit = false;
	///TODO: fix lag!
	// Ÿ�Ͽ� ���� �˻縦 �����Ѵ�.
	for (auto itor = ZGetObjectManager()->begin(); itor != ZGetObjectManager()->end(); ++itor)
	{
		// Ÿ�ٿ� ���� ������ ��´�.
		ZCharacterObject* pVictim = static_cast<ZCharacterObject*>((*itor).second);

		// Ÿ���� ���� ���¸� ���� Ÿ������ �Ѿ��.
		ZModule_HPAP* pModule = (ZModule_HPAP*)pVictim->GetModule(ZMID_HPAP);
		if ( pVictim->IsDie())
			continue;

		// Ÿ���� ������ �ڽ��̸� ���� Ÿ������ �Ѿ��.
		if ( pAttacker == pVictim)
			continue;

		// ���� ������ ��밡 �ƴϸ� ���� Ÿ������ �Ѿ��.
		//if (!CanAttack( pAttacker, pVictim))

		if (pAttacker->GetTeamID() == pVictim->GetTeamID())
			continue;

		// Ÿ���� ������ ���Ѵ�.
		rvector VictimPos, VictimDir;
		if (!pVictim->GetHistory( &VictimPos, &VictimDir, fShotTime))
			continue;

		// �����ڿ� ���� Ÿ���� ��ġ�� ���� ����ȭ
		rvector VictimNorPos	= VictimPos;
		VictimNorPos.z			= 0;

		rvector VictimNorDir	= VictimPos - ( AttackerPos - ( AttackerNorDir * 50.f));
		VictimNorDir.z			= 0;
		Normalize( VictimNorDir);


		// �����ڿ� Ÿ�ٰ��� x,y ��� ���� �Ÿ��� ���� ����, �Ÿ��� ���� ���� �Ÿ����� �ָ� ���� Ÿ������ �Ѿ��.
		float fDist = Magnitude( AttackerNorPos - VictimNorPos);
		if (fDist > m_fRange)
			continue;

		// Ÿ���� ���� ������ ���� ���̸� ���� Ÿ������ �Ѿ��.
		float fDot = D3DXVec3Dot( &AttackerNorDir, &VictimNorDir);
		if ( fDot < fRadius)
			continue;

		// Ÿ���� �ش� ���� ���� �ȿ��� ���� ���� ���̿� �ִ��� Ȯ���Ѵ�.
		if (!pGame->InRanged(pAttacker, pVictim))
			continue;

		// �����ڿ� Ÿ�� ���̿� ���� ���� ������ ���� Ÿ������ �Ѿ��.
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
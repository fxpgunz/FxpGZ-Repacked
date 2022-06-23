#include "StdAfx.h"
#include "ZWeaponNewQuest.h"
#include "ZActorAction.h"

MImplementRTTI(ZWeaponNpcBullet, ZMovingWeapon);

void ZWeaponNpcBullet::Create(RMesh* pMesh, const ZActorActionRangeShot* pRangeShot, const rvector &pos, const rvector &dir, float fScale, ZObject* pOwner) 
{
	if (!pRangeShot) { _ASSERT(0); return; }

	ZWeapon::Create(pMesh);

	m_pRangeShot = pRangeShot;

	m_Position = pos;
	m_Dir = dir;
	m_Up = rvector(0,0,1);
	m_fScale = fScale;
	m_Velocity = dir * m_pRangeShot->GetSpeed();

	m_pVMesh->SetAnimation("play");

	m_fStartTime = ZGetGame()->GetTime();
	m_fLastAddTime = ZGetGame()->GetTime();

	m_pOwner=pOwner;
	m_uidOwner=pOwner->GetUID();
	m_nTeamID=pOwner->GetTeamID();

	m_fDamage = pRangeShot->GetDamage();

	/*//tododk �������� �ϴ� ��������
	if( Z_VIDEO_DYNAMICLIGHT && !pSkill->GetDesc()->bOffDLight) {
		_ASSERT( m_SLSid == 0);
		m_SLSid = ZGetStencilLight()->AddLightSource( m_Position, 2.0f );
	}
	*/
}

bool ZWeaponNpcBullet::Update(float fElapsedTime)
{
	rvector oldPos = m_Position;

	#define WEAPON_LIFE			10.f		// 10�� �ڿ� ����

	if(ZGetGame()->GetTime() - m_fStartTime > WEAPON_LIFE ) {
		// ����ź�� ������ ������ ����Ʈ �߰�.. ����...
		Explosion(NULL, rvector(0,1,0));

		/*//tododk �������� �ϴ� ��������
		if(Z_VIDEO_DYNAMICLIGHT && m_SLSid ) {
			ZGetStencilLight()->DeleteLightSource( m_SLSid );
			m_SLSid = 0;
		}
		*/

		return false;
	}

	const DWORD dwPickPassFlag = RM_FLAG_ADDITIVE | RM_FLAG_HIDE | RM_FLAG_PASSROCKET;

	rvector diff=m_Velocity*fElapsedTime;
	rvector dir=diff;
	Normalize(dir);

	ZPICKINFO zpi;

	ZObject* pOwnerObject = m_pOwner;
	ZObject* pPickObject = NULL;

	//todok collRadius�� ����ϴ� ���� ����.. ��ŷ�� �ƴ϶� ��ü �浹�� �׽�Ʈ�ؾ� �Ѵ�

	bool bPicked=ZGetGame()->Pick(pOwnerObject, m_Position,dir,&zpi,dwPickPassFlag);
	if(bPicked)
	{
		rvector pickpos;
		rvector pickdir;	//todok ���� ���µ��� ���� ���� �Ⱦ��ٸ� ���ֹ�����
		float fDist=Magnitude(diff);

		if(zpi.bBspPicked) {
			pickpos=zpi.bpi.PickPos;
			pickdir.x=zpi.bpi.pInfo->plane.a;
			pickdir.y=zpi.bpi.pInfo->plane.b;			
			pickdir.z=zpi.bpi.pInfo->plane.c;
			Normalize(pickdir);
		}	
		else if(zpi.pObject) {
			pPickObject = zpi.pObject;
			pickpos=zpi.info.vOut;
		}

		if ( fabsf(Magnitude(pickpos-m_Position)) < fDist )
		{
			if( pPickObject && pPickObject->GetTeamID() != pOwnerObject->GetTeamID())	// ĳ���Ϳ� �浹
			{
				Explosion(pPickObject, dir);
			}
			else	// �ʿ� �浹
			{
				ExplosionOnMap();
			}

			/*//tododk �������� �ϴ� ��������
			if(Z_VIDEO_DYNAMICLIGHT && m_SLSid )
			{
			ZGetStencilLight()->DeleteLightSource( m_SLSid );
			m_SLSid = 0;
			ZGetStencilLight()->AddLightSource( pickpos, 3.0f, 1300 );
			}
			*/
			return false;
		}
	}

	m_Position+=diff;

	rmatrix mat;
	MakeWorldMatrix(&mat,m_Position,m_Dir,m_Up);

	m_pVMesh->SetScale(rvector(m_fScale,m_fScale,m_fScale));
	m_pVMesh->SetWorldMatrix(mat);

	float this_time = ZGetGame()->GetTime();

	if( this_time > m_fLastAddTime + 0.02f ) {

		rvector add = rvector(RANDOMFLOAT-0.5f,RANDOMFLOAT-0.5f,RANDOMFLOAT-0.5f);
		rvector pos = m_Position + 20.f*add;

		ZGetWorld()->GetFlags()->CheckSpearing( oldPos, pos	, ROCKET_SPEAR_EMBLEM_POWER );
		m_fLastAddTime = this_time;
	}

	/*//tododk �������� �ϴ� ��������
	if(Z_VIDEO_DYNAMICLIGHT)
		ZGetStencilLight()->SetLightSourcePosition( m_SLSid, m_Position	);
*/
	return true;
}

void ZWeaponNpcBullet::Render()
{
	ZWeapon::Render();


	// test �ڵ�
#ifndef _PUBLISH
	if ((ZApplication::GetGameInterface()->GetScreenDebugger()->IsVisible()) && (ZIsLaunchDevelop()))
	{
		if (m_pRangeShot)
		{
			RDrawSphere(m_Position, m_pRangeShot->GetCollRadius(), 10);
		}
	}
#endif

}

#define MIN_DAMAGE	0.2f


void ZWeaponNpcBullet::Explosion(ZObject* pVictim, const rvector& dir)
{
	rvector v = m_Position-rvector(0,0,100.f);
	rvector pos = m_Position;

	// ����Ʈ ���� �߰�
	if (0!=strcmp("", m_pRangeShot->GetSound()))
		ZGetSoundEngine()->PlaySound( (char*)m_pRangeShot->GetSound(), m_Position);
	if (!pVictim)
		return;

	if (pVictim->GetTeamID() == m_pOwner->GetTeamID())
		return;
	
	if (pVictim)
	{
		float fKnockBack = m_pRangeShot->GetKnockbackForce();
		pVictim->OnKnockback(rvector(dir), fKnockBack);
		//pVictim->AddVelocity(fKnockBack*7.f*(1.f/*fRange-fDist*/)*-dir);

		float fDamage = m_pRangeShot->GetDamage();
		float fPierceRatio = m_pRangeShot->GetPierce();
		pVictim->OnDamaged(m_pOwner, pos, ZD_MAGIC, MWT_PISTOL, fDamage, fPierceRatio);
	}
}

void ZWeaponNpcBullet::ExplosionOnMap()
{
	rvector v = m_Position-rvector(0,0,100.f);
	rvector pos = m_Position;

	// ����Ʈ ���� �߰�
	if (0!=strcmp("", m_pRangeShot->GetSound()))
		ZGetSoundEngine()->PlaySound( (char*)m_pRangeShot->GetSound(), m_Position);

	ZGetWorld()->GetFlags()->SetExplosion( v, EXPLOSION_EMBLEM_POWER );
}

MImplementRTTI(ZWeaponNpcGrenade, ZMovingWeapon);
//
//rvector GetReflectionVector(rvector& v, rvector& n) {
//
//	float dot = D3DXVec3Dot(&(-v), &n);
//
//	return (2 * dot)*n + v;
//}

//Custom: Grenades
void ZWeaponNpcGrenade::Create(RMesh* pMesh, ZActorActionGrenadeShot* pGrenadeShot, const rvector& pos, const rvector& vel, ZObject* pOwner)
{
	if (!pGrenadeShot) { _ASSERT(0); return; }

	ZWeapon::Create(pMesh);

	m_pGrenadeShot = pGrenadeShot;

	m_Position = pos;
	m_Dir = rvector(1,0,0);
	m_Up = rvector(0, 0, 1);
	m_Velocity = vel;
	m_fDamage = m_pGrenadeShot->GetDamage();
	m_pVMesh->SetAnimation("play");
	m_pOwner = pOwner;
	m_uidOwner = pOwner->GetUID();
	m_nTeamID = pOwner->GetTeamID();
	m_bActivated = false;
	m_bFriendly = false;
	m_bLanded = false;
	m_nGrenadeType = m_pGrenadeShot->GetGrenadeType();
	m_nItemID = m_pGrenadeShot->GetItemID();
	m_fThrowedTime = ZGetGame()->GetTime();

	if (m_uidOwner == ZGetMyUID())
		ZPostNotifyThrowedTrap(m_nItemID);

}

bool ZWeaponNpcGrenade::Update(float fElapsed)
{
	//if (m_bActivated)
//		return UpdateOnActivated(fElapsed);

	if (m_bLanded)
	{
		if (ZGetGame()->GetTime() - m_fLandedTime > 0.7f)
		{
			Activate();
		}
		return true;
	}

	if (m_Position.z < -2500.f)
		return false;

	if (ZGetGame()->GetTime() - m_fThrowedTime > MAX_TRAP_THROWING_LIFE)
		return false;

	rvector oldPos = m_Position;

	m_Velocity.z -= 1000.f*fElapsed;

	const DWORD dwPickPassFlag = RM_FLAG_ADDITIVE | RM_FLAG_HIDE | RM_FLAG_PASSROCKET;

	{
		rvector diff = m_Velocity*fElapsed;
		rvector dir = diff;
		Normalize(dir);

		float fDist = Magnitude(diff);

		rvector pickpos, normal;

		ZPICKINFO zpi;
		bool bPicked = ZGetGame()->Pick(ZGetObjectManager()->GetObjectA(m_uidOwner), m_Position, dir, &zpi, dwPickPassFlag);
		if (bPicked)
		{
			if (zpi.bBspPicked)
			{
				pickpos = zpi.bpi.PickPos;
				rplane plane = zpi.bpi.pNode->pInfo[zpi.bpi.nIndex].plane;
				normal = rvector(plane.a, plane.b, plane.c);
			}
			else if (zpi.pObject)
			{
				pickpos = zpi.info.vOut;
				if (zpi.pObject->GetPosition().z + 30.f <= pickpos.z && pickpos.z <= zpi.pObject->GetPosition().z + 160.f)
				{
					normal = pickpos - zpi.pObject->GetPosition();
					normal.z = 0;
				}
				else
					normal = pickpos - (zpi.pObject->GetPosition() + rvector(0, 0, 90));
				Normalize(normal);
			}
		}


		// ��򰡿� �浹�ߴ�
		if (bPicked && fabsf(Magnitude(pickpos - m_Position))<fDist)
		{
			m_Position = pickpos + normal;
		//	m_Velocity = GetReflectionVector(m_Velocity, normal);
			m_Velocity *= zpi.pObject ? 0.4f : 0.8f;		// ĳ���Ϳ� �浹�ϸ� �ӵ��� ���� �ش�

			if (zpi.bBspPicked) {
				ZGetSoundEngine()->PlaySound("we_grenade_fire", m_Position);
			}

			// �������� �ӵ������� ���δ�.
			Normalize(normal);
			float fAbsorb = DotProduct(normal, m_Velocity);
			m_Velocity -= 0.5*fAbsorb*normal;

			float fA = RANDOMFLOAT * 2 * pi;
			float fB = RANDOMFLOAT * 2 * pi;
			m_rotAxis = rvector(sin(fA)*sin(fB), cos(fA)*sin(fB), cos(fB));

			// ���鿡 ����� �� �����ð� �� ���� (���̳� õ�� �浹�� �н�)
			if (DotProduct(normal, rvector(0, 0, 1)) > 0.8f)
			{
				if (!m_bLanded)
				{
					m_bLanded = true;
					m_fLandedTime = ZGetGame()->GetTime();

					// Ʈ���� ��ġ�� ��ġ�� �� �� �ְ� ���̵带 ���׸İ� ǥ������
					ZGetEffectManager()->AddTrapGuideEffect(m_Position, rvector(0, 1, 0), int(0.7f * 1000), m_bFriendly, 0.1f, 12.f);
				}
				return true;
			}

		}
		else
			m_Position += diff;
	}

	float fRotSpeed = Magnitude(m_Velocity)*0.04f;

	rmatrix rotmat;
	D3DXQUATERNION q;
	D3DXQuaternionRotationAxis(&q, &m_rotAxis, fRotSpeed *fElapsed);
	D3DXMatrixRotationQuaternion(&rotmat, &q);
	m_Dir = m_Dir * rotmat;
	m_Up = m_Up * rotmat;

	rmatrix mat;
	rvector dir = m_Velocity;
	Normalize(dir);
	MakeWorldMatrix(&mat, m_Position, m_Dir, m_Up);

	mat = rotmat*mat;

	m_pVMesh->SetWorldMatrix(mat);

	ZGetWorld()->GetFlags()->CheckSpearing(oldPos, m_Position, GRENADE_SPEAR_EMBLEM_POWER);

	return true;
}

void ZWeaponNpcGrenade::CreateActivated(RMesh* pMesh, rvector& pos, float fActivatedTime, int nItemId, ZObject* pOwner)
{
	Create(pMesh, (ZActorActionGrenadeShot*)(m_pGrenadeShot), pos, m_Velocity, pOwner);

	m_fActivatedTime = fActivatedTime;
	m_fNextEffectTime = m_fActivatedTime;
	m_bActivated = true;

	rmatrix mat;
	MakeWorldMatrix(&mat, m_Position, m_Dir, m_Up);
	m_pVMesh->SetWorldMatrix(mat);

	float fElapsedTime = ZGetGame()->GetTime() - m_fActivatedTime;
//	AddEffectActivating(fElapsedTime * 1000, m_bFriendly);
}

void ZWeaponNpcGrenade::Activate()
{
	m_bActivated = true;
	m_fActivatedTime = ZGetGame()->GetTime();
	m_fNextEffectTime = m_fActivatedTime;

	//AddEffectActivating(0, m_bFriendly);

	//if (Z_VIDEO_DYNAMICLIGHT)
	//	ZGetStencilLight()->AddLightSource(m_Position, 3.0f, 1300);

	// ������ �ߵ��� Ʈ���� ����ؼ� ���� �����ϴ� ������ ���忡 ��ġ�� Ʈ������ �� �� �ֵ��� ����
	if (m_uidOwner == ZGetMyUID())
		ZPostNotifyActivatedTrap(m_nItemID, m_Position);
}
#pragma once

class ZActorActionRangeShot;
class ZActorActionGrenadeShot;

// 일반적인 탄환 (폭발이나 기타 효과 없음)
class ZWeaponNpcBullet : public ZMovingWeapon
{
	MDeclareRTTI
private:
	float		m_fScale;
	ZObject*	m_pOwner;
	
	const ZActorActionRangeShot* m_pRangeShot;

public:
	ZWeaponNpcBullet() : ZMovingWeapon() { m_fScale = 1.f; }

	void Create(RMesh* pMesh, const ZActorActionRangeShot* pRangeShot, const rvector &pos, const rvector &dir, float fScale, ZObject* pOwner);
	bool Update(float fElapsedTime);
	void Render();

	void Explosion(ZObject* pVictim, const rvector& dir);
	void ExplosionOnMap();

	float	m_fStartTime;
	float	m_fLastAddTime;
};

class ZWeaponNpcGrenade : public ZMovingWeapon
{
	MDeclareRTTI
private:
	ZObject* m_pOwner;
	int      m_nItemID;
	int		 m_nGrenadeType;
	float    m_fDamage;
	rvector	 m_Velocity;
	rvector  m_Position;
	MMatchTeam m_TeamID;
	bool     m_bLanded;
	bool     m_bActivated;
	float    m_fActivatedTime;
	float    m_fLandedTime;
	float    m_fNextEffectTime;
	bool     m_bFriendly;
	const ZActorActionGrenadeShot* m_pGrenadeShot;
	rvector   m_rotAxis;
	float    m_fThrowedTime;
public:
	//defaults
	ZWeaponNpcGrenade() : ZMovingWeapon()
		, m_nItemID(0)
		, m_nGrenadeType(0)
		, m_fDamage(0.f)
		, m_Velocity(0, 0, 0)
		, m_Position(0, 0, 0)
		, m_TeamID(MMT_BLUE)
		, m_bLanded(false)
		, m_bActivated(false)
		, m_fActivatedTime(0.f)
		, m_fLandedTime(0.f)
		, m_fNextEffectTime(0.f)
		, m_bFriendly(false)
	    , m_rotAxis(0,0,1)
	    , m_fThrowedTime(0.f){}

	void Create(RMesh* pMesh, ZActorActionGrenadeShot* pGrenadeShot, const rvector& pos, const rvector& vel, ZObject* pOwner);
	void CreateActivated(RMesh* pMesh, rvector& pos, float fActivatedTime, int nItemId, ZObject* pOwner);
	bool Update(float fElapsed);
	void Activate();
//	bool UpdateOnActivated(float fElapsed);
//	void AddEffectActivating(int nElapsedIfActivated, bool bFriendly);
};
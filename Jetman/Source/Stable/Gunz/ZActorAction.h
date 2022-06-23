#pragma once


class ZActorActionMeleeShot;
class ZActorActionRangeShot;
class ZActorActionGrenadeShot;
class ZActorActionSummon;
class ZActorActionSound;
class ZActorActionEffect;

enum ActionType
{
	AT_MELEE,
	AT_RANGE,
	AT_GRENADE,

	AT_END
};

class IActorAction
{
public:
	virtual ~IActorAction() {}

	virtual const char* GetName() = 0;
	virtual const char* GetAnimationName() = 0;
	virtual bool IsMovingAnimation() = 0;
	virtual const ZActorActionMeleeShot* GetMeleeShot(int idx) = 0;
	virtual const ZActorActionRangeShot* GetRangeShot(int idx) = 0;
	virtual const ZActorActionGrenadeShot* GetGrenadeShot(int idx) = 0;
	virtual const ZActorActionSummon* GetSummon(int idx) = 0;
	virtual const ZActorActionSound* GetSound(int idx) = 0;
	virtual const ZActorActionEffect* GetEffect(int idx) = 0;
	virtual int GetNumMeleeShot() = 0;
	virtual int GetNumRangeShot() = 0;
	virtual int GetNumGrenadeShot() = 0;
	virtual int GetNumSummon() = 0;
	virtual int GetNumSound() = 0;
	virtual int GetNumEffect() = 0;
};

// 액션은 기존에 npc의 특수행동을 정의하기 위해 사용했던 ZSkill과 비슷하다.
// 그러나 액션은 꼭 공격스킬일 필요는 없으며, 어떤 애니메이션을 단순히 재생하는 용도로 사용될 수도 있다.
// 공격스킬일 때도 ZSkill과는 다르게 공격판정을 액션 내에 직접 갖고있지 않으며 공격판정을 가진 객체를 
// 월드에 생성하기 위한 정보만을 갖게 된다.


class ZActorAction : public IActorAction
{
	string m_strName;
	string m_strAnimation;
	bool m_bMovingAnimation;

	typedef vector<ZActorActionMeleeShot*>	VecMeleeShot;
	typedef VecMeleeShot::iterator			ItorMeleeShot;
	VecMeleeShot m_vecMeleeShot;

	typedef vector<ZActorActionRangeShot*>	VecRangeShot;
	typedef VecRangeShot::iterator			ItorRangeShot;
	VecRangeShot m_vecRangeShot;

	typedef vector<ZActorActionGrenadeShot*> VecGrenadeShot;
	typedef VecGrenadeShot::iterator			ItorGrenadeShot;
	VecGrenadeShot m_vecGrenadeShot;

	typedef vector<ZActorActionSummon*> VecSummon;
	typedef VecSummon::iterator			ItorSummon;
	VecSummon m_vecSummon;

	typedef vector<ZActorActionSound*> VecSound;
	typedef VecSound::iterator			ItorSound;
	VecSound m_vecSound;

	typedef vector<ZActorActionEffect*> VecEffect;
	typedef VecEffect::iterator			ItorEffect;
	VecEffect m_vecEffect;

	ActionType m_ActionType;

public:
	ZActorAction() 
		: m_bMovingAnimation(false) {}
	virtual ~ZActorAction();

	void SetName(const char* sz) { m_strName=sz; }
	virtual const char* GetName() { return m_strName.c_str(); }

	void SetAnimationName(const char* sz) { m_strAnimation=sz; }
	virtual const char* GetAnimationName() { return m_strAnimation.c_str(); }

	void SetMovingAnimation(bool b) { m_bMovingAnimation = b; }
	virtual bool IsMovingAnimation() { return m_bMovingAnimation; }

	void AddMeleeShot(ZActorActionMeleeShot* p) { m_vecMeleeShot.push_back(p); }
	virtual const ZActorActionMeleeShot* GetMeleeShot(int idx) {
		if (idx < 0 || GetNumMeleeShot() <= idx) { _ASSERT(0); return NULL; }
		return m_vecMeleeShot[idx];
	}
	virtual int GetNumMeleeShot() { return (int)m_vecMeleeShot.size(); }


	void AddRangeShot(ZActorActionRangeShot* p) { m_vecRangeShot.push_back(p); }
	virtual const ZActorActionRangeShot* GetRangeShot(int idx) { 
		if (idx < 0 || GetNumRangeShot() <= idx) { _ASSERT(0); return NULL; }
		return m_vecRangeShot[idx];
	}
	virtual int GetNumRangeShot() { return (int)m_vecRangeShot.size(); }

	void AddGrenadeShot(ZActorActionGrenadeShot* p) { m_vecGrenadeShot.push_back(p); }
	virtual const ZActorActionGrenadeShot* GetGrenadeShot(int idx) {
		if (idx < 0 || GetNumGrenadeShot() <= idx) { _ASSERT(0); return NULL; }
		return m_vecGrenadeShot[idx];
	}

	virtual int GetNumGrenadeShot() { return (int)m_vecGrenadeShot.size(); }

	void AddSummon(ZActorActionSummon* p) { m_vecSummon.push_back(p); }
	virtual const ZActorActionSummon* GetSummon(int idx) {
		if (idx < 0 || GetNumSummon() <= idx) { _ASSERT(0); return NULL; }
		return m_vecSummon[idx];
	}

	virtual int GetNumSummon() { return (int)m_vecSummon.size(); }

	void AddSound(ZActorActionSound* p) { m_vecSound.push_back(p); }
	virtual const ZActorActionSound* GetSound(int idx) {
		if (idx < 0 || GetNumSound() <= idx) { _ASSERT(0); return NULL; }
		return m_vecSound[idx];
	}

	virtual int GetNumSound() { return (int)m_vecSound.size(); }

	void AddEffect(ZActorActionEffect* p) { m_vecEffect.push_back(p); }
	virtual const ZActorActionEffect* GetEffect(int idx) {
		if (idx < 0 || GetNumEffect() <= idx) { _ASSERT(0); return NULL; }
		return m_vecEffect[idx];
	}

	virtual int GetNumEffect() { return (int)m_vecEffect.size(); }

};

// 밀리샷과 레인지샷의 공통 속성 
class ZActorActionShotCommon
{
protected:
	float m_fDelay;		// 액션이 시작한 뒤 이 딜레이만큼 기다렸다가 샷이 발생
	float m_fDamage;
	float m_fPierce;	// 관통률
	string m_strSound;	// 히트 사운드(지정되지 않은 경우 기본 사운드 발생)
	float m_fKnockbackForce;	// 히트시 넉백 힘

public:
	ZActorActionShotCommon()
		: m_fDelay(0)
		, m_fDamage(0)
		, m_fPierce(0.5f)
		, m_fKnockbackForce(0) {}
	virtual ~ZActorActionShotCommon() {}

	float GetDelay() const { return m_fDelay; }
	float GetDamage() const { return m_fDamage; }
	float GetPierce() const { return m_fPierce; }
	const char* GetSound() const { return m_strSound.c_str(); }
	float GetKnockbackForce() const { return m_fKnockbackForce; }

	void SetDelay(float f) { m_fDelay = f; }
	void SetDamage(float f) { m_fDamage = f; }
	void SetPierce(float f) { m_fPierce= f; }
	void SetSound(const char* sz) { m_strSound = sz; }
	void SetKnockbackForce(float f) { m_fKnockbackForce = f; }
};

class ZActorActionMeleeShot : public ZActorActionShotCommon
{
	float m_fRange;		// 판정 최대 거리
	float m_fAngle;		// 판정 반경 radian
	int   m_thrust;
	int   m_uppercut;
	ActionType m_actionType;
public:
	ZActorActionMeleeShot()
		: m_fRange(0)
		, m_fAngle(0)
		, m_thrust(0)
		, m_uppercut(0)
		, m_actionType(AT_MELEE){}
	
	float GetRange() const { return m_fRange; }
	float GetAngle() const { return m_fAngle; }
	bool  GetThrust() const { return m_thrust; }
	bool  GetUpperCut() const { return m_uppercut; }

	void SetRange(float f) { m_fRange = f; }
	void SetAngle(float f) { m_fAngle = f; }
	void SetThrust(bool b) { m_thrust = b; }
	void SetUpperCut(bool b) { m_uppercut = b;
	}
	void ProcessShot(IGame* pGame, const MUID& uidOwner, float fShotTime) const;
};

class ZActorActionRangeShot : public ZActorActionShotCommon
{
	string m_strMeshName;
	_RMeshPartsPosInfoType m_posPartsType;	// 탄환의 발사위치
	float m_fSpeed;
	float m_fCollRadius;	//todok 이것은 폭발형(스플래쉬) 일때만 사용한다. 나중에 정리 필요
	rvector m_vDirectionMod;	// 자신이 보는 방향 기준으로 발사 각도 보정량
	int   m_nZAxis;
	int   m_nYAxis;
	ActionType m_actionType;
	bool	m_dirTarget;
public:
	ZActorActionRangeShot()
		: m_fSpeed(100.f)
		, m_fCollRadius(100.f)
		, m_vDirectionMod(0,0,0)
		, m_nZAxis(0)
		, m_nYAxis(0)
		, m_actionType(AT_RANGE)
		{}

	const char* GetMeshName() const  { return m_strMeshName.c_str(); }
	_RMeshPartsPosInfoType GetPosPartsType() const { return m_posPartsType; }
	float GetSpeed() const { return m_fSpeed; }
	float GetCollRadius() const  { return m_fCollRadius; }
	const rvector& GetDirectionMod() const { return m_vDirectionMod; }

	void SetMeshName(const char* sz) { m_strMeshName = sz; }
	void SetPosPartsType(_RMeshPartsPosInfoType t) { m_posPartsType = t; }
	void SetSpeed(float f) { m_fSpeed = f; }
	void SetCollRadius(float f) { m_fCollRadius = f; }
	void SetDirectionMod(const rvector& v) { m_vDirectionMod = v; }

	void ProcessShot(const rvector& pos, const rvector& dir, ZObject* pOwner) const;
	//custom: added stuff
	int GetZAxis() const { return m_nZAxis; }
	int GetYAxis() const { return m_nYAxis; }
	void SetZAxis(int n) { m_nZAxis = n; }
	void SetYAxis(int n) { m_nYAxis = n; }
	bool GetDirTarget() const {
		return m_dirTarget;
	}
	void SetDirTarget(bool b) { m_dirTarget = b; }
};

class ZActorActionGrenadeShot : public ZActorActionShotCommon
{
	int m_GrenadeType;
	int m_ItemID;
	int m_ZAxis;
	int m_YAxis;
	int m_Force;
	_RMeshPartsPosInfoType m_posPartsType;
	rvector m_PosMod;
	rvector m_DirMod;
	ActionType m_actionType;
public:
	ZActorActionGrenadeShot()
		: m_GrenadeType(0)
		, m_ItemID(0)
		, m_ZAxis(0)
		, m_YAxis(0)
		, m_Force(0)
		, m_PosMod(0, 0, 0)
		, m_DirMod(0, 0, 0)
		, m_actionType(AT_GRENADE){}

	_RMeshPartsPosInfoType GetPosPartsType() const { return m_posPartsType; }
	void SetPosPartsType(_RMeshPartsPosInfoType t) { m_posPartsType = t; }
	int  GetGrenadeType() const  { return m_GrenadeType; }
	void SetGrenadeType(int n) { m_GrenadeType = n; }
	int GetItemID() const { return m_ItemID; }
	void SetItemID(int n) { m_ItemID = n; }
	int  GetZAxis() const { return m_ZAxis; }
	void SetZAxis(int n) { m_ZAxis = n; }
	int  GetYAxis() const { return m_YAxis; }
	void SetYAxis(int n) { m_YAxis = n; }
	int GetForce() const { return m_Force; }
	void SetForce(int n) { m_Force = n; }
	rvector GetPosMod() const { return m_PosMod; }
	void SetPosMod(const rvector& pos) { m_PosMod = pos; }
	rvector GetDirMod() const { return m_DirMod; }
	void SetDirMod(const rvector& dir) { m_DirMod = dir; }

	void ProcessShot(rvector& pos, rvector& dir,ZObject* pOwner) const;
};

class ZActorActionSummon
{
	string m_Name;
	float m_fDelay;
	int m_Range;
	int m_Angle;
	int m_Route;
	bool m_AdjustPlayerNum;
	string m_ItemName;//???? right?wrong? not sure how this works

public:
	ZActorActionSummon()
		: m_Name("")
		, m_fDelay(0.f)
		, m_Range(0)
		, m_Angle(0)
		, m_Route(0)
		, m_AdjustPlayerNum(false)
		, m_ItemName("") {}

	const char* GetName() const { return m_Name.c_str(); }
	void SetName(const char* name) { m_Name = name; }
	float GetDelay() const { return m_fDelay; }
	void SetDelay(float f) { m_fDelay = f; }
	int GetRange() const { return m_Range; }
	void SetRange(int n) { m_Range = n; }
	int GetAngle() const { return m_Angle; }
	void SetAngle(int n) { m_Angle = n; }
	int GetRoute() const { return m_Route; }
	void SetRoute(int n) { m_Route = n; }
	bool GetAdjustPlayerNum()const  { return m_AdjustPlayerNum; }
	void SetAdjustPlayerNum(bool b) { m_AdjustPlayerNum = b; }
	const char* GetItemDrop() const { return m_ItemName.c_str(); }
	void SetItemDrop(const char* name) { m_ItemName = name; }

	void ProcessSummon(const MUID& uidTarget, const MUID& uidOwner, int Count,
		const char* name, const rvector& pos, const rvector& dir , int route) const;
};

class ZActorActionSound
{
	float m_Delay;
	string m_Sound;

public:
	ZActorActionSound()
		: m_Delay(0.f)
		, m_Sound("")
	{}

	float GetDelay() const { return m_Delay; }
	void SetDelay(float f){ m_Delay = f; }
	const char* GetName() const { return m_Sound.c_str(); }
	void SetName(const char* name) { m_Sound = name; }

	void ProcessSound(const rvector& pos, const char* soundName) const;

};

class ZActorActionEffect
{
	float m_Delay;
	string m_MeshName;
	_RMeshPartsPosInfoType m_PartsType;
	rvector m_PosMod;
	rvector m_DirMod;
	float m_Scale;
	int   m_ZAxis;
public:
	ZActorActionEffect()
		: m_Delay(0.f)
		, m_MeshName("")
		, m_PartsType(_RMeshPartsPosInfoType::eq_parts_pos_info_end)
		, m_PosMod(0, 0, 0)
		, m_DirMod(0, 0, 0) {}

	float GetDelay() const { return m_Delay; }
	void SetDelay(float f) { m_Delay = f; }
	const char* GetMeshName() const { return m_MeshName.c_str(); }
	void SetMeshName(const char* name) { m_MeshName = name; }
	_RMeshPartsPosInfoType GetPosPartsType() const { return m_PartsType; }
	void SetPosPartsType(_RMeshPartsPosInfoType parts) { m_PartsType = parts; }
	rvector GetPosMod() const { return m_PosMod; }
	void SetPosMod(const rvector& pos) { m_PosMod = pos; }
	rvector GetDirMod() const { return m_DirMod; }
	void SetDirMod(const rvector& dir) { m_DirMod = dir; }
	float GetScale() const { return m_Scale; }
	void SetScale(float scale) { m_Scale = scale; }
	int GetZAxis() const { return m_ZAxis; }
	void SetZAxis(int n) { m_ZAxis = n; }

	void ProcessEffect(const rvector& pos, const rvector& dir, const float scale, const char* meshName, const MUID& uidOwner,const float delay) const;

};
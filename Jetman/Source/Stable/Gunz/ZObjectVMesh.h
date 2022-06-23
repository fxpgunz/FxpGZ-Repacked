#pragma once

#include "RVisualMesh.h"

class ZActorBase;
class ZCharacter;

// 이 클래스는 RVisualMesh를 래핑해서 RVisualMesh를 직접 사용하는 코드에 
// 테스트를 위한 mock을 끼워넣을 수 있는 여지를 준다. 로직과 렌더링을 분리하기 위한 중간 역할

class ZObjectVMesh
{
	RVisualMesh* m_pVMesh;
	bool m_bOwnVMesh;

public:
	ZObjectVMesh();
	virtual ~ZObjectVMesh();

public:
	virtual void InitOwnVMesh();
	virtual void InitNpcMesh(ZActorBase* pActor, const char* szMeshName);
	virtual int InitCharacterMesh(ZCharacter* pCharacter, char* szMeshName);


	virtual bool Create(RMesh* pMesh) { return m_pVMesh->Create(pMesh); }
	virtual void Destroy() { m_pVMesh->Destroy(); }

	virtual void SetMesh(RMesh* pMesh) { m_pVMesh->SetMesh(pMesh); }
	virtual RMesh* GetMesh() { return m_pVMesh->GetMesh(); }
	virtual void SetVisualMeshToMesh() { m_pVMesh->GetMesh()->SetVisualMesh(m_pVMesh); }

	virtual rmatrix GetCurrentWeaponPositionMatrix( bool right=false ) { return m_pVMesh->GetCurrentWeaponPositionMatrix(right); }
	virtual rvector GetCurrentWeaponPosition( bool right=false ) { return m_pVMesh->GetCurrentWeaponPosition(right); }
	virtual rvector GetBipTypePosition(RMeshPartsPosInfoType type) { return m_pVMesh->GetBipTypePosition(type); }

	virtual rvector GetFootPosition() { return m_pVMesh->GetFootPosition(); }

	virtual rvector	GetHeadPosition() { return m_pVMesh->GetHeadPosition(); }
	virtual rvector	GetRFootPosition() { return m_pVMesh->GetRFootPosition(); }
	virtual rvector	GetLFootPosition() { return m_pVMesh->GetLFootPosition(); }
	virtual rvector	GetRootPosition() { return m_pVMesh->GetRootPosition(); }

	virtual const D3DXMATRIX& GetWeaponDummyMatrix(WeaponDummyType type) { return m_pVMesh->GetWeaponDummyMatrix(type); }
	virtual bool GetWeaponDummyMatrix(WeaponDummyType type,rmatrix* mat,bool bLeft) { return m_pVMesh->GetWeaponDummyMatrix(type, mat, bLeft); }
	virtual bool GetWeaponDummyPos(WeaponDummyType type,rvector* pos,bool bLeft) { return m_pVMesh->GetWeaponDummyPos(type, pos, bLeft); }

	virtual void SetEnChantType(REnchantType EnchantType) { m_pVMesh->SetEnChantType(EnchantType); }
	virtual void GetWeaponPos(rvector* p,bool bLeft=false) { m_pVMesh->GetWeaponPos(p, bLeft); }
	virtual float GetWeaponSize() { return m_pVMesh->GetWeaponSize(); }
	virtual bool IsDoubleWeapon() { return m_pVMesh->IsDoubleWeapon(); }

	virtual int GetLastWeaponTrackPos(rvector* pOutVec) { return m_pVMesh->GetLastWeaponTrackPos(pOutVec); }
	virtual void SetDrawTracksMotion(int i, bool b) { m_pVMesh->SetDrawTracksMotion(i, b); }
	virtual void SetDrawTracks(bool s) { m_pVMesh->SetDrawTracks(s); }


	virtual void AddWeapon(RWeaponMotionType type,RMesh* pVMesh,RAnimation* pAni=NULL) { m_pVMesh->AddWeapon(type, pVMesh, pAni); }
	virtual void RemoveWeapon(RWeaponMotionType type) { m_pVMesh->RemoveWeapon(type); }
	virtual void RemoveAllWeapon() { m_pVMesh->RemoveAllWeapon(); }

	virtual int				GetSetectedWeaponMotionID() { return m_pVMesh->GetSetectedWeaponMotionID(); }
	virtual void			SelectWeaponMotion(RWeaponMotionType type) { m_pVMesh->SelectWeaponMotion(type); }
	virtual RVisualMesh*	GetSelectWeaponVMesh() { return m_pVMesh->GetSelectWeaponVMesh(); }
	virtual RWeaponMotionType GetSelectWeaponMotionType() { return m_pVMesh->GetSelectWeaponMotionType(); }


	virtual void SetLight(int index,D3DLIGHT9* light,bool ShaderOnly) {	m_pVMesh->SetLight(index, light, ShaderOnly); }


	virtual AniFrameInfo* GetFrameInfo(RAniMode mode) { return m_pVMesh->GetFrameInfo(mode); }

	virtual bool SetAnimation(RAnimation* pAniSet,bool b=false) { return m_pVMesh->SetAnimation(pAniSet, b); }
	virtual bool SetAnimation(char* ani_name,bool b=false)  { return m_pVMesh->SetAnimation(ani_name, b); }
	virtual bool SetAnimation(RAniMode animode,const char* ani_name,bool b=false) { return m_pVMesh->SetAnimation(animode, ani_name, b); }
	virtual bool SetAnimation(RAniMode animode,RAnimation* pAniSet,bool b=false) { return m_pVMesh->SetAnimation(animode, pAniSet, b); }

	virtual bool SetReserveAnimation(RAnimation* pAniSet,int tick) { return m_pVMesh->SetReserveAnimation(pAniSet, tick); }
	virtual bool SetReserveAnimation(char* ani_name,int tick) { return m_pVMesh->SetReserveAnimation(ani_name, tick); }
	virtual bool SetReserveAnimation(RAniMode animode,RAnimation* pAniSet,int tick) { return m_pVMesh->SetReserveAnimation(animode, pAniSet, tick); }
	virtual bool SetReserveAnimation(RAniMode animode,char* ani_name,int tick) { return m_pVMesh->SetReserveAnimation(animode, ani_name, tick); }

	virtual void Play(RAniMode amode = ani_mode_lower) { m_pVMesh->Play(amode); }
	virtual void Stop(RAniMode amode = ani_mode_lower) { m_pVMesh->Stop(amode); }
	virtual void Pause(RAniMode amode = ani_mode_lower) { m_pVMesh->Pause(amode); }
	virtual bool isOncePlayDone(RAniMode amode = ani_mode_lower) { return m_pVMesh->isOncePlayDone(amode); }
	virtual void SetSpeed(float s,float s_up=4.8f) { m_pVMesh->SetSpeed(s, s_up); }


	virtual const rvector& GetRotXYZ()		{ return m_pVMesh->GetRotXYZ(); }
	virtual void SetRotXYZ(const rvector& v){ m_pVMesh->SetRotXYZ(v); }
	virtual void SetRotX(float x)			{ m_pVMesh->SetRotX(x); }
	virtual void SetRotY(float y)			{ m_pVMesh->SetRotY(y); }
	virtual void SetRotZ(float z)			{ m_pVMesh->SetRotZ(z); }


	virtual void SetVisibility(float vis) { m_pVMesh->SetVisibility(vis); }
	virtual float GetVisibility() { return m_pVMesh->GetVisibility(); }


	virtual void OnRestore() { m_pVMesh->OnRestore(); }
	virtual void OnInvalidate() { m_pVMesh->OnInvalidate(); }


	virtual void Frame() { m_pVMesh->Frame(); }
	virtual void Render(bool low = false ,bool render_buffer=false) { m_pVMesh->Render(low, render_buffer); }

	virtual RFrameTime* GetFrameTime() { return m_pVMesh->GetFrameTime(); }
	virtual int GetMaxFrame(RAniMode amode) { return m_pVMesh->GetMaxFrame(amode); }

	virtual bool IsRender() { return m_pVMesh->IsRender(); }
	virtual bool IsRenderWeapon() { return m_pVMesh->IsRenderWeapon(); }


	virtual void SetWorldMatrix(rmatrix& mat) { m_pVMesh->SetWorldMatrix(mat); }
	virtual void SetScale(rvector& v) { m_pVMesh->SetScale(v); }
	virtual const rvector& GetScale() { return m_pVMesh->GetScale(); }

	virtual bool Pick(int x,int y,RPickInfo* pInfo) { return m_pVMesh->Pick(x, y, pInfo); }
	virtual bool Pick(rvector* vInVec,RPickInfo* pInfo) { return m_pVMesh->Pick(vInVec, pInfo); }
	virtual bool Pick( const rvector& pos, const rvector& dir, RPickInfo* pInfo ) { return m_pVMesh->Pick(pos, dir, pInfo); }
	virtual bool Pick(rvector& pos,rvector& dir,rvector* v,float* t) { return m_pVMesh->Pick(pos, dir, v, t); }


	virtual void SetSpRenderMode(ALPHAPASS ePass) { m_pVMesh->SetSpRenderMode(ePass); }
	virtual bool UpdateSpWeaponFire() { return m_pVMesh->UpdateSpWeaponFire(); }


	virtual bool IsSelectWeaponGrenade() { return m_pVMesh->IsSelectWeaponGrenade(); }
	virtual bool IsAddGrenade() { return m_pVMesh->IsAddGrenade(); }

	virtual void SetAddGrenade(bool b) { m_pVMesh->SetAddGrenade(b); }
	virtual void SetGrenadeFire(bool b) { m_pVMesh->SetGrenadeFire(b); }
	virtual void SetGrenadeFireTime(DWORD time)	{ m_pVMesh->SetGrenadeFireTime(time); }


	virtual bool CreateCloth(RMeshNode* pMeshNode,float fAccel,int Numiter ) { return m_pVMesh->CreateCloth(pMeshNode, fAccel, Numiter); }
	virtual void DestroyCloth() { m_pVMesh->DestroyCloth(); }

	virtual bool ChangeChestCloth(float fAccel,int Numiter ) { return m_pVMesh->ChangeChestCloth(fAccel, Numiter); }
	virtual bool IsClothModel() { return m_pVMesh->IsClothModel(); }

	virtual void SetClothValue(bool bGame,float fDist) { return m_pVMesh->SetClothValue(bGame, fDist); }
	virtual bool isChestClothMesh()	{ return m_pVMesh->isChestClothMesh(); }
	virtual void UpdateForce(D3DXVECTOR3& force) { m_pVMesh->UpdateForce(force); }
	virtual void SetClothState(int state) { m_pVMesh->SetClothState(state); }
	virtual void SetClothForce(D3DXVECTOR3& f) { m_pVMesh->SetClothForce(f); }
	virtual void UpdateCloth() { m_pVMesh->UpdateCloth(); }
	virtual void RenderCloth() { m_pVMesh->RenderCloth(); }

	virtual void SetNPCBlendColor(D3DCOLORVALUE color) { m_pVMesh->SetNPCBlendColor(color); }

	virtual void SetCheckViewFrustum(bool b) { m_pVMesh->SetCheckViewFrustum(b); }


	virtual void SetParts(RMeshPartsType parts,RMeshNode* pMN) { m_pVMesh->SetParts(parts, pMN); }
	virtual void SetParts(RMeshPartsType parts,char* name) { m_pVMesh->SetParts(parts, name); }

	virtual void ClearParts() { m_pVMesh->ClearParts(); }

	virtual void SetBaseParts(RMeshPartsType parts) { m_pVMesh->SetBaseParts(parts); }

	virtual void SetLowPolyModel(RMesh* pMesh) { m_pVMesh->SetLowPolyModel(pMesh); }
	virtual void SetSkipRenderFaceParts(bool b) { m_pVMesh->SetSkipRenderFaceParts(b); }

	virtual void OutputDebugString_CharacterState() { m_pVMesh->OutputDebugString_CharacterState(); }

//	virtual void ShiftFugitiveValues() { m_pVMesh->ShiftFugitiveValues(); }
};
#include "stdafx.h"
#include "ZObjectVMesh.h"
#include "ZCharacter.h"
#include "ZActor.h"

ZObjectVMesh::ZObjectVMesh()
: m_pVMesh(NULL), m_bOwnVMesh(false)
{
}

ZObjectVMesh::~ZObjectVMesh()
{
	if (m_bOwnVMesh)
		delete m_pVMesh;
}

void ZObjectVMesh::InitOwnVMesh()
{
	m_pVMesh = new RVisualMesh;
	m_bOwnVMesh = true;
}

void ZObjectVMesh::InitNpcMesh(ZActorBase* pActor, const char* szMeshName)
{
	RMesh* pMesh;

	pMesh = ZGetNpcMeshMgr()->Get(szMeshName);//원하는 모델을 붙여주기..
	if(!pMesh) 
	{
		_ASSERT(0);
		mlog("InitNpcMesh() -  원하는 모델을 찾을수 없음\n");
		return;
	}

	int nVMID = ZGetGame()->m_VisualMeshMgr.Add(pMesh);
	if(nVMID==-1) mlog("InitNpcMesh() - 캐릭터 생성 실패\n");

	m_pVMesh = ZGetGame()->m_VisualMeshMgr.GetFast(nVMID);
	pActor->SetVisualMesh(this);
}

int ZObjectVMesh::InitCharacterMesh(ZCharacter* pCharacter, char* szMeshName)
{
	RMesh* pMesh;

	pMesh = ZGetMeshMgr()->Get(szMeshName);//원하는 모델을 붙여주기..

	if(!pMesh) {
		mlog("AddCharacter 원하는 모델을 찾을수 없음\n");
	}

	int nVMID = ZGetGame()->m_VisualMeshMgr.Add(pMesh);

	if(nVMID==-1) {
		mlog("AddCharacter 캐릭터 생성 실패\n");
	}

	m_pVMesh = ZGetGame()->m_VisualMeshMgr.GetFast(nVMID);
	pCharacter->SetVisualMesh(this);

	// low polygon model 붙여주기..
	// 남녀구분없이
/*
	if (m_Property.nSex == MMS_MALE) {
		strcpy(szMeshName, "heroman_low1");
	}
	else {
		strcpy(szMeshName, "heroman_low2");
	}
*/	
	return nVMID;
}
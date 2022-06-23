#include "stdafx.h"

#include "ZApplication.h"
#include "ZInterfaceBackground.h"
#include "RealSpace2.h"
#include "ZMapDesc.h"
#include "MDebug.h"
#include "ZScreenEffectManager.h"
#include "ZConfiguration.h"
#ifdef _CHURCH
#define DIR_LOGIN	"interface/church"
#endif

#ifdef _CHURCH
ZInterfaceBackground::ZInterfaceBackground()
{
	RealSpace2::RSetFileSystem(ZApplication::GetFileSystem());
	m_pChurch = NULL;
	m_pChurchEnd = NULL;
	m_pMapDesc = NULL;
	m_pPigeonMesh = NULL;
	m_nSceneNumber = NULL;
	m_nPrevSceneNumber = NULL;
	m_bReverseCam = false;
	m_bForcedChange = false;
	m_VMeshMgr = new RVisualMeshMgr;
}

ZInterfaceBackground::~ZInterfaceBackground()
{
	if (m_pChurch != NULL)
		delete m_pChurch;
	m_pChurch = NULL;

	if (m_pChurchEnd != NULL)
		delete m_pChurchEnd;
	m_pChurchEnd = NULL;

	if (m_pMapDesc != NULL)
		delete m_pMapDesc;
	m_pMapDesc = NULL;

	//if (m_pPigeonMesh != NULL)
	//	delete m_pPigeonMesh;
	//m_pPigeonMesh = NULL;

	m_VMeshMgr->DelAll();
}

void ZInterfaceBackground::SetScene(int nSceneNumber)
{

	if (m_nSceneNumber != nSceneNumber)
	{
		m_bForcedChange = false;
		m_nPrevSceneNumber = m_nSceneNumber;
		m_nSceneNumber = nSceneNumber;
		m_nSceneChangedTime = timeGetTime();
	}
	m_bReverseCam = false;
}

void ZInterfaceBackground::SetRenderState()
{
	// Set rendering state
	RSetViewport(0, 0, RGetScreenWidth(), RGetScreenHeight());
	RGetDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);					// from rbs rendering
	RGetDevice()->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);
	RGetDevice()->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);		// Texture filtering
	RGetDevice()->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	RGetDevice()->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
}

void ZInterfaceBackground::SetFogState(float fStart, float fEnd, unsigned long int color)
{
	RGetDevice()->SetRenderState(D3DRS_FOGENABLE, TRUE);
	RGetDevice()->SetRenderState(D3DRS_FOGCOLOR, color);

	RGetDevice()->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_LINEAR);
	RGetDevice()->SetRenderState(D3DRS_FOGSTART, *(DWORD *)(&fStart));
	RGetDevice()->SetRenderState(D3DRS_FOGEND, *(DWORD *)(&fEnd));
}

void ZInterfaceBackground::OnUpdate(float fElapsed)
{
	//	ZGetEffectManager()->Update(fElapsed);
	//	ZGetScreenEffectManager()->UpdateEffects();
	fElapsed = timeGetTime() + fElapsed;
}

void ZInterfaceBackground::OnInvalidate()
{
	if (m_pChurch)
		m_pChurch->OnInvalidate();

	if (m_pChurchEnd)
		m_pChurchEnd->OnInvalidate();
}


void ZInterfaceBackground::OnRestore()
{
	if (m_pChurch)
		m_pChurch->OnRestore();

	if (m_pChurchEnd)
		m_pChurchEnd->OnRestore();
}

void ZInterfaceBackground::ReverseCam()
{
	m_bReverseCam = true;
	m_nReverseCamTime = timeGetTime();
}

void ZInterfaceBackground::SetForcedChange(bool bForcedChange)
{
	m_bForcedChange = bForcedChange;
}

void ZInterfaceBackground::Draw()
{

	MWidget* pWidget = (MWidget*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("CharSelection");
	if (pWidget)
	{
		if (ZApplication::GetGameInterface()->GetState() == GUNZ_CHARSELECTION)
			pWidget->Show(true);
	}

	MPicture* pPicture = (MPicture*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("CharSel_TopScreen");
	if (pPicture)
		pPicture->SetOpacity(0);

	SetRenderState();


	double v11; // st7@9
	float v17; // eax@14
	float v18; // ecx@14
	float v19; // edx@14
	double v22; // st7@15
	double v23; // st6@15
	double v24; // st7@17
	double v27; // st7@24
	double v28; // st7@24
	float v29; // ST10_4@24
	float t; // [sp+18h] [bp-7Ch]@13
	int ia; // [sp+1Ch] [bp-78h]@9
	float fDiffa; // [sp+24h] [bp-70h]@21
	D3DXVECTOR3 cp; // [sp+28h] [bp-6Ch]@14
	D3DXVECTOR3 vDiff; // [sp+40h] [bp-54h]@2
	D3DXVECTOR3 up; // [sp+58h] [bp-3Ch]@19
	D3DXVECTOR3 at; // [sp+70h] [bp-24h]@24
	D3DXVECTOR3 cd;
	float v45; // [sp+84h] [bp-10h]@24
	float v46; // [sp+88h] [bp-Ch]@24

	rvector CamPos = rvector(0, 0, 2000.250496f);
	DWORD dwTime = timeGetTime();


	switch (m_nSceneNumber)
	{
		case LOGIN_SCENE_FALLDOWN:
		{
			if (m_nPrevSceneNumber == LOGIN_SCENE_FIXEDSKY)
			{
				RSetProjection(1.0471976, RGetScreenWidth() / (float)RGetScreenHeight(), 10.0, 15000.0);
				double nCurrChangeTime = 20000.0 - (double)(dwTime - m_nSceneChangedTime) * 5.0;
				if (nCurrChangeTime <= 2000.0)
					nCurrChangeTime = 2000.0;
				*(float *)&ia = (double)(dwTime + m_nSceneChangedTime % 0x4E20 - m_nSceneChangedTime) / nCurrChangeTime;
				v11 = 3.0 - (-m_EndCamPos.y - 3000.0) * 0.00025000001;
				if (*(float *)&ia > v11)
					*(float *)&ia = v11;
				if (m_bForcedChange == true)
					*(float *)&ia = v11;
				t = *(float *)&ia / v11;
				SetFogState((t * 8000) + 4000, (t * 8000) + 8000, 0x4073572E);
				if (t == 1.0)
				{
					rvector vEndCamPos = rvector(m_EndCamPos.x, m_EndCamPos.y, m_EndCamPos.z);
					cp.x = m_EndCamPos.x;
					cp.y = vEndCamPos.y;
					v17 = m_EndCamDir.x;
					cp.z = vEndCamPos.z;
					v18 = m_EndCamDir.y;
					v19 = m_EndCamDir.z;
					if (m_bReverseCam)
					{
						rvector vDiff = rvector(m_vCharDir.x, m_vCharDir.y, m_vCharDir.z);
						v22 = (double)(dwTime - m_nReverseCamTime);
						v23 = 140.0 - 5.0 * v22;
						if (v23 <= 140.0)
							v23 = 140.0;
						v24 = v22 * v23 * 0.001;
						if (v24 > 500.0)
							v24 = 500.0;
						up.y = vDiff.y * v24;
						up.z = vDiff.z * v24;
						vDiff.x = vDiff.x * v24 + cp.x;
						cp = vDiff;
						vDiff.y = cp.y + up.y;
						vDiff.z = cp.z + up.z;
					}
					vDiff = m_EndCamPos + m_EndCamDir;
					RSetCamera(cp, vDiff, rvector(0, 0, 1));
					D3DXMatrixTranslation(&m_matWorld, 0, 0, 0);
					RGetDevice()->SetTransform(D3DTS_WORLD, &m_matWorld);
					m_pChurchEnd->Draw();
					m_pChurchEnd->DrawObjects();
				}
				else
				{
					int n26 = 0;
					fDiffa = 0.0;
					do
					{
						int nEndOffset = 0;
						if (n26 == 3)
							nEndOffset = 3000;
						v27 = m_EndCamPos.z;
						up.x = m_EndCamPos.x - CamPos.x;

						up.y = -CamPos.y;
						cd.y = up.y * t;
						cd.z = (v27 - CamPos.z) * t;
						cp.x = up.x * t + CamPos.x;
						cp.y = cd.y + CamPos.y;
						cp.z = cd.z + CamPos.z;
						v28 = m_EndCamDir.x;
						vDiff.y = m_EndCamDir.y - 1.0;
						vDiff.z = m_EndCamDir.z - 1.5;
						v45 = vDiff.z * t;
						v46 = v28 * t;
						at.x = v46 + cp.x;
						at.y = vDiff.y * t + 1.0 + cp.y;
						at.z = v45 + 1.5 + cp.z;
						RSetCamera(cp, at, rvector(0, 0, 1));
						v29 = ((double)(fDiffa)-*(float *)&ia) * 4000.0 + (double)nEndOffset;
						D3DXMatrixTranslation(&m_matWorld, 0, (v29), 0);
						RGetDevice()->SetTransform(D3DTS_WORLD, &m_matWorld);
						if (n26 == 3)
						{
							m_pChurchEnd->Draw();
							m_pChurchEnd->DrawObjects();
						}
						else
						{
							m_pChurch->Draw();
							m_pChurch->DrawObjects();
						}
						++n26;
						fDiffa = n26;
					} while (n26 < 4);
				}
			}
			break;
		}
		case LOGIN_SCENE_FIXEDSKY:
		{
			RSetProjection(0.95993114, RGetScreenWidth() / (float)RGetScreenHeight(), 10.0, 10000.0);
			RSetCamera(CamPos, rvector(CamPos.x,CamPos.y + 1.0,CamPos.z + 1.5), rvector(0, 0, 1));
			SetFogState(4000.0, 8000.0, 0x4073572E);
			double fDiff = (timeGetTime() % 20000) * 0.000049999999;
			for (int i = 3; i >= 0; --i)
			{
				D3DXMatrixTranslation(&m_matWorld, 0, (i - fDiff) * 4000.0,0);
				RGetDevice()->SetTransform(D3DTS_WORLD, &m_matWorld);
				m_pChurch->Draw();
				m_pChurch->DrawObjects();
			}
			UpdatePigeon();
			break;
		}
		default:
			break;
	}
}

void ZInterfaceBackground::Free()
{
	if (m_pChurch != NULL)
		delete m_pChurch;
	m_pChurch = NULL;

	if (m_pChurchEnd != NULL)
		delete m_pChurchEnd;
	m_pChurchEnd = NULL;

	if (m_pMapDesc != NULL)
		delete m_pMapDesc;
	m_pMapDesc = NULL;

	if (m_pPigeonMesh != NULL)
		delete m_pPigeonMesh;
	m_pPigeonMesh = NULL;

	m_VMeshMgr->DelAll();
}

void ZInterfaceBackground::LoadMesh()
{
	if (m_pChurch == NULL)
	{
		m_pChurch = new RBspObject();

		m_pChurch->Open(DIR_LOGIN"/church_b.rs", "xml");

		m_pChurch->OptimizeBoundingBox();

		m_pChurch->SetMapObjectOcclusion(true);
	}

	if (m_pChurchEnd == NULL)
	{
		m_pChurchEnd = new RBspObject();

		m_pChurchEnd->Open(DIR_LOGIN"/church_a.rs", "xml");

		RDummyList* pDummyList = m_pChurchEnd->GetDummyList();

		for (RDummyList::iterator itor = pDummyList->begin(); itor != pDummyList->end(); ++itor)
		{
			RDummy* pDummy = *itor;

			if (_stricmp(pDummy->szName.c_str(), "wait_pos_01") == 0)
			{
				m_EndCamPos = pDummy->Position;
				m_EndCamDir = pDummy->Direction;
			}
			else if (_stricmp(pDummy->szName.c_str(), "spawn_solo_01") == 0)
			{
				m_vCharPos = pDummy->Position;
				m_vCharDir = pDummy->Direction;
			}
		}

		RMapObjectList* pMapObject = m_pChurchEnd->GetMapObjectList();

		list<ROBJECTINFO*>::iterator it;

		ROBJECTINFO* pInfo = NULL;

		for (it = pMapObject->begin(); it != pMapObject->end(); it++) {

			pInfo = (*it);

			if (!pInfo->pVisualMesh) continue;

			if (pInfo) {// ÇÊ¿äÇÏ¸é xml ·Î »©±â..
				string str = "login_obj_ef_sky02.elu";
				if (pInfo->name == str) {
					pInfo->pVisualMesh->SetUVAnimation(-0.055f, 0.f);
				}
			}
		}


		// Map description
		if (m_pMapDesc == NULL)
		{
			m_pMapDesc = new ZMapDesc();
			m_pMapDesc->Open(m_pChurchEnd);

			if (m_pMapDesc) {

				char szBuf[128];
				sprintf(szBuf, "%s/smoke.xml", DIR_LOGIN);
				m_pMapDesc->LoadSmokeDesc(szBuf);
				mlog("ZInterfaceBackground::LoadMesh pMapDesc->LoadSmokeDesc \n");
			}
		}

		m_pChurchEnd->SetMapObjectOcclusion(true);
	}

	if (m_pPigeonMesh == NULL)
	{
		//m_pPigeonMesh = ZGetMeshMgr()->Get("dove");
		//m_pPigeonMesh->ReloadAnimation();
		//m_pPigeonMesh->SetTextureRenderOnOff(true);
		//CreatePigeon(5);
	}
}

bool ZInterfaceBackground::CreatePigeon(int nNumPigeon)
{
	return false;
	for (int i = 0; i < nNumPigeon; ++i)
	{
		RVisualMesh* pMesh = new RVisualMesh();
		if (!pMesh->Create(m_pPigeonMesh))
		{
			mlog("Failed to create pigeon mesh\n");
			return false;
		}
		else
		{
			pMesh->SetCheckViewFrustum(false);
			pMesh->SetVisibility(1.0f);
			m_VMeshMgr->Add(pMesh);
			SetRandomPigeon(pMesh, 0.0f);
		}
	}
	return true;
}

//will do this logically at a later date
rmatrix GetMoveMatrix(RVisualMesh* pMesh,rmatrix& result)
{
	rvector up = rvector(0, 4.575657221408423936f, 1);
	rvector dir = rvector(0, 3.212836864f, 1);
	rvector pos = rvector(pMesh->GetPosition().x, pMesh->GetPosition().y + 4, pMesh->GetPosition().z);
	MakeWorldMatrix(&result, pos, dir, up);

	return result; 
}

void ZInterfaceBackground::UpdatePigeon()
{
	return;
	for (int i = 0; i < 5; ++i)
	{
		RVisualMesh* pMesh = m_VMeshMgr->GetFast(i);
		D3DXMATRIX result;
		if (pMesh)
		{
			DWORD dwTime = timeGetTime();
			DWORD ScaleX = (double)dwTime * 0.033333335 * pMesh->GetScale().z;
			rvector scale = rvector(pMesh->GetScale().x - ScaleX, pMesh->GetScale().y, pMesh->GetScale().z);
			pMesh->SetScale(scale);
			GetMoveMatrix(pMesh,result);
			pMesh->SetWorldMatrix(result);
			pMesh->Frame();
			if (pMesh->GetPosition().x < -1200.0)
				SetRandomPigeon(pMesh, 18000.0);
			pMesh->Render();
		}
	}
}

int ZInterfaceBackground::SetRandomPigeon(RVisualMesh* pVMesh, float fAdd)
{
	return 0;
	int result;
	if (pVMesh)
	{

		float x = (double)(rand() % 800) - 400.0;
		float speed = (double)(rand() % 3000) + fAdd + 12000.0;
		float z = 6500.0 - (double)(rand() % 200);
		pVMesh->SetPosition(rvector(x, speed, z));
		AniFrameInfo* pAniLow = pVMesh->GetFrameInfo(ani_mode_lower);
		AniFrameInfo* pAniUp = pVMesh->GetFrameInfo(ani_mode_upper);
		pVMesh->GetMesh()->SetAnimation("fly2");
		pVMesh->GetMesh()->SetFrame(pAniLow->m_nFrame, pAniUp->m_nFrame);
		pVMesh->GetMesh()->SetMeshVis(1.0f);

		pVMesh->GetMesh()->RenderFrame();
		pVMesh->Frame();
		result = 1;
	}
	else
	{
		result = -1;
	}
	return result;
}
#else

#define DIR_LOGIN	"interface/login"
ZInterfaceBackground::ZInterfaceBackground( void)
{
	RealSpace2::RSetFileSystem( ZApplication::GetFileSystem());

	m_pLogin = NULL;
	m_pMapDesc = NULL;
//	m_bShowMaietLogo = false;
}


ZInterfaceBackground::~ZInterfaceBackground( void)
{
	if ( m_pLogin != NULL)
		delete m_pLogin;
	m_pLogin = NULL;

	if ( m_pMapDesc != NULL)
		delete m_pMapDesc;
	m_pMapDesc = NULL;
}

void ZInterfaceBackground::LoadMesh( void)
{
#ifdef _FASTDEBUG
	return;
#endif

	if ( m_pLogin == NULL)
	{
		m_pLogin = new RBspObject();

#ifndef _DEBUG
		const char* descExt = "xml";
#else
		const char* descExt = "xml";
#endif
		if (!m_pLogin->Open(DIR_LOGIN"/login.rs", descExt))
			return;

		RDummyList* pDummyList = m_pLogin->GetDummyList();

		for ( RDummyList::iterator itor = pDummyList->begin();  itor != pDummyList->end();  ++itor)
		{
			RDummy* pDummy = *itor;

			if( _stricmp( pDummy->szName.c_str(), "camera_pos 01" ) == 0 )
			{
				m_vCamPosSt = pDummy->Position;
				m_vCamDirSt = pDummy->Direction;
			}
			else if( _stricmp( pDummy->szName.c_str(), "camera_pos 02" ) == 0 )
			{
				m_vCamPosEd = pDummy->Position;
				m_vCamDirEd = pDummy->Direction;
			}
			else if( _stricmp( pDummy->szName.c_str(), "spawn_solo_101" ) == 0 )
			{
				m_vCharPos = pDummy->Position;
				m_vCharDir = pDummy->Direction;
			}
		}

		for (list<ROBJECTINFO*>::iterator& itor =  ((RBspObject*)m_pLogin)->GetSkyBoxList()->m_MapObjectList.begin(); itor != ((RBspObject*)m_pLogin)->GetSkyBoxList()->m_MapObjectList.end(); ++itor)
		{
			ROBJECTINFO* skyBox = *itor;

			if (skyBox)
			{
				string str = "login_obj_ef_sky02.elu";
				if (skyBox->name == str)
				{
					skyBox->pVisualMesh->SetUVAnimation(-0.055f, 0.f);
				}
			}
		}
			

		// Map description
		if ( m_pMapDesc == NULL)
		{
			m_pMapDesc = new ZMapDesc();
			m_pMapDesc->Open( m_pLogin);

			if( m_pMapDesc ) {

				char szBuf[128];
				sprintf( szBuf, "%s/smoke.xml", DIR_LOGIN );
				m_pMapDesc->LoadSmokeDesc(szBuf);
				mlog("Load smoke description mesh.\n");
			}

		}

		m_pLogin->SetMapObjectOcclusion( true );
	}
}

void ZInterfaceBackground::Free( void)
{
	if ( m_pLogin != NULL)
		delete m_pLogin;
	m_pLogin = NULL;

	if ( m_pMapDesc != NULL)
		delete m_pMapDesc;
	m_pMapDesc = NULL;
}

void ZInterfaceBackground::OnUpdate(float fElapsed)
{
	ZGetEffectManager()->Update( fElapsed);
	ZGetScreenEffectManager()->UpdateEffects();
}

void ZInterfaceBackground::Draw(void)
{
#ifdef _FASTDEBUG
	MWidget* pWidget = (MWidget*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "CharSelection");
	if ( pWidget)
	{
		if ( ZApplication::GetGameInterface()->GetState() == GUNZ_CHARSELECTION)
			pWidget->Show( true);
	}

	MPicture* pPicture = (MPicture*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "CharSel_TopScreen");
	if ( pPicture)
		pPicture->SetOpacity( 0);

	return;
#endif

	// Set rendering state
	RSetViewport( 0, 0, RGetScreenWidth(), RGetScreenHeight());
	RGetDevice()->SetRenderState( D3DRS_CULLMODE  ,D3DCULL_CW);					// from rbs rendering
	RGetDevice()->SetRenderState( D3DRS_NORMALIZENORMALS , TRUE );
	RGetDevice()->SetSamplerState( 0, D3DSAMP_MINFILTER , D3DTEXF_LINEAR);		// Texture filtering
	RGetDevice()->SetSamplerState( 0, D3DSAMP_MAGFILTER , D3DTEXF_LINEAR);
	RGetDevice()->SetSamplerState( 0, D3DSAMP_MIPFILTER , D3DTEXF_LINEAR);

	// Set camera position and direction
    rvector vCamPos, vCamDir;
	float fForgDensity;

	switch ( m_nSceneNumber)
	{
		case  LOGIN_SCENE_FIXEDSKY :
		{
			// Fixed camera
			vCamPos = m_vCamPosSt;
			vCamDir = m_vCamDirSt;

			// Fixed fog -> black screen
			fForgDensity = 0.0f;

			break;
		}

		case  LOGIN_SCENE_MAIN:
		{
			DWORD dwClock = (timeGetTime() - m_dwClock);

			fForgDensity = dwClock * 15.0f;

			float fSeed = dwClock * 0.00035f;

			vCamPos = m_vCamPosEd + (m_vCamPosSt - m_vCamPosEd);
			vCamDir = m_vCamDirEd + (m_vCamDirSt - m_vCamDirEd);

			break;
		}

		case  LOGIN_SCENE_FALLDOWN :
		{
			// Get current clock
			DWORD dwClock = ( timeGetTime() - m_dwClock);

			/*
			// Show maiet logo
			if ( !m_bShowMaietLogo)
			{
				m_bShowMaietLogo = true;
				ZGetScreenEffectManager()->AddScreenEffect( "maiet_logo");
			}
			*/

			// Set fog density
			fForgDensity = dwClock * 15.0f;

			// Set wait time
			if ( dwClock < 500)
				dwClock = 0;
			else
				dwClock -= 500;

			// Play BGM music
			if ( dwClock > 2000)
				ZApplication::GetSoundEngine()->PlayMusic( true);


			// End of scroll camera
			float fSeed = dwClock * 0.00035f;			// Ä«¸Þ¶ó°¡ ÀüºÎ ´Ù ³»·Á¿À±â ±îÁö °É¸®´Â ½Ã°£( dwClock¿¡ °öÇØÁÖ´Â °ªÀÌ
														// ÀÛÀ»¼ö·Ï »¡¸® ³»·Á¿È)
			if ( fSeed > 3.14)
			{
				m_nSceneNumber = LOGIN_SCENE_FIXEDCHAR;
//				m_bShowMaietLogo = false;
			}


			// Move camera position & direction
			float fGain = ( cos( fSeed) + 1.0f) / 2.0f;			// 0 < fGain < 1.0
			vCamPos = m_vCamPosEd + ( m_vCamPosSt - m_vCamPosEd) * fGain;
			vCamDir = m_vCamDirEd + ( m_vCamDirSt - m_vCamDirEd) * fGain;

			//if (m_bShowMaietLogo)
			//{
			//	ZGetScreenEffectManager()->UpdateEffects();
				ZGetScreenEffectManager()->DrawEffects();
			//}

			break;
		}

		case  LOGIN_SCENE_FIXEDCHAR :
		{
			// Show menu UI
			MWidget* pWidget = (MWidget*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "CharSelection");
			if ( pWidget)
			{
				if ( ZApplication::GetGameInterface()->GetState() == GUNZ_CHARSELECTION)
					pWidget->Show( true);
			}
			MPicture* pPicture = (MPicture*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "CharSel_TopScreen");
			if ( pPicture)
				pPicture->SetOpacity( 0);

			// Play BGM music
			ZApplication::GetSoundEngine()->PlayMusic( true);

			// Fixed camera
			vCamPos = m_vCamPosEd;
			vCamDir = m_vCamDirEd;

			// Cleared fog
			fForgDensity = 50000.0f;
			break;
		}

		case  LOGIN_SCENE_SELECTCHAR :
		{
			// Get current clock
			DWORD dwClock = ( timeGetTime() - m_dwClock);
			float fGain = ( cos( dwClock * 0.0012f) + 1.0f) / 2.0f;
			#ifdef _MOVING_CAMERA_AFTER_CHAR_SEL
			vCamPos = m_vCamPosEd + ( m_vCamPosSt - m_vCamPosEd) * fGain;
			vCamDir = m_vCamDirEd + ( m_vCamDirSt - m_vCamDirEd) * fGain;
			#else
			vCamPos = m_vCamPosEd;
			vCamDir = m_vCamDirEd;
			#endif

			MPicture* pPicture = (MPicture*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "CharSel_TopScreen");
			if ( pPicture)
			{
				// Set wait time
				if ( dwClock < 2000)
					dwClock = 0;
				else
					dwClock -= 2000;

				// Set opacity of black screen
				int nOpacity = (int)( dwClock / 3.0f);
				if ( nOpacity > 255)
					nOpacity = 255;

				pPicture->SetOpacity( nOpacity);
			}

			// Cleared fog
			fForgDensity = 50000.0f;
			break;
		}
	}

	// Set camera
	RSetCamera( vCamPos, (vCamPos + vCamDir), rvector( 0, 0, 1));

	// Draw
	RSetProjection( D3DX_PI * 70 / 180, RGetScreenWidth() / (float)RGetScreenHeight(), 10.0f, 15000.0f);
	SetFogState( fForgDensity-10000.0f, fForgDensity, 0x00000000);
	D3DXMatrixTranslation( &m_matWorld, 0, 0, 0);
	RGetDevice()->SetTransform( D3DTS_WORLD, &m_matWorld);

	// Draw background
	if ( m_pLogin)
	{
		m_pLogin->Draw();
		m_pLogin->DrawObjects();
	}

	if ( m_pMapDesc)
		m_pMapDesc->DrawMapDesc();

	// Draw effects(smoke, cloud)
//	ZGetEffectManager()->Draw( timeGetTime());

	// Draw maiet logo effect
//	ZGetScreenEffectManager()->DrawEffects();
}


void ZInterfaceBackground::OnInvalidate()
{
#ifdef _FASTDEBUG
	return;
#endif

	if( m_pLogin)
		m_pLogin->OnInvalidate();
}


void ZInterfaceBackground::OnRestore()
{
#ifdef _FASTDEBUG
	return;
#endif

	if( m_pLogin)
		m_pLogin->OnRestore();
}


void ZInterfaceBackground::SetScene( int nSceneNumber)
{
	if ( nSceneNumber == m_nSceneNumber)
		return;

	if ( (nSceneNumber == LOGIN_SCENE_FALLDOWN) || (nSceneNumber == LOGIN_SCENE_SELECTCHAR))
		m_dwClock = timeGetTime();

	// ·Î°í¸¦ »Ñ·ÁÁØ´Ù

	#ifdef _MAIET_LOGO
	if( nSceneNumber == LOGIN_SCENE_FALLDOWN)
		ZGetScreenEffectManager()->AddScreenEffect( "maiet_logo");
	#endif

	#ifdef _MOVING_CAMERA_AFTER_CHAR_SEL
	if ( nSceneNumber == LOGIN_SCENE_SELECTCHAR)
	{
		m_vCamPosSt = m_vCamPosEd;
		m_vCamPosEd.y = -780;
		m_vCamPosEd.z = 60;

		m_vCamDirSt = m_vCamDirEd;
		m_vCamDirEd.x = -0.55f;
		m_vCamDirEd.z = 0.40f;
	}
	#endif

	m_nSceneNumber = nSceneNumber;
}

void ZInterfaceBackground::SetFogState( float fStart, float fEnd, unsigned long int color)
{
	RGetDevice()->SetRenderState( D3DRS_FOGENABLE, TRUE);
	RGetDevice()->SetRenderState( D3DRS_FOGCOLOR, color);

	RGetDevice()->SetRenderState( D3DRS_FOGTABLEMODE, D3DFOG_LINEAR);
	RGetDevice()->SetRenderState( D3DRS_FOGSTART, *(DWORD *)(&fStart));
	RGetDevice()->SetRenderState( D3DRS_FOGEND,   *(DWORD *)(&fEnd));
}
#endif
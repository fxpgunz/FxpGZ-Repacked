#include "stdafx.h"
#include "ZApplication.h"
#include "ZMiniMap.h"
#include "RealSpace2.h"
#include "MZFileSystem.h"
#include "ZCharacterManager.h"
#include "ZMap.h"
#include "ZGameClient.h"

#define ZMINIMAPTOK_PICTURE		"PICTURE"
#define ZMINIMAPTOK_LEFTTOP		"LEFTTOP"
#define ZMINIMAPTOK_RIGHTBOTTOM	"RIGHTBOTTOM"
#define ZMINIMAPTOK_CAMERAHEIGHT "CAMERAHEIGHT"

#define ZMINIMAP_DEFAULT_CAMERA_HEIGHT_MIN	2000
#define ZMINIMAP_DEFAULT_CAMERA_HEIGHT_MAX	10000

ZMiniMap::ZMiniMap()
{
	m_pBaseTexture = NULL;
	m_pTeamColorTexture = NULL;
	m_pPlayerTexture = NULL;

	m_fCameraHeightMin = ZMINIMAP_DEFAULT_CAMERA_HEIGHT_MIN;
	m_fCameraHeightMax = ZMINIMAP_DEFAULT_CAMERA_HEIGHT_MAX;
}

ZMiniMap::~ZMiniMap()
{
	Destroy();
}

void ZMiniMap::Destroy()
{
	if(m_pBaseTexture)
	{
		RDestroyBaseTexture(m_pBaseTexture);
		m_pBaseTexture = NULL;
	}
	if(m_pTeamColorTexture)
	{
		RDestroyBaseTexture(m_pTeamColorTexture);
		m_pTeamColorTexture = NULL;
	}
	if(m_pPlayerTexture)
	{
		RDestroyBaseTexture(m_pPlayerTexture);
		m_pPlayerTexture = NULL;
	}
}

bool ZMiniMap::Create(const char *szName)
{
	// xml 이름을 직접받자 zcombatingerface 로 옮겨가자
	rapidxml::xml_document<> doc;
	char szXMLName[256];
	char szMapPath[64];

	ZGetCurrMapPath(szMapPath);
	sprintf(szXMLName, "%s%s/%s.minimap.xml", szMapPath, szName, szName);

	MZFile mzf;
	if(!mzf.Open(szXMLName,ZGetFileSystem()))
		return false;

	char *buffer = new char[mzf.GetLength()+1];
	mzf.Read(buffer,mzf.GetLength());
	buffer[mzf.GetLength()]=0;
	if (!doc.parse<0>(buffer)) {
		mlog("error parsing %s", szName);
		delete[] buffer;
		return false;
	}


	rapidxml::xml_node<>* rootNode = doc.first_node();
	char szTextureName[256];

	for (auto itor = rootNode->first_node(); itor; itor = itor->next_sibling())
	{
		if (_stricmp(itor->name(), ZMINIMAPTOK_PICTURE) == 0)
		{
			char szMapPath[64];
			ZGetCurrMapPath(szMapPath);
			sprintf(szTextureName, "%s%s/%s", szMapPath, szName, itor->value());

		}
		if (_stricmp(itor->name(), ZMINIMAPTOK_LEFTTOP) == 0)
		{
			m_LeftTop.x = atof(itor->first_node("X")->value());
			m_LeftTop.y = atof(itor->first_node("Y")->value());
		}
		if (_stricmp(itor->name(), ZMINIMAPTOK_RIGHTBOTTOM) == 0)
		{
			m_RightBottom.x = atof(itor->first_node("X")->value());
			m_RightBottom.y = atof(itor->first_node("Y")->value());
		}
		if (_stricmp(itor->name(), ZMINIMAPTOK_CAMERAHEIGHT) == 0)
		{
			m_fCameraHeightMin = atof(itor->first_node("MIN")->value());
			m_fCameraHeightMax = atof(itor->first_node("MAX")->value());
		}
	}

	delete[] buffer;
	doc.clear();
	mzf.Close();

	m_pBaseTexture = RCreateBaseTexture(szTextureName);
	if(!m_pBaseTexture) return false;

	m_pTeamColorTexture = RCreateBaseTexture("Interface/Default/Combat/minimap_teamcolor.png");
	if(!m_pTeamColorTexture) return false;

	m_pPlayerTexture = RCreateBaseTexture("Interface/Default/Combat/minimap_player.png");
	if(!m_pPlayerTexture) return false;

	return true;
}

void ZMiniMap::OnDraw(MDrawContext* pDC)
{
	if(!m_pBaseTexture) return ;

	LPDIRECT3DDEVICE9 pLPDIRECT3DDEVICE9=RGetDevice();

	pLPDIRECT3DDEVICE9->SetRenderState( D3DRS_LIGHTING, FALSE);

	struct MINIMAPCUSTOMVERTEX{
		FLOAT	x, y, z ;
		FLOAT	tu, tv;
	};

#define D3DFVF_MINIMAPCUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_TEX1)


	MINIMAPCUSTOMVERTEX Sprite[4] = 
	{
		{ m_LeftTop.x		,m_LeftTop.y	,0,	0,0 },
		{ m_RightBottom.x	,m_LeftTop.y	,0,	1,0 },
		{ m_RightBottom.x	,m_RightBottom.y,0,	1,1 },
		{ m_LeftTop.x		,m_RightBottom.y,0,	0,1 },
	};

	RSetWBuffer(false);

	pLPDIRECT3DDEVICE9->SetSamplerState( 0, D3DSAMP_MAGFILTER , D3DTEXF_LINEAR);
	pLPDIRECT3DDEVICE9->SetSamplerState( 0, D3DSAMP_MINFILTER , D3DTEXF_LINEAR);
	
	pLPDIRECT3DDEVICE9->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	pLPDIRECT3DDEVICE9->SetTextureStageState( 0, D3DTSS_COLORARG1 , D3DTA_TEXTURE );
	pLPDIRECT3DDEVICE9->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
	pLPDIRECT3DDEVICE9->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	pLPDIRECT3DDEVICE9->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );

	pLPDIRECT3DDEVICE9->SetFVF(D3DFVF_MINIMAPCUSTOMVERTEX);
	pLPDIRECT3DDEVICE9->SetTexture( 0, m_pBaseTexture->GetTexture() );

	// view , projection 은 이미 설정되어있다는 가정하에,

	rmatrix World;
	D3DXMatrixIdentity(&World);
	pLPDIRECT3DDEVICE9->SetTransform(D3DTS_WORLD, &World);

	RUpdateCamera();

	pLPDIRECT3DDEVICE9->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, Sprite, sizeof(MINIMAPCUSTOMVERTEX));

	pLPDIRECT3DDEVICE9->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	pLPDIRECT3DDEVICE9->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pLPDIRECT3DDEVICE9->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	pLPDIRECT3DDEVICE9->SetTextureStageState( 0, D3DTSS_COLORARG1 , D3DTA_TEXTURE );
	pLPDIRECT3DDEVICE9->SetTextureStageState( 0, D3DTSS_COLORARG2 , D3DTA_TFACTOR );

	pLPDIRECT3DDEVICE9->SetTextureStageState( 0, D3DTSS_ALPHAARG1 , D3DTA_TEXTURE );
	pLPDIRECT3DDEVICE9->SetTextureStageState( 0, D3DTSS_ALPHAOP , D3DTOP_SELECTARG1 );

	pLPDIRECT3DDEVICE9->SetTextureStageState( 1, D3DTSS_COLOROP	, D3DTOP_DISABLE );
	pLPDIRECT3DDEVICE9->SetTextureStageState( 1, D3DTSS_ALPHAOP , D3DTOP_DISABLE );

	rboundingbox *pbb = &ZGetGame()->GetWorld()->GetBsp()->GetRootNode()->bbTree;

	MINIMAPCUSTOMVERTEX playerVer[4] = 
	{
		{ -100.f, -200.f ,0,	1,0 },
		{ 100.f	, -200.f ,0,	0,0 },
		{ 100.f	, 200.f	 ,0,	0,1 },
		{ -100.f, 200.f  ,0,	1,1 },
	};

	for (ZCharacterManager::iterator itor = ZGetCharacterManager()->begin();
		itor != ZGetCharacterManager()->end(); ++itor)
	{
		ZCharacter* pCharacter = (ZCharacter*) (*itor).second;
		if(pCharacter->IsDie()) continue;

		DWORD color = 0xfffff696;
		if(ZGetGame()->GetMatch()->IsTeamPlay())
			if(pCharacter->GetTeamID()==MMT_RED)
				color = 0xffff8080;
			else color = 0xff8080ff;

		rmatrix World;
		D3DXMatrixIdentity(&World);

		rvector pos = pCharacter->GetPosition();
		pos.z = (pos.z - pbb->minz ) / (pbb->maxz - pbb->minz);
		pos.z = 400.f * min(1.f,max(0.f,pos.z));

		rvector dir = pCharacter->GetDirection();
		dir.z = 0;
		Normalize(dir);

		rvector right;
		rvector up = rvector(0,0,1);

		CrossProduct(&right,up,dir);
		Normalize(right);

		CrossProduct(&up,dir,right);

		World._11 = right.x;	World._12 = dir.x;	World._13 = up.x;
		World._21 = right.y;	World._22 = dir.y;	World._23 = up.y;
		World._31 = right.z;	World._32 = dir.z;	World._33 = up.z;

		// 그림자
		World._41 = pos.x;		World._42 = pos.y;	World._43 = 0;
		pLPDIRECT3DDEVICE9->SetTransform(D3DTS_WORLD, &World);

		pLPDIRECT3DDEVICE9->SetRenderState(D3DRS_TEXTUREFACTOR, 0xff000000);
		pLPDIRECT3DDEVICE9->SetTextureStageState( 0, D3DTSS_COLOROP , D3DTOP_MODULATE );
		pLPDIRECT3DDEVICE9->SetTexture( 0, m_pTeamColorTexture->GetTexture() );
		pLPDIRECT3DDEVICE9->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, playerVer, sizeof(MINIMAPCUSTOMVERTEX));


		World._41 = pos.x;		World._42 = pos.y;	World._43 = pos.z;
		pLPDIRECT3DDEVICE9->SetTransform(D3DTS_WORLD, &World);

		pLPDIRECT3DDEVICE9->SetRenderState(D3DRS_TEXTUREFACTOR, color);
		pLPDIRECT3DDEVICE9->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, playerVer, sizeof(MINIMAPCUSTOMVERTEX));

		pLPDIRECT3DDEVICE9->SetTextureStageState( 0, D3DTSS_COLOROP , D3DTOP_SELECTARG1 );
		pLPDIRECT3DDEVICE9->SetTexture( 0, m_pPlayerTexture->GetTexture() );
		pLPDIRECT3DDEVICE9->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, playerVer, sizeof(MINIMAPCUSTOMVERTEX));


	}

	for (ZCharacterManager::iterator itor = ZGetCharacterManager()->begin();
		itor != ZGetCharacterManager()->end(); ++itor)
	{
		ZCharacter* pCharacter = (ZCharacter*) (*itor).second;

		if (!pCharacter->IsVisible()) continue;
		if (pCharacter->IsDie()) continue;
		
		rvector pos, screen_pos;
		pos = pCharacter->GetPosition();

		rvector dir = pCharacter->GetDirection();
		dir.z = 0;
		Normalize(dir);

		pos += -150.f*dir;
		pos.z = 0;
		screen_pos = RGetTransformCoord(pos);

		MCOLOR _color = MCOLOR(0xfffff696);

		MFont *pFont=NULL;

		if(pCharacter->IsAdminName()) {
			pFont = MFontManager::Get("FONTa12_O1Org");
			pDC->SetColor(MCOLOR(ZCOLOR_ADMIN_NAME));
		}
		else {
			pFont = MFontManager::Get("FONTa12_O1Blr");
			if(ZGetGame()->GetMatch()->IsTeamPlay())
				if(pCharacter->GetTeamID()==MMT_RED)
					pFont = MFontManager::Get("FONTa12_O1Red");
			pDC->SetColor(_color);
		}

//		pFont = MFontManager::Get("FONTa10b");
		pDC->SetBitmap(NULL);

		/////// Outline Font //////////
//				MFont *pFont=MFontManager::Get("FONTa12_O1Blr");
		if (pFont == NULL) _ASSERT(0);
		pDC->SetFont(pFont);
		///////////////////////////////

		int x = screen_pos.x - pDC->GetFont()->GetWidth(pCharacter->GetCharName()) / 2;
		int y = screen_pos.y;// + MGetWorkspaceHeight()*0.05;

		/*
		if(ZGetGameInterface()->GetCombatInterface()->GetObserver()->GetTargetCharacter() == pCharacter) {

			int nWidth = pDC->GetFont()->GetWidth(pCName);
			pDC->SetColor(MCOLOR(0xffffffff));

#define MARGIN	5
			pDC->FillRectangle(x-MARGIN,y-MARGIN,nWidth+MARGIN*2,pDC->GetFont()->GetHeight()+MARGIN*2);
		}
		*/

		pDC->Text(x, y, pCharacter->GetCharName());
	}
}
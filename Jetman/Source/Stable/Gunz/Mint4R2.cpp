#include "stdafx.h"

#include "Mint4R2.h"
#include "MWidget.h"
#include "Mint.h"

_USING_NAMESPACE_REALSPACE2

static int Floorer2PowerSize(int v)
{
	if(v<=2) return 2;
	else if(v<=4) return 4;
	else if(v<=8) return 8;
	else if(v<=16) return 16;
	else if(v<=32) return 32;
	else if(v<=64) return 64;
	else if(v<=128) return 128;
	else if(v<=256) return 256;
	else if(v<=512) return 512;
	else if(v<=1024) return 1024;

	_ASSERT(FALSE);	// Too Big!

	return 2;
}

MDrawContextR2::MDrawContextR2(LPDIRECT3DDEVICE9 pLPDIRECT3DDEVICE9)
{
	m_pLPDIRECT3DDEVICE9 = pLPDIRECT3DDEVICE9;
}

MDrawContextR2::~MDrawContextR2(void)
{
}

struct PIXEL2DVERTEX { D3DXVECTOR4 p;   DWORD color; };
#define D3DFVF_PIXEL2DVERTEX (D3DFVF_XYZRHW|D3DFVF_DIFFUSE)

void MDrawContextR2::SetPixel(int x, int y, MCOLOR& color)
{
	m_pLPDIRECT3DDEVICE9->SetFVF(D3DFVF_PIXEL2DVERTEX);
	m_pLPDIRECT3DDEVICE9->SetPixelShader(NULL);

	static PIXEL2DVERTEX p;
	p.p.x = float(x);
	p.p.y = float(y);
	p.p.z = 0;
	p.p.w = 1;
	p.color = color.GetARGB();

	m_pLPDIRECT3DDEVICE9->DrawPrimitiveUP(D3DPT_POINTLIST, 1, &p, sizeof(PIXEL2DVERTEX));
}

void MDrawContextR2::HLine(int x, int y, int len)
{
	Line(x, y, x+len, y);
}

void MDrawContextR2::VLine(int x, int y, int len)
{
	Line(x, y, x, y+len);
}

void MDrawContextR2::Line(int sx, int sy, int ex, int ey)
{
	m_pLPDIRECT3DDEVICE9->SetFVF(D3DFVF_PIXEL2DVERTEX);
	m_pLPDIRECT3DDEVICE9->SetPixelShader(NULL);

	sx += m_Origin.x;
	sy += m_Origin.y;
	ex += m_Origin.x;
	ey += m_Origin.y;

	DWORD argb = m_Color.GetARGB();
	static PIXEL2DVERTEX p[2];
	p[0].p.x = float(sx);
	p[0].p.y = float(sy);
	p[0].p.z = 0;
	p[0].p.w = 1;
	p[0].color = argb;
	p[1].p.x = float(ex);
	p[1].p.y = float(ey);
	p[1].p.z = 0;
	p[1].p.w = 1;
	p[1].color = argb;

    m_pLPDIRECT3DDEVICE9->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE);
    m_pLPDIRECT3DDEVICE9->SetRenderState( D3DRS_SRCBLEND,   D3DBLEND_SRCALPHA );
    m_pLPDIRECT3DDEVICE9->SetRenderState( D3DRS_DESTBLEND,  D3DBLEND_INVSRCALPHA );
    m_pLPDIRECT3DDEVICE9->SetRenderState( D3DRS_ALPHATESTENABLE,  FALSE );
	m_pLPDIRECT3DDEVICE9->SetRenderState( D3DRS_ZWRITEENABLE,     FALSE );

	m_pLPDIRECT3DDEVICE9->SetTexture(0, NULL);

	m_pLPDIRECT3DDEVICE9->DrawPrimitiveUP(D3DPT_LINELIST, 1, p, sizeof(PIXEL2DVERTEX));
}

void MDrawContextR2::Rectangle(int x, int y, int cx, int cy)
{
	x += m_Origin.x;
	y += m_Origin.y;

	DWORD argb = m_Color.GetARGB();
	static PIXEL2DVERTEX p[5];
	p[0].p.x = float(x);
	p[0].p.y = float(y);
	p[0].p.z = 0;
	p[0].p.w = 1;
	p[0].color = argb;
	p[1].p.x = float(x+cx);
	p[1].p.y = float(y);
	p[1].p.z = 0;
	p[1].p.w = 1;
	p[1].color = argb;
	p[2].p.x = float(x+cx);
	p[2].p.y = float(y+cy);
	p[2].p.z = 0;
	p[2].p.w = 1;
	p[2].color = argb;
	p[3].p.x = float(x);
	p[3].p.y = float(y+cy);
	p[3].p.z = 0;
	p[3].p.w = 1;
	p[3].color = argb;
	p[4].p.x = float(x);
	p[4].p.y = float(y);
	p[4].p.z = 0;
	p[4].p.w = 1;
	p[4].color = argb;

	m_pLPDIRECT3DDEVICE9->SetFVF(D3DFVF_PIXEL2DVERTEX);
	m_pLPDIRECT3DDEVICE9->SetPixelShader(NULL);

    m_pLPDIRECT3DDEVICE9->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE);
    m_pLPDIRECT3DDEVICE9->SetRenderState( D3DRS_SRCBLEND,   D3DBLEND_SRCALPHA );
    m_pLPDIRECT3DDEVICE9->SetRenderState( D3DRS_DESTBLEND,  D3DBLEND_INVSRCALPHA );
    m_pLPDIRECT3DDEVICE9->SetRenderState( D3DRS_ALPHATESTENABLE,  FALSE );
	m_pLPDIRECT3DDEVICE9->SetRenderState( D3DRS_ZWRITEENABLE,     FALSE );

	m_pLPDIRECT3DDEVICE9->SetTexture(0, NULL);

	m_pLPDIRECT3DDEVICE9->DrawPrimitiveUP(D3DPT_LINESTRIP, 4, p, sizeof(PIXEL2DVERTEX));

}

void MDrawContextR2::FillRectangleW(int x, int y, int cx, int cy)
{
#ifndef _EXTRESOLUTION
    if( RGetIsWidthScreen() )
    {
        x = ( 800*x+80*RGetScreenWidth() )/960.f;//( x/float(RGetScreenWidth()) *800 + 80 )/960 * RGetScreenWidth();
        cx = int(cx*800.f/960.f);
    }
#endif


    FillRectangle( x,  y,  cx,  cy);
}


void MDrawContextR2::FillRectangle(int x, int y, int cx, int cy)
{
	m_pLPDIRECT3DDEVICE9->SetFVF(D3DFVF_PIXEL2DVERTEX);
	m_pLPDIRECT3DDEVICE9->SetPixelShader(NULL);

	x += m_Origin.x;
	y += m_Origin.y;

	DWORD argb = m_Color.GetARGB();
	static PIXEL2DVERTEX p[4];
	p[0].p.x = float(x);
	p[0].p.y = float(y);
	p[0].p.z = 0;
	p[0].p.w = 1;
	p[0].color = argb;

	p[1].p.x = float(x+cx);
	p[1].p.y = float(y);
	p[1].p.z = 0;
	p[1].p.w = 1;
	p[1].color = argb;

	p[2].p.x = float(x+cx);
	p[2].p.y = float(y+cy);
	p[2].p.z = 0;
	p[2].p.w = 1;
	p[2].color = argb;

	p[3].p.x = float(x);
	p[3].p.y = float(y+cy);
	p[3].p.z = 0;
	p[3].p.w = 1;
	p[3].color = argb;

    m_pLPDIRECT3DDEVICE9->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE);
    m_pLPDIRECT3DDEVICE9->SetRenderState( D3DRS_SRCBLEND,   D3DBLEND_SRCALPHA );
    m_pLPDIRECT3DDEVICE9->SetRenderState( D3DRS_DESTBLEND,  D3DBLEND_INVSRCALPHA );
	m_pLPDIRECT3DDEVICE9->SetRenderState( D3DRS_ALPHATESTENABLE,  FALSE );
	m_pLPDIRECT3DDEVICE9->SetRenderState( D3DRS_ZWRITEENABLE,     FALSE );
	m_pLPDIRECT3DDEVICE9->SetRenderState( D3DRS_FOGENABLE, FALSE );
	m_pLPDIRECT3DDEVICE9->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	
	m_pLPDIRECT3DDEVICE9->SetTexture(0, NULL);
	 
	HRESULT hr = m_pLPDIRECT3DDEVICE9->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, p, sizeof(PIXEL2DVERTEX));
	_ASSERT(hr==D3D_OK);
}

void MDrawContextR2::Draw(MBitmap *pBitmap, int x, int y, int w, int h, int sx, int sy, int sw, int sh, bool bMirrorX, bool bMirrorY )
{
	if(pBitmap==NULL) return;

//	_ASSERT(pBitmap->m_nTypeID==MINT_R2_CLASS_TYPE);

	// clip rect 넘어가는 것은 컬링

	if(x+m_Origin.x>m_Clip.x+m_Clip.w || y+m_Origin.y>m_Clip.y+m_Clip.h || x+w+m_Origin.x<m_Clip.x || y+h+m_Origin.y<m_Clip.y) return;

	//MCOLOR color(0xFF, 0xFF, 0xFF, m_nOpacity);

	//pBitmap->Draw((float)x+m_Origin.x, (float)y+m_Origin.y, (float)w, (float)h, (float)sx, (float)sy, (float)sw, (float)sh, color.GetARGB(), m_Effect);
	
	MCOLOR color(m_BitmapColor.r,m_BitmapColor.g,m_BitmapColor.b,m_nOpacity);
//	MCOLOR color(255,255,255,m_nOpacity);
	((MBitmapR2*)pBitmap)->Draw((float)x+m_Origin.x, (float)y+m_Origin.y, (float)w, (float)h, (float)sx, (float)sy, (float)sw, (float)sh, color.GetARGB(), m_Effect, bMirrorX, bMirrorY);
}


void MDrawContextR2::DrawEx(int tx1, int ty1, int tx2, int ty2, int tx3, int ty3, int tx4, int ty4)
{
	MBitmapR2* pBitmap = (MBitmapR2*)m_pBitmap;
	if(pBitmap==NULL) return;
	//_ASSERT(pBitmap->m_nTypeID==MINT_R2_CLASS_TYPE);
	MCOLOR color(0xFF, 0xFF, 0xFF, m_nOpacity);

	pBitmap->DrawEx((float)tx1+m_Origin.x, (float)ty1+m_Origin.y,
		            (float)tx2+m_Origin.x, (float)ty2+m_Origin.y,
					(float)tx3+m_Origin.x, (float)ty3+m_Origin.y,
					(float)tx4+m_Origin.x, (float)ty4+m_Origin.y, color.GetARGB(), m_Effect);
}

bool MDrawContextR2::BeginFont()
{
	MFontR2* pFont = (MFontR2*)m_pFont;

	if(m_pFont==NULL)
		pFont = (MFontR2*)MFontManager::Get(NULL);

	//_ASSERT(pFont->m_nTypeID==MINT_R2_CLASS_TYPE);

	return pFont->m_Font.BeginFont();
}

bool MDrawContextR2::EndFont()
{
	MFontR2* pFont = (MFontR2*)m_pFont;

	if(m_pFont==NULL) 
		pFont = (MFontR2*)MFontManager::Get(NULL);

	//_ASSERT(pFont->m_nTypeID==MINT_R2_CLASS_TYPE);

	return pFont->m_Font.EndFont();
}

int MDrawContextR2::Text(int x, int y, const char* szText)
{
	MFontR2* pFont = (MFontR2*)m_pFont;

	if(m_pFont==NULL) 
		pFont = (MFontR2*)MFontManager::Get(NULL);

//	_ASSERT(pFont->m_nTypeID==MINT_R2_CLASS_TYPE);

	x += m_Origin.x;
	y += m_Origin.y;

	/*
	bool bShadow = false;

	if (pFont->m_nOutlineStyle <= 0) {	// 아웃라인폰트 아니면 그림자덧대기
		if (m_Color.r+m_Color.g+m_Color.b > 300) {
			bShadow = true;
//			pFont->m_Font.DrawText((float)x+1.0f, (float)y+1.0f, szText, MINT_ARGB(m_Color.a,0,0,0), pFont->m_fScale);
		}
	}

	pFont->m_Font.DrawText((float)x, (float)y, szText, m_Color.GetARGB(), pFont->m_fScale,bShadow,MINT_ARGB(m_Color.a,0,0,0));
*/
	DWORD dwColor = m_Color.GetARGB();
	if(pFont->m_nOutlineStyle==1)
		dwColor = 0xffffffff;

	pFont->m_Font.DrawText((float)x, (float)y, szText, dwColor , pFont->m_fScale);
	return 0;
}

void MDrawContextR2::SetClipRect(MRECT& r)
{
	MDrawContext::SetClipRect(r);

	// DX Clipping이 한픽셀 삑사리가 나서 보정
	r.w+=1;
	r.h+=1;

	if(r.x<0){
		r.w += r.x;
		r.x = 0;
	}
	if(r.y<0){
		r.h += r.y;
		r.y = 0;
	}
	if(r.x+r.w>=MGetWorkspaceWidth()) r.w = MGetWorkspaceWidth() - r.x;
	if(r.y+r.h>=MGetWorkspaceHeight()) r.h = MGetWorkspaceHeight() - r.y;

	D3DVIEWPORT9 vp;
	vp.X = r.x;
	vp.Y = r.y;
	vp.Width = r.w;
	vp.Height = r.h;
	vp.MaxZ = 1;
	vp.MinZ = 0;

	HRESULT hr = m_pLPDIRECT3DDEVICE9->SetViewport(&vp);
	_ASSERT(hr==D3D_OK);
}


MBitmapR2::MBitmapR2(void)
{
	m_nWidth = m_nHeight = 0;
	m_pLPDIRECT3DDEVICE9 = NULL;
	//m_pSprite = NULL;
	m_pTexture = NULL;

	m_bUnloadedTemporarily = false;

	m_dwStateBlock = NULL;
}

MBitmapR2::~MBitmapR2(void)
{
	Destroy();
}

bool MBitmapR2::Create(const char* szAliasName, LPDIRECT3DDEVICE9 pLPDIRECT3DDEVICE9, const char* szFileName,bool bUseFileSystem,DWORD colorkey)
{
	MBitmap::Create(szAliasName);

	m_pTexture = RCreateBaseTexture(szFileName,RTextureType_Etc,false,bUseFileSystem,colorkey);

	if(!m_pTexture) return false;

	m_bUnloadedTemporarily = false;

	m_nWidth = m_pTexture->GetWidth();
	m_nHeight = m_pTexture->GetHeight();

//	m_Texture.Create(szFileName);

	/*
	if( FAILED(D3DXCreateTextureFromFileEx(pLPDIRECT3DDEVICE9, szFileName, D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, 
				D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR, D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR, 
				0, &m_Info, NULL, &m_pTexture))) return false;
*/
	//if(FAILED(D3DXCreateSprite(pLPDIRECT3DDEVICE9, &m_pSprite))) return false;

	m_pLPDIRECT3DDEVICE9 = pLPDIRECT3DDEVICE9;

	return true;
}

/*
bool MBitmapR2::Create(const char* szAliasName, LPDIRECT3DDEVICE9 pLPDIRECT3DDEVICE9, void* pData, int nSize)
{
	MBitmap::Create(szAliasName);

	m_Texture.Create(pData,nSize);

	if( FAILED(D3DXCreateTextureFromFileInMemoryEx(pLPDIRECT3DDEVICE9,(void*)pData, nSize, D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, 
				D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR, D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR, 
//				D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_FILTER_NONE, D3DX_FILTER_NONE, 
				0, &m_Info, NULL, &m_pTexture ))) return false;
	//if(FAILED(D3DXCreateSprite(pLPDIRECT3DDEVICE9, &m_pSprite))) return false;

	m_pLPDIRECT3DDEVICE9 = pLPDIRECT3DDEVICE9;

	return true;
}
*/

void MBitmapR2::Destroy(void)
{
	if(m_pTexture)
	{
		RDestroyBaseTexture(m_pTexture);
		m_pTexture = NULL;
	}

	m_nWidth = m_nHeight = 0;
	m_bUnloadedTemporarily = false;


	//SAFE_RELEASE(m_pSprite);
//	SAFE_RELEASE(m_pTexture);

/*
	if(m_dwStateBlock!=NULL){
		m_pLPDIRECT3DDEVICE9->DeleteStateBlock(m_dwStateBlock);
		m_dwStateBlock = NULL;
	}
*/
}

void MBitmapR2::UnloadTextureTemporarily()
{
	if (!m_pTexture) return;
	if (m_bUnloadedTemporarily) return;

	m_pTexture->OnInvalidate();
	m_bUnloadedTemporarily = true;
}

int MBitmapR2::GetWidth(void)
{
	return m_nWidth;
}

int MBitmapR2::GetHeight(void)
{
	return m_nHeight;
}

struct CUSTOMVERTEX{
    FLOAT	x, y, z, rhw;
    DWORD	color;
    FLOAT	tu, tv;
};
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1)

DWORD MBitmapR2::m_dwStateBlock;

// 현재는 state block을 사용하지 않는다.
//#define USE_STATEBLOCK

void MBitmapR2::BeginState(MDrawEffect effect)
{
#ifdef USE_STATEBLOCK
	//	if(m_dwStateBlock==NULL){
	//		m_pLPDIRECT3DDEVICE9->BeginStateBlock();
#endif

	m_pLPDIRECT3DDEVICE9->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

	switch (effect)
	{
	case MDE_NORMAL:
	{
		m_pLPDIRECT3DDEVICE9->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		m_pLPDIRECT3DDEVICE9->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	}
	break;
	case MDE_ADD:
	{
		m_pLPDIRECT3DDEVICE9->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		m_pLPDIRECT3DDEVICE9->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	}
	break;
	case MDE_MULTIPLY:
	{
		m_pLPDIRECT3DDEVICE9->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ZERO);
		m_pLPDIRECT3DDEVICE9->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_SRCCOLOR);
	}
	break;
	}
	m_pLPDIRECT3DDEVICE9->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	m_pLPDIRECT3DDEVICE9->SetRenderState(D3DRS_ALPHAREF, 0x08);
	m_pLPDIRECT3DDEVICE9->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);

	const bool bFiltering = false;

	/*
			if(bFiltering==true){
				m_pLPDIRECT3DDEVICE9->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
				m_pLPDIRECT3DDEVICE9->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
				m_pLPDIRECT3DDEVICE9->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );
			}
			else{
				m_pLPDIRECT3DDEVICE9->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_NONE );
				m_pLPDIRECT3DDEVICE9->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_NONE );
				m_pLPDIRECT3DDEVICE9->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_NONE );
			}
	*/

	m_pLPDIRECT3DDEVICE9->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pLPDIRECT3DDEVICE9->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pLPDIRECT3DDEVICE9->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	m_pLPDIRECT3DDEVICE9->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_pLPDIRECT3DDEVICE9->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	m_pLPDIRECT3DDEVICE9->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	m_pLPDIRECT3DDEVICE9->SetRenderState(D3DRS_STENCILENABLE, FALSE);
	m_pLPDIRECT3DDEVICE9->SetRenderState(D3DRS_CLIPPING, TRUE);
	//		m_pLPDIRECT3DDEVICE9->SetRenderState( D3DRS_EDGEANTIALIAS,    FALSE );
	m_pLPDIRECT3DDEVICE9->SetRenderState(D3DRS_CLIPPLANEENABLE, FALSE);
	m_pLPDIRECT3DDEVICE9->SetRenderState(D3DRS_VERTEXBLEND, FALSE);
	m_pLPDIRECT3DDEVICE9->SetRenderState(D3DRS_INDEXEDVERTEXBLENDENABLE, FALSE);
	m_pLPDIRECT3DDEVICE9->SetRenderState(D3DRS_FOGENABLE, FALSE);
	m_pLPDIRECT3DDEVICE9->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	m_pLPDIRECT3DDEVICE9->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	m_pLPDIRECT3DDEVICE9->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pLPDIRECT3DDEVICE9->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	m_pLPDIRECT3DDEVICE9->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	m_pLPDIRECT3DDEVICE9->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pLPDIRECT3DDEVICE9->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
	//		m_pLPDIRECT3DDEVICE9->SetTextureStageState( 0, D3DTSS_MIPFILTER, D3DTEXF_NONE );
	m_pLPDIRECT3DDEVICE9->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
	m_pLPDIRECT3DDEVICE9->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
	//		m_pLPDIRECT3DDEVICE9->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
	//		m_pLPDIRECT3DDEVICE9->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

#ifdef USE_STATEBLOCK
//	}
//	else{
//		m_pLPDIRECT3DDEVICE9->ApplyStateBlock(m_dwStateBlock);
//	}
#endif
}
void MBitmapR2::EndState(void)
{
#ifdef USE_STATEBLOCK
//	if(m_dwStateBlock==NULL) m_pLPDIRECT3DDEVICE9->EndStateBlock(&m_dwStateBlock);
#endif
}

inline void _swap(float& a,float& b) {
	static float temp;
	temp = a;
	a = b;
	b = temp;
}

void MBitmapR2::CheckDrawMode(float* fuv)
{
	if(m_DrawMode) {

		float temp[2];

		if(m_DrawMode & MBM_FlipLR)	{//좌우 바꾸기
			_swap(fuv[0],fuv[2]);
			_swap(fuv[1],fuv[3]);
			_swap(fuv[4],fuv[6]);
			_swap(fuv[5],fuv[7]);
		}
		if(m_DrawMode & MBM_FlipUD) {//상하 바꾸기
			_swap(fuv[0],fuv[6]);
			_swap(fuv[1],fuv[7]);
			_swap(fuv[2],fuv[4]);
			_swap(fuv[3],fuv[5]);
		}
		if(m_DrawMode & MBM_RotL90) {
			temp[0] = fuv[4];temp[1] = fuv[5];
			fuv[4]  = fuv[6];fuv[5]  = fuv[7];
			fuv[6]  = fuv[0];fuv[7]  = fuv[1];
			fuv[0]  = fuv[2];fuv[1]  = fuv[3];
			fuv[2]  = temp[0];fuv[3]  = temp[1];
		}
		if(m_DrawMode & MBM_RotR90) {
			temp[0] = fuv[6];temp[1] = fuv[7];
			fuv[6]  = fuv[4];fuv[7]  = fuv[5];
			fuv[4]  = fuv[2];fuv[5]  = fuv[3];
			fuv[2]  = fuv[0];fuv[3]  = fuv[1];
			fuv[0]  = temp[0];fuv[1]  = temp[1];
		}
	}
}

void MBitmapR2::Draw(float x, float y, float w, float h, float sx, float sy, float sw, float sh, 
					 DWORD dwColor, MDrawEffect effect, bool bMirrorX, bool bMirrorY)
{
	/*
	float ftw = (float)Floorer2PowerSize(m_Info.Width);
	float fth = (float)Floorer2PowerSize(m_Info.Height);
	*/
	if(m_pTexture==NULL) return;

	// 임시로 메모리에서 내린 상태라면 복구해준다
	if(m_bUnloadedTemporarily) {
		m_pTexture->OnRestore();
		m_bUnloadedTemporarily = false;
	}

	float ftw = (float)m_pTexture->GetWidth();
	float fth = (float)m_pTexture->GetHeight();

	// 인버스 작업
	float fuv[8];
	// X축 인버스
	if (!bMirrorX) {
		fuv[0] = (sx)/ftw;		
		fuv[2] = (sx+sw)/ftw;	
		fuv[4] = (sx+sw)/ftw;	
		fuv[6] = (sx)/ftw;		
	} else {

		fuv[2] = (sx)/ftw;		
		fuv[0] = (sx+sw)/ftw;	
		fuv[6] = (sx+sw)/ftw;	
		fuv[4] = (sx)/ftw;
	}

	// Y축 인버스
	if (!bMirrorY) {
		fuv[1] = (sy)/fth;
		fuv[3] = (sy)/fth;
		fuv[5] = (sy+sh)/fth;
		fuv[7] = (sy+sh)/fth;
	} else {
		fuv[7] = (sy)/fth;
		fuv[5] = (sy)/fth;
		fuv[3] = (sy+sh)/fth;
		fuv[1] = (sy+sh)/fth;
	}

	CheckDrawMode(fuv);

	CUSTOMVERTEX Sprite[4] = {
#define ADJUST_SIZE		0.5f
#define ADJUST_SIZE2	0.0f
		{x-ADJUST_SIZE,    y-ADJUST_SIZE,    0, 1.0f, dwColor, fuv[0], fuv[1]},
		{x+w-ADJUST_SIZE2, y-ADJUST_SIZE,    0, 1.0f, dwColor, fuv[2], fuv[3]},
		{x+w-ADJUST_SIZE2, y+h-ADJUST_SIZE2, 0, 1.0f, dwColor, fuv[4], fuv[5]},
		{x-ADJUST_SIZE,    y+h-ADJUST_SIZE2, 0, 1.0f, dwColor, fuv[6], fuv[7]},
	};
	m_pLPDIRECT3DDEVICE9->SetFVF(D3DFVF_CUSTOMVERTEX);
	m_pLPDIRECT3DDEVICE9->SetPixelShader(NULL);

	m_pLPDIRECT3DDEVICE9->SetTexture(0, m_pTexture->GetTexture());

	BeginState(effect);

	HRESULT hr = m_pLPDIRECT3DDEVICE9->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, Sprite, sizeof(CUSTOMVERTEX));
	_ASSERT(hr==D3D_OK);

	EndState();
}

void MBitmapR2::DrawEx(float tx1, float ty1, float tx2, float ty2, 
			float tx3, float ty3, float tx4, float ty4, DWORD dwColor, MDrawEffect effect)
{
	if (!m_pTexture) return;

	// 임시로 메모리에서 내린 상태라면 복구해준다
	if(m_bUnloadedTemporarily) {
		m_pTexture->OnRestore();
		m_bUnloadedTemporarily = false;
	}

	float ftw = (float)m_pTexture->GetWidth();
	float fth = (float)m_pTexture->GetHeight();

	float fuv[8] = {
		0.f,0.f,
		1.f,0.f,
		1.f,1.f,
		0.f,1.f
	};

	CheckDrawMode(fuv);

	CUSTOMVERTEX Sprite[4] = {
#define ADJUST_SIZE		0.5f
#define ADJUST_SIZE2	0.0f
		{tx1-ADJUST_SIZE,  ty1-ADJUST_SIZE,  0, 1.0f, dwColor, fuv[0], fuv[1]},
		{tx2-ADJUST_SIZE2, ty2-ADJUST_SIZE,  0, 1.0f, dwColor, fuv[2], fuv[3]},
		{tx4-ADJUST_SIZE2, ty4-ADJUST_SIZE2, 0, 1.0f, dwColor, fuv[4], fuv[5]},
		{tx3-ADJUST_SIZE,  ty3-ADJUST_SIZE2, 0, 1.0f, dwColor, fuv[6], fuv[7]},
	};
	m_pLPDIRECT3DDEVICE9->SetFVF(D3DFVF_CUSTOMVERTEX);
	m_pLPDIRECT3DDEVICE9->SetPixelShader(NULL);

	m_pLPDIRECT3DDEVICE9->SetTexture(0, m_pTexture->GetTexture());

	BeginState(effect);

	HRESULT hr = m_pLPDIRECT3DDEVICE9->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, Sprite, sizeof(CUSTOMVERTEX));
	_ASSERT(hr==D3D_OK);

	EndState();
}

void MBitmapR2::OnLostDevice(void)
{
	//if(m_pSprite!=NULL) m_pSprite->OnLostDevice();
}

void MBitmapR2::OnResetDevice(void)
{
	//if(m_pSprite!=NULL) m_pSprite->OnResetDevice();
}

MFontR2::MFontR2(void)
{
	m_fScale = 1.0f;
}

MFontR2::~MFontR2(void)
{
	Destroy();
}

bool MFontR2::Create(const char* szAliasName, const char* szFontName, int nHeight, float fScale, bool bBold, bool bItalic, int nOutlineStyle, int nCacheSize, bool bAntiAlias, DWORD nColorArg1, DWORD nColorArg2)
{
	/// 폰트 생성 정보들을 저장해둔다, 별도로 다루어지는 값들은 제외 ////
	strcpy(m_creationInfo.szFontName, szFontName);
	m_creationInfo.nHeight = nHeight;
	m_creationInfo.bBold = bBold;
	m_creationInfo.bItalic = bItalic;
	m_creationInfo.nCacheSize = nCacheSize;
	//////////////////////////

	m_fScale = fScale;

	m_nOutlineStyle = nOutlineStyle;
	m_ColorArg1 = nColorArg1;
	m_ColorArg2 = nColorArg2;
	m_bAntiAlias = bAntiAlias;

	MFont::Create(szAliasName);
	return m_Font.Create(szFontName, nHeight, bBold, bItalic, nOutlineStyle, nCacheSize, bAntiAlias, nColorArg1, nColorArg2);
}

bool MFontR2::Resize(float ratio, int nMinimumHeight)
{
	int height = int(m_creationInfo.nHeight * ratio + 0.5f);
	if (height < nMinimumHeight)
		height = nMinimumHeight;

	m_Font.Destroy();
	return m_Font.Create(m_creationInfo.szFontName, 
		height,
		m_creationInfo.bBold, 
		m_creationInfo.bItalic, 
		m_nOutlineStyle, 
		m_creationInfo.nCacheSize, 
		m_bAntiAlias, 
		m_ColorArg1, 
		m_ColorArg2);
}

void MFontR2::Destroy(void)
{
	m_Font.Destroy();
}

int MFontR2::GetHeight(void)
{
	return int(m_Font.GetHeight()*m_fScale);
}

int MFontR2::GetWidth(const char* szText, int nSize)
{
	return int(m_Font.GetTextWidth(szText, nSize)*m_fScale);
}

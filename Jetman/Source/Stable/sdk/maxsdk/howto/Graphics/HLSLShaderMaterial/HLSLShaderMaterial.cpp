//
// Copyright [2009] Autodesk, Inc.  All rights reserved. 
//
// This computer source code and related instructions and comments are the
// unpublished confidential and proprietary information of Autodesk, Inc. and
// are protected under applicable copyright and trade secret law.  They may
// not be disclosed to, copied or used by any third party without the prior
// written consent of Autodesk, Inc.
//
//
#include "HLSLShaderMaterial.h"
#include "IMaterialBrowserEntryInfo.h"

#define DX_Shader9

using namespace MaxSDK::Graphics;

class HLSLShaderMaterialClassDesc : public ClassDesc2, public IMaterialBrowserEntryInfo
{
public:
	virtual int IsPublic() 							{ return TRUE; }
	virtual void* Create(BOOL loading = FALSE) 		{ return new HLSLShaderMaterial(loading); }
	virtual const TCHAR *	ClassName() 			{ return GetString(IDS_CLASS_NAME); }
	virtual SClass_ID SuperClassID() 				{ return MATERIAL_CLASS_ID; }
	virtual Class_ID ClassID() 						{ return HLSL_SHADER_MATERIAL_CLASS_ID; }
	virtual const TCHAR* Category() 				{ return GetString(IDS_CATEGORY); }
	virtual const TCHAR* InternalName() 			{ return _T("HLSLShaderMaterial"); }	// returns fixed parsable name (scripter-visible name)
	virtual HINSTANCE HInstance() 					{ return hInstance; }					// returns owning module handle
	virtual FPInterface* GetInterface(Interface_ID id);


	// From IMaterialBrowserEntryInfo
	virtual const MCHAR* GetEntryName() const			{ return GetString(IDS_CLASS_NAME); }
	virtual const MCHAR* GetEntryCategory() const;
	virtual bool IsStockObject() const				{ return true; }
	virtual Bitmap* GetEntryThumbnail()	const			{ return NULL; }

};

FPInterface* HLSLShaderMaterialClassDesc::GetInterface(Interface_ID id)
{
	if (id == IMATERIAL_BROWSER_ENTRY_INFO_INTERFACE)
	{
		return static_cast<IMaterialBrowserEntryInfo*>(this);
	}
	else
	{
		return ClassDesc2::GetInterface(id);
	}
}

const MCHAR* HLSLShaderMaterialClassDesc::GetEntryCategory() const
{
	static TSTR s_categoryInfo;
	if( s_categoryInfo.length()==0 ) {
		s_categoryInfo = GetString( IDS_CATEGORY_MPOOL_PREFIX );
		s_categoryInfo += _T("\\");
		s_categoryInfo += GetString( IDS_CATEGORY_MPOOL );
	}
	return s_categoryInfo.data();
}

ClassDesc2* GetHLSLShaderMaterialDesc() { 
	static HLSLShaderMaterialClassDesc HLSLShaderMaterialDesc;
	return &HLSLShaderMaterialDesc; 
}


enum { hlslshadermaterial_params };


enum 
{ 
	proxy_map,
	proxy_map_on,
};




static ParamBlockDesc2 hlslshadermaterial_param_blk ( hlslshadermaterial_params, _T("params"),  0, GetHLSLShaderMaterialDesc(), 
						  P_AUTO_CONSTRUCT + P_AUTO_UI, HLSLShaderMaterial::k_RefParam, 
	//rollout
	IDD_PANEL, IDS_PARAMS, 0, 0, NULL,
	// params
	proxy_map,			_T("shader"),	TYPE_TEXMAP, P_SUBANIM, IDS_PROXY_SHADER,
		p_ui,			TYPE_TEXMAPBUTTON, IDC_SHADER,
		p_subtexno, 0,
	p_end,
	proxy_map_on,		_T("shader_on"), TYPE_BOOL,0,	IDS_PROXY_SHADER_ON,
		p_default,		TRUE,
		p_ui,			TYPE_SINGLECHECKBOX, IDC_SHADER_ON,
	p_end,

	p_end
);



HLSLShaderMaterial::HLSLShaderMaterial(BOOL loading) 
{
	m_pblock = NULL;
	
	if (!loading)
	{
		Reset();
	}

	SetMtlFlag(MTL_HW_MAT_ENABLED|MTL_HW_TEX_ENABLED);
}

HLSLShaderMaterial::~HLSLShaderMaterial()
{

}

void HLSLShaderMaterial::Reset() 
{
	m_iValid.SetEmpty();
	GetHLSLShaderMaterialDesc()->MakeAutoParamBlocks(this);
}

ParamDlg* HLSLShaderMaterial::CreateParamDlg(HWND hwMtlEdit, IMtlParams *imp) 
{
	IAutoMParamDlg* masterDlg = GetHLSLShaderMaterialDesc()->CreateParamDlgs(hwMtlEdit, imp, this);
	return masterDlg;
}

Interval HLSLShaderMaterial::Validity(TimeValue t)
{
	Interval valid = FOREVER;	
	int count = NumSubTexmaps();
	for(int i = 0; i < count; ++i) {
		Texmap* subMap = GetSubTexmap(i);
		if(subMap != NULL)
			valid &= subMap->Validity(t);
	}
	return valid;
}

/*===========================================================================*\
 |	Subanim & References support
\*===========================================================================*/

RefTargetHandle HLSLShaderMaterial::GetReference(int i) 
{
	if(i==k_RefParam)
		return m_pblock;

	return NULL;
}

void HLSLShaderMaterial::SetReference(int i, RefTargetHandle rtarg) 
{
	if(i==k_RefParam)
	{
		m_pblock = (IParamBlock2 *)rtarg; 
	}
}


TSTR HLSLShaderMaterial::SubAnimName(int i) 
{
	if(m_pblock)
		return m_pblock->GetLocalName();

	return _T("");
}

Animatable* HLSLShaderMaterial::SubAnim(int i) 
{
	return m_pblock;
}

RefResult HLSLShaderMaterial::NotifyRefChanged(const Interval& changeInt, RefTargetHandle hTarget, 
   PartID& partID, RefMessage message, BOOL propagate ) 
{
	switch (message) {
		case REFMSG_CHANGE:
			m_iValid.SetEmpty();
			if (hTarget == m_pblock)
			{
				ParamID changing_param = m_pblock->LastNotifyParamID();
				hlslshadermaterial_param_blk.InvalidateUI(changing_param);
				IParamMap2* pMap = m_pblock->GetMap();
				if(pMap != NULL)
					pMap->Invalidate();
			}
			break;

		}
	return REF_SUCCEED;
}

/*===========================================================================*\
 |	Texmap get and set
 |  By default, we support none
\*===========================================================================*/

Texmap* HLSLShaderMaterial::GetSubTexmap(int i)
{
	return m_pblock->GetTexmap(proxy_map);
}

void HLSLShaderMaterial::SetSubTexmap(int i, Texmap *m)
{
	m_pblock->SetValue(proxy_map,0,m);
	if(m && this->TestMtlFlag(MTL_HW_TEX_ENABLED))
		m->SetMtlFlag(MTL_HW_TEX_ENABLED);
}

int HLSLShaderMaterial::SubTexmapOn(int i)
{
	return m_pblock->GetInt(proxy_map_on);
};

TSTR HLSLShaderMaterial::GetSubTexmapSlotName(int i)
{
	return GetString(IDS_PROXY_SHADER);
}

TSTR HLSLShaderMaterial::GetSubTexmapTVName(int i)
{
	// Return i'th sub-texture name 
	return GetSubTexmapSlotName(i);
}

ULONG HLSLShaderMaterial::Requirements(int subMtlNum) 
{

	// No two-sided to match Quicksilver, possibly transparent
	ULONG requirements = MTLREQ_TRANSP | MTLREQ_TRANSP_IN_VP | MTLREQ_UV;
	return requirements;
}

/*===========================================================================*\
 |	Standard IO
\*===========================================================================*/

#define MTL_HDR_CHUNK 0x4000

IOResult HLSLShaderMaterial::Save(ISave *isave) { 
	IOResult res;
	isave->BeginChunk(MTL_HDR_CHUNK);
	res = MtlBase::Save(isave);
	if (res!=IO_OK) return res;
	isave->EndChunk();

	return IO_OK;
	}	

IOResult HLSLShaderMaterial::Load(ILoad *iload) { 
	IOResult res;
	int id;
	while (IO_OK==(res=iload->OpenChunk())) {
		switch(id = iload->CurChunkID())  {
			case MTL_HDR_CHUNK:
				res = MtlBase::Load(iload);
				break;
			}
		iload->CloseChunk();
		if (res!=IO_OK) 
			return res;
		}

	return IO_OK;
	}


/*===========================================================================*\
 |	Updating and cloning
\*===========================================================================*/

RefTargetHandle HLSLShaderMaterial::Clone(RemapDir &remap) 
{
	HLSLShaderMaterial *mnew = new HLSLShaderMaterial(FALSE);
	*((MtlBase*)mnew) = *((MtlBase*)this); 
	mnew->ReplaceReference(HLSLShaderMaterial::k_RefParam,remap.CloneRef(m_pblock));
	ReferenceTarget * targ = GetReference(k_RefPhen);
	mnew->ReplaceReference(k_RefPhen,remap.CloneRef(targ));
	BaseClone(this, mnew, remap);
	return (RefTargetHandle)mnew;
}


void HLSLShaderMaterial::Update(TimeValue t, Interval& valid) 
{
	if (!m_iValid.InInterval(t))
	{
		int count = NumSubTexmaps();
		for(int i = 0; i < count; ++i) {
			Texmap* subMap = GetSubTexmap(i);
			if(subMap != NULL)
				subMap->Update(t, m_iValid);
		}
	}
	valid &= m_iValid;
}

/*===========================================================================*\
 |	Determine the characteristics of the material
\*===========================================================================*/

void HLSLShaderMaterial::SetAmbient(Color c, TimeValue t) {}		
void HLSLShaderMaterial::SetDiffuse(Color c, TimeValue t) {}		
void HLSLShaderMaterial::SetSpecular(Color c, TimeValue t) {}
void HLSLShaderMaterial::SetShininess(float v, TimeValue t) {}
				
Color HLSLShaderMaterial::GetAmbient(int mtlNum, BOOL backFace)
{
	return Color(0,0,0);
}

Color HLSLShaderMaterial::GetDiffuse(int mtlNum, BOOL backFace)
{
	return Color(0,0,0);
}

Color HLSLShaderMaterial::GetSpecular(int mtlNum, BOOL backFace)
{
	return Color(0,0,0);
}

float HLSLShaderMaterial::GetXParency(int mtlNum, BOOL backFace)
{
	return 0.0f;
}

float HLSLShaderMaterial::GetShininess(int mtlNum, BOOL backFace)
{
	return 0.0f;
}

float HLSLShaderMaterial::GetShinStr(int mtlNum, BOOL backFace)
{
	return 0.0f;
}

float HLSLShaderMaterial::WireSize(int mtlNum, BOOL backFace)
{
	return 0.0f;
}

		
/*===========================================================================*\
 |	Actual shading takes place
\*===========================================================================*/

void HLSLShaderMaterial::Shade(ShadeContext& sc) 
{
	
	// TODO: compute the color and transparency output returned in sc.out.
}

float HLSLShaderMaterial::EvalDisplacement(ShadeContext& sc)
{
	return 0.0f;
}

Interval HLSLShaderMaterial::DisplacementValidity(TimeValue t)
{
	Interval iv; iv.SetInfinite();
	return iv;	
}

BaseInterface *HLSLShaderMaterial::GetInterface(Interface_ID id)
{
	if ( id == IHLSL_MATERIAL_TRANSLATOR_INTERFACE_ID)
	{
		return static_cast<IHLSLMaterialTranslator*>(this);
	}
	else {
		return BaseInterface::GetInterface(id);
	}
}

bool GetParamIDByName(ParamID& paramID, const TCHAR* name, IParamBlock2* pBlock) {

	DbgAssert(pBlock != NULL);
	int count = pBlock->NumParams();
	for(int i = 0; i < count; ++i) {
		ParamID id = pBlock->IndextoID(i);
		const ParamDef& paramDef = pBlock->GetParamDef(id);
		if(_tcsicmp(name, paramDef.int_name) == 0) {
			paramID = id;
			return true;
		}
	}

	return false;
}

void HLSLShaderMaterial::CreateHLSLMaterialHandle(MaxSDK::Graphics::GraphicFeatureLevel featureLevel)
{
	TSTR pRootDir = GetCOREInterface()->GetDir(APP_MAX_SYS_ROOT_DIR);
	
	if (featureLevel == Level3_0)
	{
		TSTR hlslShaderDX9 = pRootDir + TSTR(_T("maps\\fx\\StandardFX.fx"));
		HLSLMaterialHandle hlslMaterialHandleDX9;
		bool result = hlslMaterialHandleDX9.InitializeWithFile(MSTR(hlslShaderDX9.data()));
		m_HLSLDX9MaterialHandle = hlslMaterialHandleDX9;
		if (!result)
		{
			DebugOutputString(hlslMaterialHandleDX9.GetShaderErrorInformation().data());
		}
	}
	else
	{
		TSTR hlslShaderDX11 = pRootDir + TSTR(_T("maps\\fx\\StandardFX11.fx"));
		HLSLMaterialHandle hlslMaterialHandleDX11;
		bool result = hlslMaterialHandleDX11.InitializeWithFile(MSTR(hlslShaderDX11.data()));
		m_HLSLDX11MaterialHandle = hlslMaterialHandleDX11;
		if (!result)
		{
			DebugOutputString(hlslMaterialHandleDX11.GetShaderErrorInformation().data());
		}
	}

}

bool HLSLShaderMaterial::UpdateHLSLMaterial(const TimeValue t, MaxSDK::Graphics::GraphicFeatureLevel featureLevel)
{
	HLSLMaterialHandle handle = GetHLSLMaterialHandle(featureLevel);
	handle.SetFloat4Parameter(_M("k_d"), Point4(0, 1, 0, 1));
	handle.SetBoolParameter(_M("g_TopDiffuseEnable"), TRUE);

	TSTR pRootDir = GetCOREInterface()->GetDir(APP_MAX_SYS_ROOT_DIR);
	TSTR texturePath = pRootDir + TSTR(_T("maps\\mental_mill\\default.dds"));
	m_diffuseTexture.Initialize(texturePath.data());
	handle.SetTextureParameter(_M("g_TopTexture"), m_diffuseTexture);

	return true;
}

const HLSLMaterialHandle& HLSLShaderMaterial::GetHLSLMaterialHandle(MaxSDK::Graphics::GraphicFeatureLevel featureLevel)
{
	if (featureLevel == Level3_0)
	{
		if (!m_HLSLDX9MaterialHandle.IsValid())
		{
			CreateHLSLMaterialHandle(featureLevel);
		}
		return m_HLSLDX9MaterialHandle;
	}
	else
	{
		if (!m_HLSLDX11MaterialHandle.IsValid())
		{
			CreateHLSLMaterialHandle(featureLevel);
		}
		return m_HLSLDX11MaterialHandle;
	}
}

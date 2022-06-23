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

#pragma once

#include "Max.h"
#include "resource.h"
#include "istdplug.h"
#include "iparamb2.h"
#include "iparamm2.h"

#include "IMtlRender_Compatibility.h"
#include <Graphics/IHLSLMaterialTranslator.h>

#define HLSL_SHADER_MATERIAL_CLASS_ID	Class_ID(0x849510f, 0x51cd59d2)

extern TCHAR *GetString(int id);
extern HINSTANCE hInstance;

class HLSLShaderMaterial : public Mtl, public MaxSDK::Graphics::IHLSLMaterialTranslator
{
public:

	enum
	{
		k_RefParam,
		k_RefPhen,
		k_NumRefs,	//Always last
	};

	enum
	{
		k_ShaderSubNum,
		k_NumberOfMaps,
	};


	ParamDlg *CreateParamDlg(HWND hwMtlEdit, IMtlParams *imp);
	void Update(TimeValue t, Interval& valid);
	Interval Validity(TimeValue t);
	void Reset();

	// From MtlBase and Mtl
	void SetAmbient(Color c, TimeValue t);		
	void SetDiffuse(Color c, TimeValue t);		
	void SetSpecular(Color c, TimeValue t);
	void SetShininess(float v, TimeValue t);
	Color GetAmbient(int mtlNum=0, BOOL backFace=FALSE);
	Color GetDiffuse(int mtlNum=0, BOOL backFace=FALSE);
	Color GetSpecular(int mtlNum=0, BOOL backFace=FALSE);
	float GetXParency(int mtlNum=0, BOOL backFace=FALSE);
	float GetShininess(int mtlNum=0, BOOL backFace=FALSE);		
	float GetShinStr(int mtlNum=0, BOOL backFace=FALSE);
	float WireSize(int mtlNum=0, BOOL backFace=FALSE);


	// Shade and displacement calculation
	void Shade(ShadeContext& sc);
	float EvalDisplacement(ShadeContext& sc); 
	Interval DisplacementValidity(TimeValue t); 	

	// SubTexmap access methods
	int NumSubTexmaps() {return k_NumberOfMaps;}
	Texmap* GetSubTexmap(int i);
	void SetSubTexmap(int i, Texmap *m);
	TSTR GetSubTexmapSlotName(int i);
	TSTR GetSubTexmapTVName(int i);
	int SubTexmapOn(int i);
	ULONG Requirements(int subMtlNum);

	HLSLShaderMaterial(BOOL loading);
	~HLSLShaderMaterial();


	// Loading/Saving
	IOResult Load(ILoad *iload);
	IOResult Save(ISave *isave);

	//From Animatable
	Class_ID ClassID() {return HLSL_SHADER_MATERIAL_CLASS_ID;}		
	SClass_ID SuperClassID() { return MATERIAL_CLASS_ID; }
	void GetClassName(TSTR& s) {s = GetString(IDS_CLASS_NAME);}

	RefTargetHandle Clone( RemapDir &remap );
	RefResult NotifyRefChanged(const Interval& changeInt, RefTargetHandle hTarget, 
		PartID& partID, RefMessage message, BOOL propagate);


	int NumSubs() { return 1; }
	Animatable* SubAnim(int i); 
	TSTR SubAnimName(int i);

	// TODO: Maintain the number or references here 
	int NumRefs() { return k_NumRefs; }
	RefTargetHandle GetReference(int i);
private:
	virtual void SetReference(int i, RefTargetHandle rtarg);
	
	void CreateHLSLMaterialHandle(MaxSDK::Graphics::GraphicFeatureLevel featureLevel);

public:

	int	NumParamBlocks() { return 1; }					// return number of ParamBlocks in this instance
	IParamBlock2* GetParamBlock(int i) { return m_pblock; } // return i'th ParamBlock
	IParamBlock2* GetParamBlockByID(BlockID id) { return (m_pblock->ID() == id) ? m_pblock : NULL; } // return id'd ParamBlock

	void DeleteThis() { delete this; }
	BaseInterface* GetInterface(Interface_ID id);

	//!-- From IHLSLMaterialTranslator
	virtual bool UpdateHLSLMaterial(const TimeValue t, MaxSDK::Graphics::GraphicFeatureLevel featureLevel);
	virtual const MaxSDK::Graphics::HLSLMaterialHandle& GetHLSLMaterialHandle(MaxSDK::Graphics::GraphicFeatureLevel featureLevel);

private:

		IParamBlock2	*m_pblock;	
		Interval		m_iValid;
		MaxSDK::Graphics::HLSLMaterialHandle m_HLSLDX9MaterialHandle;
		MaxSDK::Graphics::HLSLMaterialHandle m_HLSLDX11MaterialHandle;
		MaxSDK::Graphics::TextureHandle m_diffuseTexture;
};

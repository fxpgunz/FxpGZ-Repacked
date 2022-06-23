//**************************************************************************/
// Copyright (c) 1998-2007 Autodesk, Inc.
// All rights reserved.
// 
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information written by Autodesk, Inc., and are
// protected by Federal copyright law. They may not be disclosed to third
// parties or copied or duplicated in any form, in whole or in part, without
// the prior written consent of Autodesk, Inc.
//**************************************************************************/
// DESCRIPTION: Appwizard generated plugin
// AUTHOR: 
//***************************************************************************/

#include "[!output PROJECT_NAME].h"

#define [!output CLASS_NAME]_CLASS_ID	Class_ID([!output CLASSID1], [!output CLASSID2])

#define PBLOCK_REF	0

class [!output CLASS_NAME];
class [!output CLASS_NAME]Gen : public ShadowGenerator 
{ 
private:
	[!output CLASS_NAME]* mShadow;

	Matrix3 lightToWorld;
	Matrix3 worldToLight;
	Matrix3 camToLight;

	LightObject* light;
	ObjLightDesc* lightDesc;

public:
	// Default Constructor
	[!output CLASS_NAME]Gen();
	// Copy Constructor
	[!output CLASS_NAME]Gen([!output CLASS_NAME]* shad, LightObject* l,  ObjLightDesc *ld, ULONG /*flags*/);
	// Destructor
	~[!output CLASS_NAME]Gen();

	// Update the shadow generator per frame
	int Update(TimeValue t,const RendContext& rendCntxt, RenderGlobalContext *rgc, Matrix3& lightToWorld, float aspect, float param, float clipDist);
	int UpdateViewDepParams(const Matrix3& worldToCam);

	// Clean up methods
	void FreeBuffer() {}
	void DeleteThis() { delete this; }

	// Perform the shadow sampling
	float Sample(ShadeContext &sc, float x, float y, float z, float xslope, float yslope);
	float Sample(ShadeContext &sc, Point3 &norm, Color& color);
};

class [!output CLASS_NAME]Dlg : public ShadowParamDlg 
{
public:
	[!output CLASS_NAME]* mShadow;
	IParamMap2* pmap;

	// Constructor
	[!output CLASS_NAME]Dlg([!output CLASS_NAME]* shad, Interface* ip);
	~[!output CLASS_NAME]Dlg();

	void DeleteThis() { delete this; }
};

class [!output CLASS_NAME] : public [!output SUPER_CLASS_NAME]
{
public:
	//Constructor/Destructor
	[!output CLASS_NAME]();
	~[!output CLASS_NAME]();

	// Create the shadow system's UI
	virtual ShadowParamDlg* CreateShadowParamDlg(Interface *ip);
	// Create a shadow generator instance - only exists during a render
	virtual ShadowGenerator* CreateShadowGenerator(LightObject* l,  ObjLightDesc* ld, ULONG flags);

	BOOL SupportStdMapInterface() { return FALSE; }

	// Loading/Saving
	virtual IOResult Load(ILoad *iload);
	virtual IOResult Save(ISave *isave);

	//From Animatable
	virtual Class_ID ClassID() {return [!output CLASS_NAME]_CLASS_ID;}
	virtual SClass_ID SuperClassID() { return SHADOW_TYPE_CLASS_ID; }
	virtual void GetClassName(TSTR& s) {s = GetString(IDS_CLASS_NAME);}

	virtual RefTargetHandle Clone( RemapDir &remap );
	virtual RefResult NotifyRefChanged(const Interval& changeInt, RefTargetHandle hTarget, PartID& partID, RefMessage message, BOOL propagate);


	virtual int NumSubs() { return 1; }
	virtual TSTR SubAnimName(int /*i*/) { return GetString(IDS_PARAMS); }
	virtual Animatable* SubAnim(int /*i*/) { return pblock; }

	// TODO: Maintain the number or references here
	virtual int NumRefs() { return 1; }
	virtual RefTargetHandle GetReference(int i);

	virtual int	NumParamBlocks() { return 1; }					// return number of ParamBlocks in this instance
	virtual IParamBlock2* GetParamBlock(int /*i*/) { return pblock; } // return i'th ParamBlock
	virtual IParamBlock2* GetParamBlockByID(BlockID id) { return (pblock->ID() == id) ? pblock : NULL; } // return id'd ParamBlock

	virtual void DeleteThis() { delete this; }

	void ClearDialog() { mpShadowDlg = nullptr; }
protected:
	virtual void SetReference(int i, RefTargetHandle rtarg);

private:
	// Parameter block
	IParamBlock2* pblock; //ref 0
	[!output CLASS_NAME]Dlg* mpShadowDlg;
};


[!output TEMPLATESTRING_CLASSDESC]


[!if PARAM_MAPS != 0]
[!output TEMPLATESTRING_PARAMBLOCKDESC]
[!endif]


[!output CLASS_NAME]::[!output CLASS_NAME]()
	: pblock(nullptr)
	, mpShadowDlg(nullptr)
{
	Get[!output CLASS_NAME]Desc()->MakeAutoParamBlocks(this);
	DbgAssert(pblock);
}

[!output CLASS_NAME]::~[!output CLASS_NAME]()
{
	if (mpShadowDlg) 
		mpShadowDlg->mShadow = nullptr;
}

ShadowParamDlg* [!output CLASS_NAME]::CreateShadowParamDlg(Interface* ip)
{
	mpShadowDlg = new [!output CLASS_NAME]Dlg(this, ip); 
	return mpShadowDlg;
}

// Create a shadow generator instance - only exists during a render
ShadowGenerator* [!output CLASS_NAME]::CreateShadowGenerator(LightObject* l,  ObjLightDesc* ld, ULONG flags)
{
	return new [!output CLASS_NAME]Gen(this,l,ld,flags);
}

RefTargetHandle [!output CLASS_NAME]::GetReference( int i)
{
	if (i == PBLOCK_REF)
	{
		return pblock;
	}
	return nullptr;
}

void [!output CLASS_NAME]::SetReference(int i, RefTargetHandle rtarg )
{
	if (i == PBLOCK_REF)
	{
		pblock=(IParamBlock2*)rtarg;
	}
}

RefResult [!output CLASS_NAME]::NotifyRefChanged(
	const Interval& /*changeInt*/, RefTargetHandle hTarget,
	PartID& /*partID*/,  RefMessage message, BOOL /*propagate*/) 
{
	switch (message)
	{
	case REFMSG_CHANGE:
		[!output CLASS_NAME_LOWER]_param_blk.InvalidateUI();
		break;

	case REFMSG_TARGET_DELETED:
		{
			if (hTarget == pblock)
			{
				pblock = nullptr;
			}
		}
		break;
	}
	return REF_SUCCEED;
}

IOResult [!output CLASS_NAME]::Save(ISave* /*isave*/) 
{
	#pragma message(TODO("Add code to allow plugin to save its data"))
	return IO_OK;
}

IOResult [!output CLASS_NAME]::Load(ILoad* /*iload*/) 
{ 
	#pragma message(TODO("Add code to allow plugin to load its data"))
	return IO_OK;
}

RefTargetHandle [!output CLASS_NAME]::Clone( RemapDir &remap )
{
	[!output CLASS_NAME]* newObj = new [!output CLASS_NAME]();
	newObj->ReplaceReference(0,remap.CloneRef(pblock));
	BaseClone(this, newObj, remap);
	return (RefTargetHandle)newObj;
}

/*===========================================================================*\
 | [!output CLASS_NAME]Gen
\*===========================================================================*/


[!output CLASS_NAME]Gen::[!output CLASS_NAME]Gen()
	: mShadow(nullptr)
	, light(nullptr)
	, lightDesc(nullptr)
{ }

[!output CLASS_NAME]Gen::[!output CLASS_NAME]Gen([!output CLASS_NAME]* shad, LightObject* l,  ObjLightDesc *ld, ULONG /*flags*/)
	: mShadow(shad)
	, light(l)
	, lightDesc(ld)
{ }

[!output CLASS_NAME]Gen::~[!output CLASS_NAME]Gen()
{
	FreeBuffer();
}

int [!output CLASS_NAME]Gen::Update(TimeValue /*t*/,const RendContext& /*rendCntxt*/,RenderGlobalContext* /*rgc*/,
		Matrix3& ltToWorld,float /*aspect*/,float /*param*/,float /*clipDist*/)
{
	lightToWorld  = ltToWorld;
	worldToLight = Inverse(lightToWorld);

	return 1;
}

int [!output CLASS_NAME]Gen::UpdateViewDepParams(const Matrix3& worldToCam) {
	camToLight = Inverse(lightToWorld * worldToCam);
	return 1;
}

float [!output CLASS_NAME]Gen::Sample(ShadeContext& /*sc*/, Point3& /*norm*/, Color& /*color*/)
{ 
	return 1.0f;
}

float [!output CLASS_NAME]Gen::Sample(ShadeContext& /*sc*/, float /*x*/, float /*y*/, float /*z*/, float /*xslope*/, float /*yslope*/)
{
	return 1.0f;
}

/*===========================================================================*\
 |	Support the Parammap UI
\*===========================================================================*/

[!output CLASS_NAME]Dlg::[!output CLASS_NAME]Dlg([!output CLASS_NAME]* shadow, Interface* ip) {
	mShadow = shadow;

	pmap = CreateCPParamMap2(
		mShadow->GetParamBlock(PBLOCK_REF),
		ip,
		hInstance,
		MAKEINTRESOURCE(IDD_PANEL),
		GetString(IDS_PARAMS),
		0);
	}

[!output CLASS_NAME]Dlg::~[!output CLASS_NAME]Dlg()
{
	if(mShadow) 
	{
		mShadow->ClearDialog();
	}
	DestroyCPParamMap2(pmap);
}


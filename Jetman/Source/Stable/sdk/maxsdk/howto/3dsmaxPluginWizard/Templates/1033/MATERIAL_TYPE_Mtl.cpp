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


#define NUM_SUBMATERIALS 1 // TODO: number of sub-materials supported by this plug-in
// Reference Indexes
// 
#define PBLOCK_REF NUM_SUBMATERIALS

class [!output CLASS_NAME] : public [!output SUPER_CLASS_NAME] {
public:
	[!output CLASS_NAME]();
	[!output CLASS_NAME](BOOL loading);
	~[!output CLASS_NAME]();


	ParamDlg* CreateParamDlg(HWND hwMtlEdit, IMtlParams* imp);
	void      Update(TimeValue t, Interval& valid);
	Interval  Validity(TimeValue t);
	void      Reset();

	void NotifyChanged();

	// From MtlBase and Mtl
	virtual void SetAmbient(Color c, TimeValue t);
	virtual void SetDiffuse(Color c, TimeValue t);
	virtual void SetSpecular(Color c, TimeValue t);
	virtual void SetShininess(float v, TimeValue t);
	virtual Color GetAmbient(int mtlNum=0, BOOL backFace=FALSE);
	virtual Color GetDiffuse(int mtlNum=0, BOOL backFace=FALSE);
	virtual Color GetSpecular(int mtlNum=0, BOOL backFace=FALSE);
	virtual float GetXParency(int mtlNum=0, BOOL backFace=FALSE);
	virtual float GetShininess(int mtlNum=0, BOOL backFace=FALSE);
	virtual float GetShinStr(int mtlNum=0, BOOL backFace=FALSE);
	virtual float WireSize(int mtlNum=0, BOOL backFace=FALSE);


	// Shade and displacement calculation
	virtual void     Shade(ShadeContext& sc);
	virtual float    EvalDisplacement(ShadeContext& sc);
	virtual Interval DisplacementValidity(TimeValue t);

	// SubMaterial access methods
	virtual int  NumSubMtls() {return NUM_SUBMATERIALS;}
	virtual Mtl* GetSubMtl(int i);
	virtual void SetSubMtl(int i, Mtl *m);
	virtual TSTR GetSubMtlSlotName(int i);
	virtual TSTR GetSubMtlTVName(int i);

	// SubTexmap access methods
	virtual int     NumSubTexmaps() {return 0;}
	virtual Texmap* GetSubTexmap(int i);
	virtual void    SetSubTexmap(int i, Texmap *m);
	virtual TSTR    GetSubTexmapSlotName(int i);
	virtual TSTR    GetSubTexmapTVName(int i);

	virtual BOOL SetDlgThing(ParamDlg* dlg);

	// Loading/Saving
	virtual IOResult Load(ILoad *iload);
	virtual IOResult Save(ISave *isave);

	// From Animatable
	virtual Class_ID ClassID() {return [!output CLASS_NAME]_CLASS_ID;}
	virtual SClass_ID SuperClassID() { return MATERIAL_CLASS_ID; }
	virtual void GetClassName(TSTR& s) {s = GetString(IDS_CLASS_NAME);}

	virtual RefTargetHandle Clone( RemapDir &remap );
	virtual RefResult NotifyRefChanged(const Interval& changeInt, RefTargetHandle hTarget, PartID& partID, RefMessage message, BOOL propagate);

	virtual int NumSubs() { return 1+NUM_SUBMATERIALS; }
	virtual Animatable* SubAnim(int i);
	virtual TSTR SubAnimName(int i);

	// TODO: Maintain the number or references here
	virtual int NumRefs() { return 1 + NUM_SUBMATERIALS; }
	virtual RefTargetHandle GetReference(int i);

	virtual int NumParamBlocks() { return 1; }					  // return number of ParamBlocks in this instance
	virtual IParamBlock2* GetParamBlock(int /*i*/) { return pblock; } // return i'th ParamBlock
	virtual IParamBlock2* GetParamBlockByID(BlockID id) { return (pblock->ID() == id) ? pblock : NULL; } // return id'd ParamBlock

	virtual void DeleteThis() { delete this; }

protected:
	virtual void SetReference(int i, RefTargetHandle rtarg);

private:
	Mtl*          submtl[NUM_SUBMATERIALS];  // Fixed size Reference array of sub-materials. (Indexes: 0-(N-1))
	IParamBlock2* pblock;					 // Reference that comes AFTER the sub-materials. (Index: N)
	
	BOOL          mapOn[NUM_SUBMATERIALS];
	float         spin;
	Interval      ivalid;
};


[!output TEMPLATESTRING_CLASSDESC]


[!if PARAM_MAPS != 0]
[!output TEMPLATESTRING_PARAMBLOCKDESC]
[!endif]


[!output CLASS_NAME]::[!output CLASS_NAME]()
	: pblock(nullptr)
{
	for (int i=0; i<NUM_SUBMATERIALS; i++) 
		submtl[i] = nullptr;
	Reset();
}

[!output CLASS_NAME]::[!output CLASS_NAME](BOOL loading)
	: pblock(nullptr)
{
	for (int i=0; i<NUM_SUBMATERIALS; i++) 
		submtl[i] = nullptr;
	
	if (!loading)
		Reset();
}

[!output CLASS_NAME]::~[!output CLASS_NAME]()
{
	DeleteAllRefs();
}


void [!output CLASS_NAME]::Reset()
{
	ivalid.SetEmpty();
	// Always have to iterate backwards when deleting references.
	for (int i = NUM_SUBMATERIALS - 1; i >= 0; i--) {
		if( submtl[i] ){
			DeleteReference(i);
			DbgAssert(submtl[i] == nullptr);
			submtl[i] = nullptr;
		}
		mapOn[i] = FALSE;
	}
	DeleteReference(PBLOCK_REF);

	Get[!output CLASS_NAME]Desc()->MakeAutoParamBlocks(this);
}



ParamDlg* [!output CLASS_NAME]::CreateParamDlg(HWND hwMtlEdit, IMtlParams *imp)
{
	IAutoMParamDlg* masterDlg = Get[!output CLASS_NAME]Desc()->CreateParamDlgs(hwMtlEdit, imp, this);

	// TODO: Set param block user dialog if necessary
	return masterDlg;
}

BOOL [!output CLASS_NAME]::SetDlgThing(ParamDlg* /*dlg*/)
{
	return FALSE;
}

Interval [!output CLASS_NAME]::Validity(TimeValue t)
{
	Interval valid = FOREVER;

	for (int i = 0; i < NUM_SUBMATERIALS; i++)
	{
		if (submtl[i])
			valid &= submtl[i]->Validity(t);
	}

	float u;
	pblock->GetValue(pb_spin,t,u,valid);
	return valid;
}

/*===========================================================================*\
 |	Sub-anim & References support
\*===========================================================================*/

RefTargetHandle [!output CLASS_NAME]::GetReference(int i)
{
	if ((i >= 0) && (i < NUM_SUBMATERIALS))
		return submtl[i];
	else if (i == PBLOCK_REF)
		return pblock;
	else
		return nullptr;
}

void [!output CLASS_NAME]::SetReference(int i, RefTargetHandle rtarg)
{
	if ((i >= 0) && (i < NUM_SUBMATERIALS))
		submtl[i] = (Mtl *)rtarg;
	else if (i == PBLOCK_REF)
	{
		pblock = (IParamBlock2 *)rtarg;
	}
}

TSTR [!output CLASS_NAME]::SubAnimName(int i)
{
	if ((i >= 0) && (i < NUM_SUBMATERIALS))
		return GetSubMtlTVName(i);
	else 
		return TSTR(_T(""));
}

Animatable* [!output CLASS_NAME]::SubAnim(int i)
{
	if ((i >= 0) && (i < NUM_SUBMATERIALS))
		return submtl[i];
	else if (i == PBLOCK_REF)
		return pblock;
	else
		return nullptr;
}

RefResult [!output CLASS_NAME]::NotifyRefChanged(const Interval& /*changeInt*/, RefTargetHandle hTarget, 
	PartID& /*partID*/, RefMessage message, BOOL /*propagate*/ ) 
{
	switch (message) {
	case REFMSG_CHANGE:
		{
			ivalid.SetEmpty();
			if (hTarget == pblock)
			{
				ParamID changing_param = pblock->LastNotifyParamID();
				[!output CLASS_NAME_LOWER]_param_blk.InvalidateUI(changing_param);
			}
		}
		break;
	case REFMSG_TARGET_DELETED:
		{
			if (hTarget == pblock)
			{
				pblock = nullptr;
			} 
			else
			{
				for (int i = 0; i < NUM_SUBMATERIALS; i++)
				{
					if (hTarget == submtl[i])
					{
						submtl[i] = nullptr;
						break;
					}
				}
			}
			break;
		}
	}
	return REF_SUCCEED;
}

/*===========================================================================*\
 |	SubMtl get and set
\*===========================================================================*/

Mtl* [!output CLASS_NAME]::GetSubMtl(int i)
{
	if ((i >= 0) && (i < NUM_SUBMATERIALS))
		return submtl[i];
	return 
		nullptr;
}

void [!output CLASS_NAME]::SetSubMtl(int i, Mtl* m)
{
	ReplaceReference(i,m);
	// TODO: Set the material and update the UI
}

TSTR [!output CLASS_NAME]::GetSubMtlSlotName(int)
{
	// Return i'th sub-material name
	return _T("");
}

TSTR [!output CLASS_NAME]::GetSubMtlTVName(int i)
{
	return GetSubMtlSlotName(i);
}

/*===========================================================================*\
 |	Texmap get and set
 |  By default, we support none
\*===========================================================================*/

Texmap* [!output CLASS_NAME]::GetSubTexmap(int /*i*/)
{
	return nullptr;
}

void [!output CLASS_NAME]::SetSubTexmap(int /*i*/, Texmap* /*m*/)
{
}

TSTR [!output CLASS_NAME]::GetSubTexmapSlotName(int /*i*/)
{
	return _T("");
}

TSTR [!output CLASS_NAME]::GetSubTexmapTVName(int i)
{
	// Return i'th sub-texture name
	return GetSubTexmapSlotName(i);
}



/*===========================================================================*\
 |	Standard IO
\*===========================================================================*/

#define MTL_HDR_CHUNK 0x4000

IOResult [!output CLASS_NAME]::Save(ISave* isave)
{
	IOResult res;
	isave->BeginChunk(MTL_HDR_CHUNK);
	res = MtlBase::Save(isave);
	if (res!=IO_OK) 
		return res;
	isave->EndChunk();

	return IO_OK;
}

IOResult [!output CLASS_NAME]::Load(ILoad* iload)
{
	IOResult res;
	while (IO_OK==(res=iload->OpenChunk()))
	{
		int id = iload->CurChunkID();
		switch(id)
		{
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

RefTargetHandle [!output CLASS_NAME]::Clone(RemapDir &remap)
{
	[!output CLASS_NAME] *mnew = new [!output CLASS_NAME](FALSE);
	*((MtlBase*)mnew) = *((MtlBase*)this);
	// First clone the parameter block
	mnew->ReplaceReference(PBLOCK_REF,remap.CloneRef(pblock));
	// Next clone the sub-materials
	mnew->ivalid.SetEmpty();
	for (int i = 0; i < NUM_SUBMATERIALS; i++) {
		mnew->submtl[i] = nullptr;
		if (submtl[i])
			mnew->ReplaceReference(i,remap.CloneRef(submtl[i]));
		mnew->mapOn[i] = mapOn[i];
		}
	BaseClone(this, mnew, remap);
	return (RefTargetHandle)mnew;
	}

void [!output CLASS_NAME]::NotifyChanged()
{
	NotifyDependents(FOREVER, PART_ALL, REFMSG_CHANGE);
}

void [!output CLASS_NAME]::Update(TimeValue t, Interval& valid)
{
	if (!ivalid.InInterval(t)) {

		ivalid.SetInfinite();
		pblock->GetValue( mtl_mat1_on, t, mapOn[0], ivalid);
		pblock->GetValue( pb_spin, t, spin, ivalid);

		for (int i=0; i < NUM_SUBMATERIALS; i++) {
			if (submtl[i])
				submtl[i]->Update(t,ivalid);
			}
		}
	valid &= ivalid;
}

/*===========================================================================*\
 |	Determine the characteristics of the material
\*===========================================================================*/

void [!output CLASS_NAME]::SetAmbient(Color /*c*/, TimeValue /*t*/) {}		
void [!output CLASS_NAME]::SetDiffuse(Color /*c*/, TimeValue /*t*/) {}		
void [!output CLASS_NAME]::SetSpecular(Color /*c*/, TimeValue /*t*/) {}
void [!output CLASS_NAME]::SetShininess(float /*v*/, TimeValue /*t*/) {}

Color [!output CLASS_NAME]::GetAmbient(int mtlNum, BOOL backFace)
{
	return submtl[0] ? submtl[0]->GetAmbient(mtlNum,backFace): Color(0,0,0);
}

Color [!output CLASS_NAME]::GetDiffuse(int mtlNum, BOOL backFace)
{
	return submtl[0] ? submtl[0]->GetDiffuse(mtlNum,backFace): Color(0,0,0);
}

Color [!output CLASS_NAME]::GetSpecular(int mtlNum, BOOL backFace)
{
	return submtl[0] ? submtl[0]->GetSpecular(mtlNum,backFace): Color(0,0,0);
}

float [!output CLASS_NAME]::GetXParency(int mtlNum, BOOL backFace)
{
	return submtl[0] ? submtl[0]->GetXParency(mtlNum,backFace): 0.0f;
}

float [!output CLASS_NAME]::GetShininess(int mtlNum, BOOL backFace)
{
	return submtl[0] ? submtl[0]->GetShininess(mtlNum,backFace): 0.0f;
}

float [!output CLASS_NAME]::GetShinStr(int mtlNum, BOOL backFace)
{
	return submtl[0] ? submtl[0]->GetShinStr(mtlNum,backFace): 0.0f;
}

float [!output CLASS_NAME]::WireSize(int mtlNum, BOOL backFace)
{
	return submtl[0] ? submtl[0]->WireSize(mtlNum,backFace): 0.0f;
}


/*===========================================================================*\
 |	Actual shading takes place
\*===========================================================================*/

void [!output CLASS_NAME]::Shade(ShadeContext& sc)
{
	Mtl* subMaterial = mapOn[0] ? submtl[0] : nullptr;
	if (gbufID) 
		sc.SetGBufferID(gbufID);

	if(subMaterial) 
		subMaterial->Shade(sc);
	// TODO: compute the color and transparency output returned in sc.out.
}

float [!output CLASS_NAME]::EvalDisplacement(ShadeContext& sc)
{
	Mtl* subMaterial = mapOn[0] ? submtl[0] : nullptr;
	return (subMaterial) ? subMaterial->EvalDisplacement(sc) : 0.0f;
}

Interval [!output CLASS_NAME]::DisplacementValidity(TimeValue t)
{
	Mtl* subMaterial = mapOn[0] ? submtl[0] : nullptr;

	Interval iv; 
	iv.SetInfinite();
	if(subMaterial) 
		iv &= subMaterial->DisplacementValidity(t);

	return iv;
}



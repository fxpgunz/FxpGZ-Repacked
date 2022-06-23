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

#define NSUBTEX   1 // TODO: number of sub-textures supported by this plugin
// Reference Indexes
#define COORD_REF  0
#define PBLOCK_REF 1

class [!output CLASS_NAME];

class [!output CLASS_NAME]Sampler: public MapSampler
{
private:
	[!output CLASS_NAME]* mTexture;
public:
	[!output CLASS_NAME]Sampler() : mTexture(nullptr) { }
	[!output CLASS_NAME]Sampler([!output CLASS_NAME] *c) { mTexture= c; }
	~[!output CLASS_NAME]Sampler() { }

	void   Set([!output CLASS_NAME] *c) { mTexture = c; }
	AColor Sample(ShadeContext& sc, float u,float v);
	AColor SampleFilter(ShadeContext& sc, float u,float v, float du, float dv);
	float  SampleMono(ShadeContext& sc, float u,float v);
	float  SampleMonoFilter(ShadeContext& sc, float u,float v, float du, float dv);
} ;


class [!output CLASS_NAME] : public [!output SUPER_CLASS_NAME] {
public:
	//Constructor/Destructor
	[!output CLASS_NAME]();
	virtual ~[!output CLASS_NAME]();

	//From MtlBase
	virtual ParamDlg* CreateParamDlg(HWND hwMtlEdit, IMtlParams* imp);
	virtual BOOL SetDlgThing(ParamDlg* dlg);
	virtual void Update(TimeValue t, Interval& valid);
	virtual void Reset();
	virtual Interval Validity(TimeValue t);
	virtual ULONG LocalRequirements(int subMtlNum);

	//TODO: Return the number of sub-textures
	virtual int NumSubTexmaps() { return NSUBTEX; }
	//TODO: Return the pointer to the 'i-th' sub-texmap
	virtual Texmap* GetSubTexmap(int i) { return subtex[i]; }
	virtual void SetSubTexmap(int i, Texmap *m);
	virtual TSTR GetSubTexmapSlotName(int i);

	//From Texmap
	virtual RGBA   EvalColor(ShadeContext& sc);
	virtual float  EvalMono(ShadeContext& sc);
	virtual Point3 EvalNormalPerturb(ShadeContext& sc);

	//TODO: Returns TRUE if this texture can be used in the interactive renderer
	virtual BOOL SupportTexDisplay() { return FALSE; }
	virtual void ActivateTexDisplay(BOOL onoff);
	virtual DWORD_PTR GetActiveTexHandle(TimeValue t, TexHandleMaker& thmaker);
	//TODO: Return UV transformation matrix for use in the viewports
	virtual void GetUVTransform(Matrix3 &uvtrans) { uvGen->GetUVTransform(uvtrans); }
	//TODO: Return the tiling state of the texture for use in the viewports
	virtual int    GetTextureTiling() { return  uvGen->GetTextureTiling(); }
	virtual int    GetUVWSource() { return uvGen->GetUVWSource(); }
	virtual UVGen* GetTheUVGen() { return uvGen; }

	//TODO: Return anim index to reference index
	virtual int SubNumToRefNum(int subNum) { return subNum; }

	// Loading/Saving
	virtual IOResult Load(ILoad *iload);
	virtual IOResult Save(ISave *isave);

	//From Animatable
	virtual Class_ID  ClassID() {return [!output CLASS_NAME]_CLASS_ID;}
	virtual SClass_ID SuperClassID() { return TEXMAP_CLASS_ID; }
	virtual void GetClassName(TSTR& s) {s = GetString(IDS_CLASS_NAME);}

	virtual RefTargetHandle Clone( RemapDir &remap );
	virtual RefResult NotifyRefChanged(const Interval& changeInt, RefTargetHandle hTarget, PartID& partID, RefMessage message, BOOL propagate);


	virtual int NumSubs() { return 2 + NSUBTEX; }
	virtual Animatable* SubAnim(int i);
	virtual TSTR SubAnimName(int i);

	// TODO: Maintain the number or references here
	virtual int NumRefs() { return 2 + NSUBTEX; }
	virtual RefTargetHandle GetReference(int i);


	virtual int	NumParamBlocks() { return 1; }					// return number of ParamBlocks in this instance
	virtual IParamBlock2* GetParamBlock(int /*i*/) { return pblock; } // return i'th ParamBlock
	virtual IParamBlock2* GetParamBlockByID(BlockID id) { return (pblock->ID() == id) ? pblock : NULL; } // return id'd ParamBlock

	virtual void DeleteThis() { delete this; }

protected:
	virtual void SetReference(int i, RefTargetHandle rtarg);

private:
	UVGen*           uvGen;           // ref 0
	IParamBlock2*    pblock;          // ref 1
	Texmap*          subtex[NSUBTEX]; // Other refs

	static ParamDlg* uvGenDlg;
	Interval         ivalid;
};


[!output TEMPLATESTRING_CLASSDESC]

[!if PARAM_MAPS != 0]
[!output TEMPLATESTRING_PARAMBLOCKDESC]
[!endif]


ParamDlg* [!output CLASS_NAME]::uvGenDlg;

//--- [!output CLASS_NAME] -------------------------------------------------------
[!output CLASS_NAME]::[!output CLASS_NAME]()
	: pblock(nullptr)
{
	for (int i = 0; i < NSUBTEX; i++)
		subtex[i] = nullptr;
	//TODO: Add all the initializing stuff
	Get[!output CLASS_NAME]Desc()->MakeAutoParamBlocks(this);
	Reset();
}

[!output CLASS_NAME]::~[!output CLASS_NAME]()
{

}

//From MtlBase
void [!output CLASS_NAME]::Reset()
{
	if (uvGen) 
		uvGen->Reset();
	else
		ReplaceReference( 0, GetNewDefaultUVGen());
	//TODO: Reset texmap back to its default values
	ivalid.SetEmpty();
}

void [!output CLASS_NAME]::Update(TimeValue /*t*/, Interval& /*valid*/)
{
	//TODO: Add code to evaluate anything prior to rendering
}

Interval [!output CLASS_NAME]::Validity(TimeValue /*t*/)
{
	//TODO: Update ivalid here
	return ivalid;
}

ParamDlg* [!output CLASS_NAME]::CreateParamDlg(HWND hwMtlEdit, IMtlParams* imp)
{
	IAutoMParamDlg* masterDlg = Get[!output CLASS_NAME]Desc()->CreateParamDlgs(hwMtlEdit, imp, this);
	uvGenDlg = uvGen->CreateParamDlg(hwMtlEdit, imp);
	masterDlg->AddDlg(uvGenDlg);
	//TODO: Set the user dialog proc of the param block, and do other initialization
	return masterDlg;
}

BOOL [!output CLASS_NAME]::SetDlgThing(ParamDlg* dlg)
{
	if (dlg == uvGenDlg)
		uvGenDlg->SetThing(uvGen);
	else
		return FALSE;
	return TRUE;
}

void [!output CLASS_NAME]::SetSubTexmap(int i, Texmap* m)
{
	ReplaceReference(i+2,m);
	//TODO Store the 'i-th' sub-texmap managed by the texture
}

TSTR [!output CLASS_NAME]::GetSubTexmapSlotName(int /*i*/)
{
	//TODO: Return the slot name of the 'i-th' sub-texmap
	return TSTR(_T(""));
}


//From ReferenceMaker
RefTargetHandle [!output CLASS_NAME]::GetReference(int i)
{
	//TODO: Return the references based on the index
	switch (i) {
		case 0: return uvGen;
		case 1: return pblock;
		default: return subtex[i-2];
		}
}

void [!output CLASS_NAME]::SetReference(int i, RefTargetHandle rtarg)
{
	//TODO: Store the reference handle passed into its 'i-th' reference
	switch(i) {
		case 0: uvGen = (UVGen *)rtarg; break;
		case 1:	pblock = (IParamBlock2 *)rtarg; break;
		default: subtex[i-2] = (Texmap *)rtarg; break;
	}
}

RefResult [!output CLASS_NAME]::NotifyRefChanged(const Interval& /*changeInt*/, RefTargetHandle hTarget, PartID& /*partID*/, RefMessage message, BOOL /*propagate*/ )
{
	switch (message)
	{
	case REFMSG_TARGET_DELETED:
		{
			if      (hTarget == uvGen) { uvGen = nullptr; }
			else if (hTarget == pblock) { pblock = nullptr; }
			else
			{
				for (int i = 0; i < NSUBTEX; i++)
				{
					if (subtex[i] == hTarget)
					{
						subtex[i] = nullptr;
						break;
					}
				}
			}
		}
		break;
	}
	return(REF_SUCCEED);
}

//From ReferenceTarget
RefTargetHandle [!output CLASS_NAME]::Clone(RemapDir &remap)
{
	[!output CLASS_NAME] *mnew = new [!output CLASS_NAME]();
	*((MtlBase*)mnew) = *((MtlBase*)this); // copy superclass stuff
	//TODO: Add other cloning stuff
	BaseClone(this, mnew, remap);
	return (RefTargetHandle)mnew;
}


Animatable* [!output CLASS_NAME]::SubAnim(int i)
{
	//TODO: Return 'i-th' sub-anim
	switch (i) {
		case 0: return uvGen;
		case 1: return pblock;
		default: return subtex[i-2];
		}
}

TSTR [!output CLASS_NAME]::SubAnimName(int i)
{
	//TODO: Return the sub-anim names
	switch (i) {
		case 0: return GetString(IDS_COORDS);
		case 1: return GetString(IDS_PARAMS);
		default: return GetSubTexmapTVName(i-1);
		}
}

IOResult [!output CLASS_NAME]::Save(ISave* /*isave*/)
{
	//TODO: Add code to allow plug-in to save its data
	return IO_OK;
}

IOResult [!output CLASS_NAME]::Load(ILoad* /*iload*/)
{
	//TODO: Add code to allow plug-in to load its data
	return IO_OK;
}

AColor [!output CLASS_NAME]::EvalColor(ShadeContext& /*sc*/)
{
	//TODO: Evaluate the color of texture map for the context.
	return AColor (0.0f,0.0f,0.0f,0.0f);
}

float [!output CLASS_NAME]::EvalMono(ShadeContext& sc)
{
	//TODO: Evaluate the map for a "mono" channel
	return Intens(EvalColor(sc));
}

Point3 [!output CLASS_NAME]::EvalNormalPerturb(ShadeContext& /*sc*/)
{
	//TODO: Return the perturbation to apply to a normal for bump mapping
	return Point3(0, 0, 0);
}

ULONG [!output CLASS_NAME]::LocalRequirements(int subMtlNum)
{
	//TODO: Specify various requirements for the material
	return uvGen->Requirements(subMtlNum);
}

void [!output CLASS_NAME]::ActivateTexDisplay(BOOL /*onoff*/)
{
	//TODO: Implement this only if SupportTexDisplay() returns TRUE
}

DWORD_PTR [!output CLASS_NAME]::GetActiveTexHandle(TimeValue /*t*/, TexHandleMaker& /*maker*/)
{
	//TODO: Return the texture handle to this texture map
	return 0;
}

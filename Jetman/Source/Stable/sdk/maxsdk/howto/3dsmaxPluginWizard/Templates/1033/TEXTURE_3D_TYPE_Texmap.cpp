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

#define NUM_SUBTEXTURES		1 // TODO: number of sub-textures supported by this plugin
// Reference Indexes
#define COORD_REF  0
#define PBLOCK_REF 1

class [!output CLASS_NAME];

class [!output CLASS_NAME] : public [!output SUPER_CLASS_NAME]
{
public:
	//Constructor/Destructor
	[!output CLASS_NAME]();
	virtual ~[!output CLASS_NAME]();

	//From MtlBase
	virtual ParamDlg* CreateParamDlg(HWND hwMtlEdit, IMtlParams *imp);
	virtual BOOL      SetDlgThing(ParamDlg* dlg);
	virtual void      Update(TimeValue t, Interval& valid);
	virtual void      Reset();
	virtual Interval  Validity(TimeValue t);
	virtual ULONG     LocalRequirements(int subMtlNum);

	//TODO: Return the number of sub-textures
	virtual int NumSubTexmaps() { return NUM_SUBTEXTURES; }
	//TODO: Return the pointer to the 'i-th' sub-texmap
	virtual Texmap* GetSubTexmap(int i) { return subtex[i]; }
	virtual void SetSubTexmap(int i, Texmap *m);
	virtual TSTR GetSubTexmapSlotName(int i);

	//From Texmap
	virtual RGBA EvalColor(ShadeContext& sc);
	virtual float EvalMono(ShadeContext& sc);
	virtual Point3 EvalNormalPerturb(ShadeContext& sc);

	virtual XYZGen *GetTheXYZGen() { return xyzGen; }

	//TODO: Return anim index to reference index
	virtual int SubNumToRefNum(int subNum) { return subNum; }

	//TODO: If your class is derived from Tex3D then you should also
	//implement ReadSXPData for 3D Studio/DOS SXP texture compatibility
	virtual void ReadSXPData(TCHAR* /*name*/ , void* /*sxpdata*/) { }

	// Loading/Saving
	virtual IOResult Load(ILoad *iload);
	virtual IOResult Save(ISave *isave);

	//From Animatable
	virtual Class_ID ClassID() {return [!output CLASS_NAME]_CLASS_ID;}
	virtual SClass_ID SuperClassID() { return TEXMAP_CLASS_ID; }
	virtual void GetClassName(TSTR& s) {s = GetString(IDS_CLASS_NAME);}

	virtual int NumSubs() { return 2+NUM_SUBTEXTURES; }
	virtual Animatable* SubAnim(int i);
	virtual TSTR SubAnimName(int i);

	// TODO: Maintain the number or references here
	virtual int NumRefs() { return 2+NUM_SUBTEXTURES; }
	virtual RefTargetHandle GetReference(int i);

	virtual RefTargetHandle Clone( RemapDir &remap );
	virtual RefResult NotifyRefChanged(const Interval& changeInt, RefTargetHandle hTarget, PartID& partID, RefMessage message, BOOL propagate);

	virtual int	NumParamBlocks() { return 1; }					// return number of ParamBlocks in this instance
	virtual IParamBlock2* GetParamBlock(int /*i*/) { return pblock; } // return i'th ParamBlock
	virtual IParamBlock2* GetParamBlockByID(BlockID id) { return (pblock->ID() == id) ? pblock : NULL; } // return id'd ParamBlock

	virtual void DeleteThis() { delete this; }
protected:
	virtual void SetReference(int i, RefTargetHandle rtarg);

private:
	// References
	XYZGen*          xyzGen;          // ref 0
	IParamBlock2*    pblock;          // ref 1
	Texmap*          subtex[NUM_SUBTEXTURES]; // Reference array of sub-materials

	static ParamDlg* xyzGenDlg;
	Interval         ivalid;
};


[!output TEMPLATESTRING_CLASSDESC]

[!if PARAM_MAPS != 0]
[!output TEMPLATESTRING_PARAMBLOCKDESC]
[!endif]


ParamDlg* [!output CLASS_NAME]::xyzGenDlg;

//--- [!output CLASS_NAME] -------------------------------------------------------
[!output CLASS_NAME]::[!output CLASS_NAME]()
	: xyzGen(nullptr)
	, pblock(nullptr)
{
	for (int i = 0; i < NUM_SUBTEXTURES; i++)
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
	if (xyzGen) 
		xyzGen->Reset();
	else 
		ReplaceReference( COORD_REF, GetNewDefaultXYZGen());
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
	xyzGenDlg = xyzGen->CreateParamDlg(hwMtlEdit, imp);
	masterDlg->AddDlg(xyzGenDlg);
	//TODO: Set the user dialog proc of the param block, and do other initialization
	return masterDlg;
}

BOOL [!output CLASS_NAME]::SetDlgThing(ParamDlg* dlg)
{
	if (dlg == xyzGenDlg)
		xyzGenDlg->SetThing(xyzGen);
	else
		return FALSE;
	return TRUE;
}

void [!output CLASS_NAME]::SetSubTexmap(int i, Texmap *m)
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
		case COORD_REF:  return xyzGen;
		case PBLOCK_REF: return pblock;
		default: return subtex[i-2];
		}
}

void [!output CLASS_NAME]::SetReference(int i, RefTargetHandle rtarg)
{
	//TODO: Store the reference handle passed into its 'i-th' reference
	switch(i) {
		case COORD_REF:  xyzGen = (XYZGen *)rtarg; break;
		case PBLOCK_REF: pblock = (IParamBlock2 *)rtarg; break;
		default: subtex[i-2] = (Texmap *)rtarg; break;
	}
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
		case COORD_REF: return xyzGen;
		case PBLOCK_REF: return pblock;
		default: return subtex[i-2];
		}
}

TSTR [!output CLASS_NAME]::SubAnimName(int i)
{
	//TODO: Return the sub-anim names
	switch (i) {
		case COORD_REF: return GetString(IDS_COORDS);
		case PBLOCK_REF: return GetString(IDS_PARAMS);
		default: return GetSubTexmapTVName(i-1);
		}
}

RefResult [!output CLASS_NAME]::NotifyRefChanged(const Interval& /*changeInt*/, RefTargetHandle hTarget,PartID& /*partID*/, RefMessage message, BOOL /*propagate*/ )
{
	switch (message)
	{
	case REFMSG_TARGET_DELETED:
		{
			if      (hTarget == xyzGen) { xyzGen = nullptr; }
			else if (hTarget == pblock) { pblock = nullptr; }
			else
			{
				for (int i = 0; i < NUM_SUBTEXTURES; i++)
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

IOResult [!output CLASS_NAME]::Save(ISave* /*isave*/)
{
	//TODO: Add code to allow plugin to save its data
	return IO_OK;
}

IOResult [!output CLASS_NAME]::Load(ILoad* /*iload*/)
{
	//TODO: Add code to allow plugin to load its data
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
	return xyzGen->Requirements(subMtlNum);
}


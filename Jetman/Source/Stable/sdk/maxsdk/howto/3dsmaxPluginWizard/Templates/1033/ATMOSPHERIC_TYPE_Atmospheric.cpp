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

class [!output CLASS_NAME] : public [!output SUPER_CLASS_NAME]
{
public:
	//Constructor/Destructor
	[!output CLASS_NAME]();
	virtual ~[!output CLASS_NAME]();

	// From Atmospheric
	virtual TSTR GetName() {return GetString(IDS_CLASS_NAME);}
	virtual AtmosParamDlg *CreateParamDialog(IRendParams *ip);
	virtual int RenderBegin(TimeValue t, ULONG flags);
	virtual int RenderEnd(TimeValue t);
	virtual void Update(TimeValue t, Interval& valid);
	virtual void Shade(ShadeContext& sc,const Point3& p0,const Point3& p1,Color& color, Color& trans, BOOL isBG);

	// Support for gizmos in our atmospheric
	virtual int NumGizmos() ;
	virtual INode *GetGizmo(int i);
	virtual void DeleteGizmo(int i);
	virtual void InsertGizmo(int i, INode *node);
	virtual void AppendGizmo(INode *node);
	virtual BOOL OKGizmo(INode *node);
	virtual void EditGizmo(INode *node);

	// Loading/Saving
	virtual IOResult Load(ILoad *iload);
	virtual IOResult Save(ISave *isave);

	//From Animatable
	virtual Class_ID ClassID() {return [!output CLASS_NAME]_CLASS_ID;}
	virtual SClass_ID SuperClassID() { return ATMOSPHERIC_CLASS_ID; }
	virtual void GetClassName(TSTR& s) {s = GetString(IDS_CLASS_NAME);}

	virtual RefTargetHandle Clone( RemapDir &remap );
	virtual RefResult NotifyRefChanged(const Interval& changeInt, RefTargetHandle hTarget,
		PartID& partID, RefMessage message, BOOL propagate);

	virtual int NumSubs() { return 1; }
	virtual TSTR SubAnimName(int /*i*/) { return GetString(IDS_PARAMS); }
	virtual Animatable* SubAnim(int /*i*/) { return pblock; }

	// TODO: Maintain the number or references here
	virtual int NumRefs() { return 1; }
	virtual RefTargetHandle GetReference(int i);

	virtual int	NumParamBlocks() { return 1; }					// return number of ParamBlocks in this instance
	virtual IParamBlock2* GetParamBlock(int /*i*/) { return pblock; } // return i'th ParamBlock
	virtual IParamBlock2* GetParamBlockByID(BlockID id) { return (pblock->ID() == id) ? pblock : NULL; } // return id'd ParamBlock

	virtual void DeleteThis();

protected:
	virtual void SetReference(int , RefTargetHandle rtarg);

private:

	// Parameter block
	IParamBlock2* pblock; // ref 0
};


class [!output CLASS_NAME]ParamDlg : public AtmosParamDlg
{
public:
	[!output CLASS_NAME]ParamDlg([!output CLASS_NAME] *a,IRendParams *i);
	virtual ~[!output CLASS_NAME]ParamDlg();

	virtual Class_ID ClassID() {return [!output CLASS_NAME]_CLASS_ID;}
	virtual ReferenceTarget* GetThing() {return atmos;}
	virtual void SetThing(ReferenceTarget *m);
	virtual void DeleteThis();

private:
	[!output CLASS_NAME]* atmos;
	IRendParams* ip;
	IParamMap2* pmap;
};

[!output TEMPLATESTRING_CLASSDESC]


[!if PARAM_MAPS != 0]
[!output TEMPLATESTRING_PARAMBLOCKDESC]
[!endif]


//--- [!output CLASS_NAME]ParamDlg -------------------------------------------------------

[!output CLASS_NAME]ParamDlg::[!output CLASS_NAME]ParamDlg([!output CLASS_NAME] *a,IRendParams *i)
{
	//TODO: Add the dialog and parameter map creation code here
	atmos = a;
	ip    = i;
	pmap = CreateRParamMap2(
		atmos->GetParamBlock(0),
		i,
		hInstance,
		MAKEINTRESOURCE(IDD_PANEL),
		GetString(IDS_PARAMS),
		0);
}

[!output CLASS_NAME]ParamDlg::~[!output CLASS_NAME]ParamDlg()
{
#pragma message(TODO("Put destruction and uninitialization code here"))
}

void [!output CLASS_NAME]ParamDlg::SetThing(ReferenceTarget* /*m*/)
{

}

void [!output CLASS_NAME]ParamDlg::DeleteThis()
{
	delete this;
}



[!output CLASS_NAME]::[!output CLASS_NAME]()
{
	pblock = NULL;
	Get[!output CLASS_NAME]Desc()->MakeAutoParamBlocks(this);
}

[!output CLASS_NAME]::~[!output CLASS_NAME]()
{
}

IOResult [!output CLASS_NAME]::Load(ILoad *iload)
{
	Atmospheric::Load(iload);

	#pragma message(TODO("Add code to allow plugin to load its data"))

	return IO_OK;
}

IOResult [!output CLASS_NAME]::Save(ISave *isave)
{
	Atmospheric::Save(isave);

	#pragma message(TODO("Add code to allow plugin to save its data"))

	return IO_OK;
}

void [!output CLASS_NAME]::SetReference( int i, RefTargetHandle rtarg )
{
	if (i == PBLOCK_REF)
	{
		pblock=(IParamBlock2*)rtarg;
	}
}

RefTargetHandle [!output CLASS_NAME]::GetReference( int i)
{
	if (i == PBLOCK_REF)
	{
		return pblock;
	}
	return nullptr;
}

RefResult [!output CLASS_NAME]::NotifyRefChanged(
		const Interval& /*changeInt*/, RefTargetHandle hTarget,
		PartID& /*part*/,  RefMessage message, BOOL /*propagate*/)
	{
	switch (message) {
		case REFMSG_TARGET_DELETED:
			{
				for(int i = 0; i < NumGizmos(); i++)
				{
					if((INode*)hTarget==GetGizmo(i))
						DeleteGizmo(i);
				}
			}
			break;

		case REFMSG_CHANGE:
			if (hTarget == pblock && pblock->GetMap() != NULL)
				pblock->GetMap()->Invalidate();
			break;
		}
	return REF_SUCCEED;
	}


RefTargetHandle [!output CLASS_NAME]::Clone( RemapDir &remap )
{
	[!output CLASS_NAME]* newObj = new [!output CLASS_NAME]();
	newObj->ReplaceReference(0,remap.CloneRef(pblock));
	BaseClone(this, newObj, remap);
	return (RefTargetHandle)newObj;
}

AtmosParamDlg *[!output CLASS_NAME]::CreateParamDialog(IRendParams *ip)
{
	return new [!output CLASS_NAME]ParamDlg(this,ip);
}

int [!output CLASS_NAME]::RenderBegin(TimeValue /*t*/,ULONG /*flags*/)
{
	#pragma message(TODO("Add intiliazing stuff required by plugin prior to rendering"))
	return 0;
}

int [!output CLASS_NAME]::RenderEnd(TimeValue /*t*/)
{
	#pragma message(TODO("Add stuff required after each render"))
	return 0;
}

void [!output CLASS_NAME]::Update(TimeValue /*t*/, Interval& /*validity*/)
{

}

void [!output CLASS_NAME]::Shade(
		ShadeContext& /*sc*/,const Point3& /*p0*/,const Point3& /*p1*/,
		Color& /*color*/, Color& /*trans*/, BOOL /*isBG*/)
{
	#pragma message(TODO("Add the code that is called to apply the atmospheric effect"))
}

/*===========================================================================*\
 |	Support for getting/setting gizmos
\*===========================================================================*/

int [!output CLASS_NAME]::NumGizmos()
{
	return pblock->Count(pb_gizmos);
}

INode *[!output CLASS_NAME]::GetGizmo(int i)
{
	INode *node = NULL;
	Interval iv;
	pblock->GetValue(pb_gizmos,0,node,iv, i);
	return node;
}

void [!output CLASS_NAME]::DeleteGizmo(int i)
{
	pblock->Delete(pb_gizmos, i,1);
	[!output CLASS_NAME_LOWER]_param_blk.InvalidateUI();

	NotifyDependents(FOREVER, PART_ALL, REFMSG_CHANGE);
}

void [!output CLASS_NAME]::InsertGizmo(int i, INode *node)
{
	pblock->SetValue(pb_gizmos, 0, node, i);
}

void [!output CLASS_NAME]::AppendGizmo(INode *node)
{
	pblock->Append(pb_gizmos, 1, &node);
}

BOOL [!output CLASS_NAME]::OKGizmo(INode *node)
{
	// check for duplicates in the gizmo list
	for(int i=0;i<NumGizmos();i++) { if(node==GetGizmo(i)) return FALSE; }

	ObjectState os = node->EvalWorldState(GetCOREInterface()->GetTime());
	if (os.obj->ClassID()==SPHEREGIZMO_CLASSID) return TRUE;
	if (os.obj->ClassID()==CYLGIZMO_CLASSID) return TRUE;
	if (os.obj->ClassID()==BOXGIZMO_CLASSID) return TRUE;
	return FALSE;
}

void [!output CLASS_NAME]::EditGizmo(INode* /*node*/)
{

}

void [!output CLASS_NAME]::DeleteThis()
{
	delete this;
}


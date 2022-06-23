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

class [!output CLASS_NAME] : public [!output SUPER_CLASS_NAME] , public ResourceMakerCallback
{
public:
	//Constructor/Destructor
	[!output CLASS_NAME]();
	~[!output CLASS_NAME]();

	virtual void BeginEditParams( IObjParam* ip, ULONG flags, Animatable* prev);
	virtual void EndEditParams( IObjParam* ip, ULONG flags, Animatable* next);

	virtual void SetInitialName();
	virtual const TCHAR *GetName();
	virtual void SetName(const TCHAR* name);
	virtual BOOL IsEnabled();
	virtual BOOL IsVolumeBased();
	virtual BOOL IsInVolume(Point3 p, Matrix3 tm);

	virtual BOOL IsEditing();
	virtual void Enable(BOOL enable);
	virtual void EndEditing();
	virtual void EnableEditing(BOOL enable);

	virtual IGizmoBuffer* CopyToBuffer(); 
	virtual void PasteFromBuffer(IGizmoBuffer* buffer); 

	virtual BOOL InitialCreation(int count, Point3* p, int numbeOfInstances, int* mapTable); 
	virtual void PostDeformSetup(TimeValue t);
	virtual void PreDeformSetup(TimeValue t);

	virtual Point3 DeformPoint(TimeValue t, int index, Point3 initialP, Point3 p, Matrix3 tm);

	// From Base Object
	virtual void GetWorldBoundBox(TimeValue t,INode* inode, ViewExp* vpt, Box3& box, ModContext* mc);
	virtual int Display(TimeValue t, GraphicsWindow* gw, Matrix3 tm );
	virtual int HitTest(TimeValue t, INode* inode, int type, int crossing, int flags, IPoint2* p, ViewExp* vpt, ModContext* mc, Matrix3 tm);
	virtual void SelectSubComponent(HitRecord *hitRec, BOOL selected, BOOL all, BOOL invert=FALSE);
	virtual void Move( TimeValue t, Matrix3& partm, Matrix3& tmAxis, Point3& val, Matrix3 tm, BOOL localOrigin=FALSE );
	virtual void GetSubObjectCenters(SubObjAxisCallback *cb,TimeValue t,INode *node, Matrix3 tm);
	virtual void GetSubObjectTMs(SubObjAxisCallback *cb,TimeValue t,INode *node, Matrix3 tm);
	virtual void ClearSelection(int selLevel);
	virtual void SelectAll(int selLevel);
	virtual void InvertSelection(int selLevel);

	// Loading/Saving
	virtual IOResult Load(ILoad *iload);
	virtual IOResult Save(ISave *isave);

	//From Animatable
	virtual Class_ID ClassID() {return [!output CLASS_NAME]_CLASS_ID;}		
	virtual SClass_ID SuperClassID() { return REF_TARGET_CLASS_ID; }
	virtual void GetClassName(TSTR& s) {s = GetString(IDS_CLASS_NAME);}

	virtual RefResult NotifyRefChanged(const Interval& changeInt, RefTargetHandle hTarget, PartID& partID, RefMessage message, BOOL propagate);

	virtual void DeleteThis() { delete this; }

private:
	HWND hGizmoParams;
};


[!output TEMPLATESTRING_CLASSDESC]


[!if PARAM_MAPS != 0]
[!output TEMPLATESTRING_PARAMBLOCKDESC]
[!endif]

[!output CLASS_NAME]::[!output CLASS_NAME]()
{
	Get[!output CLASS_NAME]Desc()->MakeAutoParamBlocks(this);
	DbgAssert(pblock_gizmo_data);
}

[!output CLASS_NAME]::~[!output CLASS_NAME]()
{

}

void [!output CLASS_NAME]::BeginEditParams( IObjParam *ip, ULONG flags,Animatable *prev )
{
	Get[!output CLASS_NAME]Desc()->BeginEditParams(ip, this, flags, prev);
}

void [!output CLASS_NAME]::EndEditParams( IObjParam *ip, ULONG flags,Animatable *next)
{
	Get[!output CLASS_NAME]Desc()->EndEditParams(ip, this, flags, next);
}

/*******************************************************************************************
*
	This is called by the Skin Modifier to receive the name at creation of the gizmo
	It will appear in Skin's Gizmo list box
*
********************************************************************************************/

void [!output CLASS_NAME]::SetInitialName()
{
	//TODO: Set the initial name to appear in the gizmo list
	pblock_gizmo_data->SetValue(pb_gizmoparam_name,0,_T("Wizard Gizmo"));
}


/********************************************************************************************
*
	The following functions give access to the name of the Gizmo by accessing the
	Parameter block storing the name
*
*********************************************************************************************/

const TCHAR * [!output CLASS_NAME]::GetName()
{
	const TCHAR* name = nullptr;
	Interval forever = FOREVER;
	pblock_gizmo_data->GetValue(pb_gizmoparam_name,0,name,forever);
	return name;
}

void [!output CLASS_NAME]::SetName(const TCHAR *name)
{
	pblock_gizmo_data->SetValue(pb_gizmoparam_name,0,name);
}

BOOL [!output CLASS_NAME]::IsEnabled()
{
	//TODO:  Tell the system whether the gizmo is Enabled or not
	return TRUE;
}
		
BOOL [!output CLASS_NAME]::IsVolumeBased()
{
	//TODO: Tell the system whether the gizmo works on a volume basis
	return FALSE;
}

BOOL [!output CLASS_NAME]::IsInVolume(Point3 /*p*/, Matrix3 /*tm*/)
{
	//TODO:  Check whether the past in point lies in the volume
	return TRUE;
}

BOOL [!output CLASS_NAME]::IsEditing()
{
	return TRUE;
}

void [!output CLASS_NAME]::Enable(BOOL /*enable*/)
{
	//TODO:  Enable or disable the Gizmo
}

void [!output CLASS_NAME]::EndEditing()
{

}

void [!output CLASS_NAME]::EnableEditing(BOOL /*enable*/)
{

}

IGizmoBuffer* [!output CLASS_NAME]::CopyToBuffer() 
{
	return nullptr;
}

void [!output CLASS_NAME]::PasteFromBuffer(IGizmoBuffer*)
{

}

/************************************************************************************************
*
	This is called when the gizmo is initially created it is passed to the current selected 
	verts in the world space.

	count is the number of vertice in *p

	*p is the list of point being affected in world space

	numberOfInstances is the number of times this modifier has been instanced

	mapTable is an index list into the original vertex table for *p
*
**************************************************************************************************/


BOOL [!output CLASS_NAME]::InitialCreation(int /*count*/, Point3* /*p*/, int /*numbeOfInstances*/, int* /*mapTable*/)
{
	return TRUE;
}

/*************************************************************************************************
*
	This is called before the deformation on a frame to allow the gizmo to do some
	initial setup
*
**************************************************************************************************/

void [!output CLASS_NAME]::PreDeformSetup(TimeValue /*t*/)
{
	//TODO:Perform any pre deform initialization
}

void [!output CLASS_NAME]::PostDeformSetup(TimeValue /*t*/)
{
	//TODO: Perform any clean up
}

/*************************************************************************************************
*
	This is what deforms the point
	It is passed in from the Map call from the Skin modifier
*
**************************************************************************************************/	

Point3 [!output CLASS_NAME]::DeformPoint(TimeValue /*t*/, int /*index*/, Point3 /*initial*/, Point3 /*p*/, Matrix3 /*tm*/)
{
	//TODO: Defrom the Point
	return Point3(0,0,0);
}

/**************************************************************************************************
*
  This retrieves the bounding box of the gizmo in world space
*
**************************************************************************************************/


void [!output CLASS_NAME]::GetWorldBoundBox(TimeValue /*t*/,INode* /*inode*/, ViewExp* /*vpt*/, Box3& /*box*/, ModContext* /*mc*/ )
{
	//TODO: Calculate the bounding box of the Gizmo
}

/*************************************************************************************************
*
	This is called in the skin modifier's display code to show the actual gizmo
*
***************************************************************************************************/


int [!output CLASS_NAME]::Display(TimeValue /*t*/, GraphicsWindow* /*gw*/, Matrix3 /*tm*/)
{
	//TODO: Draw the physical representation of the Gizmo
	return 1;
}


/*************************************************************************************************
*
	The following functions are standard sub-object selection and manipulation routines
*
**************************************************************************************************/


int [!output CLASS_NAME]::HitTest(TimeValue /*t*/, INode* , int /*type*/, int /*crossing*/, int /*flags*/, IPoint2*, ViewExp*, ModContext*, Matrix3)
{
	//TODO:  Perform the gizmo's hit testing and return the result
	return 0;
}

void [!output CLASS_NAME]::SelectSubComponent(HitRecord* , BOOL /*selected*/, BOOL /*all*/, BOOL /*invert*/)
{
	//TODO: Perform the actual selection based on the HitRecord
}

void [!output CLASS_NAME]::Move( TimeValue /*t*/, Matrix3& /*partm*/, Matrix3& /*tmAxis*/, Point3& /*val*/, Matrix3 /*tm*/, BOOL /*localOrigin*/ ) 
{
	//TODO: Perform the transformation of the selected object
}

void [!output CLASS_NAME]::GetSubObjectCenters(SubObjAxisCallback* /*cb*/, TimeValue /*t*/, INode* /*node*/, Matrix3 /*tm*/)
{
	//TODO: Update the centres of the selected Objected
}

void [!output CLASS_NAME]::GetSubObjectTMs(SubObjAxisCallback*, TimeValue /*t*/, INode* /*node*/, Matrix3 /*tm*/)
{
	//TODO: Update the Tranformation matrices of the selected objects
}

void [!output CLASS_NAME]::ClearSelection(int /*subObjectLevel*/)
{
	//TODO: Clear the current selection 
}

void [!output CLASS_NAME]::SelectAll(int /*subObjectLevel*/)
{
	//TODO: Select all the subobject components at the currect sub object level
}

void [!output CLASS_NAME]::InvertSelection(int /*subObjectLevel*/)
{
	//TODO: Invert the current selection at the currect sub object level
}

IOResult [!output CLASS_NAME]::Load(ILoad* /*iload*/)
{
	//TODO: Standard loading of data
	return IO_OK;
}

IOResult [!output CLASS_NAME]::Save(ISave* /*isave*/)
{
	//TODO: standard saving of data
	return IO_OK;
}

RefResult [!output CLASS_NAME]::NotifyRefChanged(
		const Interval& /*changeInt*/, RefTargetHandle hTarget,
		PartID& /*partID*/,  RefMessage message, BOOL /*propagate*/) 
{
	switch (message)
	{
	case REFMSG_TARGET_DELETED:
		{
			if (hTarget == pblock_gizmo_data)
			{
				pblock_gizmo_data = nullptr;
			}
		}
		break;
	}
	return REF_SUCCEED;
}



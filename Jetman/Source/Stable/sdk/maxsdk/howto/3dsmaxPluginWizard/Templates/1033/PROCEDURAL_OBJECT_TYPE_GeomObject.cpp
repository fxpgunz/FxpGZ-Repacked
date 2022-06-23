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

#define PBLOCK_REF 0

class [!output CLASS_NAME] : public [!output SUPER_CLASS_NAME]
{
public:
	//Constructor/Destructor
	[!output CLASS_NAME]();
	virtual ~[!output CLASS_NAME]();

	virtual void DeleteThis() { delete this; }

	// From BaseObject
	virtual CreateMouseCallBack* GetCreateMouseCallBack();
	virtual int Display(TimeValue t, INode* inode, ViewExp *vpt, int flags);
	virtual int HitTest(TimeValue t, INode* inode, int type, int crossing, int flags, IPoint2 *p, ViewExp *vpt);
	virtual void Snap(TimeValue t, INode* inode, SnapInfo *snap, IPoint2 *p, ViewExp *vpt);
	//TODO: Return the name that will appear in the history browser (modifier stack)
	virtual const TCHAR *GetObjectName() { return GetString(IDS_CLASS_NAME); }

	virtual void GetWorldBoundBox(TimeValue t, INode *mat, ViewExp *vpt, Box3& box );
	virtual void GetLocalBoundBox(TimeValue t, INode *mat, ViewExp *vpt, Box3& box );

	virtual void GetDeformBBox(TimeValue t, Box3& box, Matrix3 *tm, BOOL useSel );
	//TODO: Return the default name of the node when it is created.
	virtual void InitNodeName(TSTR& s) { s = GetString(IDS_CLASS_NAME); }

	// From Object
	virtual BOOL HasUVW();
	virtual void SetGenUVW(BOOL sw);
	virtual int CanConvertToType(Class_ID obtype);
	virtual Object* ConvertToType(TimeValue t, Class_ID obtype);
	virtual void GetCollapseTypes(Tab<Class_ID> &clist,Tab<TSTR*> &nlist);
	virtual int IntersectRay(TimeValue t, Ray& ray, float& at, Point3& norm);
	//TODO: Evaluate the object and return the ObjectState
	virtual ObjectState Eval(TimeValue /*t*/) { return ObjectState(this); };
	//TODO: Return the validity interval of the object as a whole
	virtual Interval ObjectValidity(TimeValue /*t*/) { return FOREVER; }

	// From Animatable
	virtual void BeginEditParams( IObjParam  *ip, ULONG flags,Animatable *prev);
	virtual void EndEditParams( IObjParam *ip, ULONG flags,Animatable *next);

	// From GeomObject
	virtual Mesh* GetRenderMesh(TimeValue t, INode *inode, View& view, BOOL& needDelete);

	// Loading/Saving
	virtual IOResult Load(ILoad *iload);
	virtual IOResult Save(ISave *isave);

	//From Animatable
	virtual Class_ID ClassID() {return [!output CLASS_NAME]_CLASS_ID;}
	virtual SClass_ID SuperClassID() { return GEOMOBJECT_CLASS_ID; }
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

	// local methods
	BOOL GetSuspendSnap(){return suspendSnap;}
	void SetSuspendSnap(BOOL iSuspendSnap){suspendSnap = iSuspendSnap;}

protected:
	virtual void SetReference(int i, RefTargetHandle rtarg);

private:
	// Parameter block
	IParamBlock2* pblock;      // ref 0
	BOOL          suspendSnap; // A flag for setting snapping on/off
};


[!output TEMPLATESTRING_CLASSDESC]


[!if PARAM_MAPS != 0]
[!output TEMPLATESTRING_PARAMBLOCKDESC]
[!endif]


//--- [!output CLASS_NAME] -------------------------------------------------------

[!output CLASS_NAME]::[!output CLASS_NAME]()
	: pblock(nullptr)
{
	Get[!output CLASS_NAME]Desc()->MakeAutoParamBlocks(this);
}

[!output CLASS_NAME]::~[!output CLASS_NAME]()
{
}

IOResult [!output CLASS_NAME]::Load(ILoad* /*iload*/)
{
	#pragma message(TODO("Add code to allow plugin to load its data"))
	return IO_OK;
}

IOResult [!output CLASS_NAME]::Save(ISave* /*isave*/)
{
	#pragma message(TODO("Add code to allow plugin to save its data"))
	return IO_OK;
}

void [!output CLASS_NAME]::BeginEditParams(IObjParam *ip,ULONG flags,Animatable *prev)
{
	Get[!output CLASS_NAME]Desc()->BeginEditParams(ip, this, flags, prev);
}

void [!output CLASS_NAME]::EndEditParams( IObjParam *ip, ULONG flags,Animatable *next )
{
	#pragma message(TODO("Save plugin parameter values into class variables, if they are not hosted in ParamBlocks."))

	Get[!output CLASS_NAME]Desc()->EndEditParams(ip, this, flags, next);
}

//From Object
BOOL [!output CLASS_NAME]::HasUVW()
{
	#pragma message(TODO("Return whether the object has UVW coordinates or not"))
	return TRUE;
}

void [!output CLASS_NAME]::SetGenUVW(BOOL sw)
{
	if (sw==HasUVW()) return;
	#pragma message(TODO("TODO: Set the plugin's internal value to sw"))
}

//Class for interactive creation of the object using the mouse
class [!output CLASS_NAME]CreateCallBack : public CreateMouseCallBack {
	IPoint2 sp0;		//First point in screen coordinates
	[!output CLASS_NAME] *ob;		//Pointer to the object
	Point3 p0;			//First point in world coordinates
public:
	int proc( ViewExp *vpt,int msg, int point, int flags, IPoint2 m, Matrix3& mat);
	void SetObj([!output CLASS_NAME] *obj) {ob = obj;}
};

int [!output CLASS_NAME]CreateCallBack::proc(ViewExp *vpt,int msg, int point, int /*flags*/, IPoint2 m, Matrix3& mat )
{
	#pragma message(TODO("Implement the mouse creation code here"))

	if ( ! vpt || ! vpt->IsAlive() )
	{
		// why are we here
		DbgAssert(!_T("Invalid viewport!"));
		return FALSE;
	}


	if (msg==MOUSE_POINT||msg==MOUSE_MOVE) {
		switch(point) {
		case 0: // only happens with MOUSE_POINT msg
			ob->SetSuspendSnap(TRUE);
			sp0 = m;
			p0 = vpt->SnapPoint(m,m,NULL,SNAP_IN_PLANE);
			mat.SetTrans(p0);
			break;
		#pragma message(TODO("Add for the rest of points"))
		}
	} else {
		if (msg == MOUSE_ABORT) return CREATE_ABORT;
	}

	return TRUE;
}

static [!output CLASS_NAME]CreateCallBack [!output CLASS_NAME]CreateCB;

//From BaseObject
CreateMouseCallBack* [!output CLASS_NAME]::GetCreateMouseCallBack()
{
	[!output CLASS_NAME]CreateCB.SetObj(this);
	return(&[!output CLASS_NAME]CreateCB);
}

int [!output CLASS_NAME]::Display(TimeValue /*t*/, INode* /*inode*/, ViewExp* /*vpt*/, int /*flags*/)
{
	#pragma message(TODO("Implement the displaying of the object here"))
	return 0;
}

int [!output CLASS_NAME]::HitTest(TimeValue /*t*/, INode* /*inode*/, int /*type*/, int /*crossing*/, int /*flags*/, IPoint2* /*p*/, ViewExp* /*vpt*/)
{
	#pragma message(TODO("Implement the hit testing here"))
	return 0;
}

void [!output CLASS_NAME]::Snap(TimeValue /*t*/, INode* /*inode*/, SnapInfo* /*snap*/, IPoint2* /*p*/, ViewExp* /*vpt*/)
{
	#pragma message(TODO("Check the point passed for a snap and update the SnapInfo structure"))
}

void [!output CLASS_NAME]::GetWorldBoundBox(TimeValue /*t*/, INode* /*mat*/, ViewExp* /*vpt*/, Box3& /*box*/ )
{
	#pragma message(TODO("Return the world space bounding box of the object"))
}

void [!output CLASS_NAME]::GetLocalBoundBox(TimeValue /*t*/, INode* /*mat*/, ViewExp* /*vpt*/, Box3& /*box*/ )
{
	#pragma message(TODO("Return the local space bounding box of the object"))
}

void [!output CLASS_NAME]::GetDeformBBox(TimeValue /*t*/, Box3& /*box*/, Matrix3* /*tm*/, BOOL /*useSel*/ )
{
	#pragma message(TODO("Compute the bounding box in the objects local coordinates or the optional space defined by tm."))
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

//From ReferenceMaker
RefResult [!output CLASS_NAME]::NotifyRefChanged( const Interval& /*changeInterval*/, RefTargetHandle hTarget, PartID& /*part*/, RefMessage message, BOOL /*propagate*/ )
{
	#pragma message(TODO("Implement, if the object makes references to other things"))
	switch (message)
	{
	case REFMSG_TARGET_DELETED:
		{
			if (hTarget == pblock)
			{
				pblock = nullptr;
			}
		}
		break;
	}
	return(REF_SUCCEED);
}

Mesh* [!output CLASS_NAME]::GetRenderMesh(TimeValue /*t*/, INode* /*inode*/, View& /*view*/, BOOL& /*needDelete*/)
{
	#pragma message(TODO("Return the mesh representation of the object used by the renderer"))
	return NULL;
}


Object* [!output CLASS_NAME]::ConvertToType(TimeValue /*t*/, Class_ID /*obtype*/)
{
	#pragma message(TODO("If the plugin can convert to a nurbs surface then check to see whether obtype == EDITABLE_SURF_CLASS_ID and convert the object to nurbs surface and return the object"))
	return NULL;
}

int [!output CLASS_NAME]::CanConvertToType(Class_ID obtype)
{
	#pragma message(TODO("Check for any additional types the plugin supports"))
	if (obtype == defObjectClassID || obtype == triObjectClassID)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

// From Object
int [!output CLASS_NAME]::IntersectRay( TimeValue /*t*/, Ray& /*ray*/, float& /*at*/, Point3& /*norm*/)
{
	#pragma message(TODO("Return TRUE after you implement this method"))
	return FALSE;
}

void [!output CLASS_NAME]::GetCollapseTypes(Tab<Class_ID>& clist, Tab<TSTR*>& nlist)
{
	Object::GetCollapseTypes(clist, nlist);
	#pragma message(TODO("Append any any other collapse type the plugin supports"))
}

// From ReferenceTarget
RefTargetHandle [!output CLASS_NAME]::Clone(RemapDir& remap)
{
	[!output CLASS_NAME]* newob = new [!output CLASS_NAME]();
	#pragma message(TODO("Make a copy all the data and also clone all the references"))
	newob->ReplaceReference(0,remap.CloneRef(pblock));
	BaseClone(this, newob, remap);
	return(newob);
}


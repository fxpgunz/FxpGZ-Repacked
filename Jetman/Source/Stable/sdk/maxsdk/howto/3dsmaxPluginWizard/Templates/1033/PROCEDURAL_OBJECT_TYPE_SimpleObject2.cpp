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

	// Parameter block handled by parent

	// From BaseObject
	virtual CreateMouseCallBack* GetCreateMouseCallBack();

	// From Object
	virtual BOOL HasUVW();
	virtual void SetGenUVW(BOOL sw);
	virtual int CanConvertToType(Class_ID obtype);
	virtual Object* ConvertToType(TimeValue t, Class_ID obtype);
	virtual void GetCollapseTypes(Tab<Class_ID>& clist,Tab<TSTR*>& nlist);
	virtual int IntersectRay(TimeValue t, Ray& ray, float& at, Point3& norm);

	// From Animatable
	virtual void BeginEditParams( IObjParam  *ip, ULONG flags,Animatable *prev);
	virtual void EndEditParams( IObjParam *ip, ULONG flags,Animatable *next);

	// From SimpleObject
	virtual void BuildMesh(TimeValue t);
	virtual void InvalidateUI();

	//From Animatable
	virtual Class_ID ClassID() {return [!output CLASS_NAME]_CLASS_ID;}
	virtual SClass_ID SuperClassID() { return GEOMOBJECT_CLASS_ID; }
	virtual void GetClassName(TSTR& s) {s = GetString(IDS_CLASS_NAME);}

	virtual RefTargetHandle Clone( RemapDir& remap );

	virtual int NumParamBlocks() { return 1; }					// return number of ParamBlocks in this instance
	virtual IParamBlock2* GetParamBlock(int /*i*/) { return pblock2; } // return i'th ParamBlock
	virtual IParamBlock2* GetParamBlockByID(BlockID id) { return (pblock2->ID() == id) ? pblock2 : NULL; } // return id'd ParamBlock

	void DeleteThis() { delete this; }
};


[!output TEMPLATESTRING_CLASSDESC]


[!if PARAM_MAPS != 0]
[!output TEMPLATESTRING_PARAMBLOCKDESC]
[!endif]


//--- [!output CLASS_NAME] -------------------------------------------------------

[!output CLASS_NAME]::[!output CLASS_NAME]()
{
	Get[!output CLASS_NAME]Desc()->MakeAutoParamBlocks(this);
}

[!output CLASS_NAME]::~[!output CLASS_NAME]()
{
}

void [!output CLASS_NAME]::BeginEditParams(IObjParam* ip, ULONG flags, Animatable* prev)
{
	SimpleObject2::BeginEditParams(ip,flags,prev);
	Get[!output CLASS_NAME]Desc()->BeginEditParams(ip, this, flags, prev);
}

void [!output CLASS_NAME]::EndEditParams( IObjParam* ip, ULONG flags, Animatable* next )
{
	//TODO: Save plugin parameter values into class variables, if they are not hosted in ParamBlocks. 
	SimpleObject2::EndEditParams(ip,flags,next);
	Get[!output CLASS_NAME]Desc()->EndEditParams(ip, this, flags, next);
}

//From Object
BOOL [!output CLASS_NAME]::HasUVW() 
{ 
	//TODO: Return whether the object has UVW coordinates or not
	return TRUE; 
}

void [!output CLASS_NAME]::SetGenUVW(BOOL sw) 
{
	if (sw==HasUVW()) 
		return;
	//TODO: Set the plugin's internal value to sw
}

//Class for interactive creation of the object using the mouse
class [!output CLASS_NAME]CreateCallBack : public CreateMouseCallBack {
	IPoint2 sp0;              //First point in screen coordinates
	[!output CLASS_NAME]* ob; //Pointer to the object 
	Point3 p0;                //First point in world coordinates
public:	
	int proc( ViewExp *vpt,int msg, int point, int flags, IPoint2 m, Matrix3& mat);
	void SetObj([!output CLASS_NAME] *obj) {ob = obj;}
};

int [!output CLASS_NAME]CreateCallBack::proc(ViewExp *vpt,int msg, int point, int /*flags*/, IPoint2 m, Matrix3& mat )
{
	//TODO: Implement the mouse creation code here
	if ( ! vpt || ! vpt->IsAlive() )
	{
		// why are we here
		DbgAssert(!_T("Invalid viewport!"));
		return FALSE;
	}

	if (msg == MOUSE_POINT || msg == MOUSE_MOVE) {
		switch(point) {
		case 0: // only happens with MOUSE_POINT msg
			ob->suspendSnap = TRUE;
			sp0 = m;
			p0 = vpt->SnapPoint(m,m,NULL,SNAP_IN_PLANE);
			mat.SetTrans(p0);
			break;
		//TODO: Add for the rest of points
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

//From SimpleObject
void [!output CLASS_NAME]::BuildMesh(TimeValue /*t*/)
{
	//TODO: Implement the code to build the mesh representation of the object
	//      using its parameter settings at the time passed. The plug-in should 
	//      use the data member mesh to store the built mesh.
	//      SimpleObject ivalid member should be updated. This can be done while
	//      retrieving values from pblock2.
	ivalid = FOREVER;
}

void [!output CLASS_NAME]::InvalidateUI() 
{
	// Hey! Update the param blocks
	pblock2->GetDesc()->InvalidateUI();
}

Object* [!output CLASS_NAME]::ConvertToType(TimeValue t, Class_ID obtype)
{
	//TODO: If the plugin can convert to a nurbs surface then check to see 
	//      whether obtype == EDITABLE_SURF_CLASS_ID and convert the object
	//      to nurbs surface and return the object
	//      If no special conversion is needed remove this implementation.
	
	return SimpleObject::ConvertToType(t,obtype);
}

int [!output CLASS_NAME]::CanConvertToType(Class_ID obtype)
{
	//TODO: Check for any additional types the plugin supports
	//      If no special conversion is needed remove this implementation.
	return SimpleObject::CanConvertToType(obtype);
}

// From Object
int [!output CLASS_NAME]::IntersectRay(TimeValue /*t*/, Ray& /*ray*/, float& /*at*/, Point3& /*norm*/)
{
	//TODO: Return TRUE after you implement this method
	return FALSE;
}

void [!output CLASS_NAME]::GetCollapseTypes(Tab<Class_ID>& clist,Tab<TSTR*>& nlist)
{
	Object::GetCollapseTypes(clist, nlist);
	//TODO: Append any any other collapse type the plugin supports
}

// From ReferenceTarget
RefTargetHandle [!output CLASS_NAME]::Clone(RemapDir& remap) 
{
	[!output CLASS_NAME]* newob = new [!output CLASS_NAME]();	
	//TODO: Make a copy all the data and also clone all the references
	newob->ReplaceReference(0,remap.CloneRef(pblock2));
	newob->ivalid.SetEmpty();
	BaseClone(this, newob, remap);
	return(newob);
}

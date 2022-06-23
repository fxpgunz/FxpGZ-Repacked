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
	[!output CLASS_NAME](INode *pnode);
	~[!output CLASS_NAME]();

	int HitTest(TimeValue t, INode* pNode, int type, int crossing, int flags, IPoint2 *pScreenPoint, ViewExp *pVpt);
	int Display(TimeValue t, INode* pNode, ViewExp *pVpt, int flags);

	// Used for manipulator set manager, which is always active.
	bool AlwaysActive();

	DisplayState  MouseEntersObject(TimeValue t, ViewExp* pVpt, IPoint2& m, ManipHitData* pHitData);
	DisplayState  MouseLeavesObject(TimeValue t, ViewExp* pVpt, IPoint2& m, ManipHitData* pHitData);

	void OnMouseMove(TimeValue t, ViewExp* pVpt, IPoint2& m, DWORD flags, ManipHitData* pHitData);
	void OnButtonDown(TimeValue t, ViewExp* pVpt, IPoint2& m, DWORD flags, ManipHitData* pHitData);
	void OnButtonUp(TimeValue t, ViewExp* pVpt, IPoint2& m, DWORD flags, ManipHitData* pHitData); 
	INode* GetINode();
	TSTR& GetManipName(); 

	//from BaseObject
	virtual CreateMouseCallBack* GetCreateMouseCallBack(void);

	//from Object
	virtual ObjectState Eval(int);
	virtual void InitNodeName(TSTR& s);
	virtual Interval ObjectValidity(TimeValue t);

	//from GeomObject
	virtual void GetWorldBoundBox(TimeValue t, INode* inode, ViewExp* vpt, Box3& box );
	virtual void GetLocalBoundBox(TimeValue t, INode* inode, ViewExp* vpt, Box3& box );
	virtual void GetDeformBBox(TimeValue t, Box3& box, Matrix3* tm = nullptr, BOOL useSel = FALSE );
	virtual void BeginEditParams( IObjParam* ip, ULONG flags, Animatable* prev);
	virtual void EndEditParams( IObjParam* ip, ULONG flags, Animatable* next);

	//From Animatable
	virtual Class_ID ClassID() {return [!output CLASS_NAME]_CLASS_ID;}
	virtual SClass_ID SuperClassID() { return HELPER_CLASS_ID; }
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
protected:
	virtual void SetReference(int i, RefTargetHandle rtarg);

private:
	TSTR name;
	// Parameter block
	IParamBlock2* pblock; //ref 0
};



[!output TEMPLATESTRING_CLASSDESC]

/*********************************************************************************************************
*
	If a manipulator applies to a node, this call will create an instance of it and add it to the 
	attached objects of the node.  It will then initialize the manipualtor
*
\*****************************************************************************************************/
Manipulator * [!output CLASS_NAME]ClassDesc::CreateManipulator(RefTargetHandle /*hTarget*/, INode* /*node*/)
{
	[!output CLASS_NAME]* pManip = new [!output CLASS_NAME]();
	return pManip;
}


[!if PARAM_MAPS != 0]
[!output TEMPLATESTRING_PARAMBLOCKDESC]
[!endif]




/**************************************************************************************************
*
	Define a Mouse Callback to implement the creation of the gizmo.  If the Manipulator does not
	have any user interface or creation method, then this is not need and ClassDesc2::Ispublic() 
	can return FALSE.
*
\**************************************************************************************************/

class [!output CLASS_NAME]CreateCallBack : public CreateMouseCallBack
{
	IPoint2 sp0;				//First point in screen coordinates
	[!output CLASS_NAME]* ob;	//Pointer to the object 
	Point3 p0;				//First point in world coordinates
public:	
	int proc( ViewExp *vpt,int msg, int point, int flags, IPoint2 m, Matrix3& mat);
	void SetObj([!output CLASS_NAME] *obj) {ob = obj;}
};


int [!output CLASS_NAME]CreateCallBack::proc(ViewExp* vpt, int msg, int point, int /*flags*/, IPoint2 m, Matrix3& mat )
{
	if ( ! vpt || ! vpt->IsAlive() )
	{
		// why are we here
		DbgAssert(!_T("Invalid viewport!"));
		return FALSE;
	}
	//TODO: Implement the mouse creation code here
	if (msg == MOUSE_POINT || msg == MOUSE_MOVE)
	{
		switch(point)
		{
		case 0: // only happens with MOUSE_POINT msg
			sp0 = m;
			p0 = vpt->SnapPoint(m,m,NULL,SNAP_IN_PLANE);
			mat.SetTrans(p0);
			break;
		//TODO: Add for the rest of points
		}
	}
	else
	{
		if (msg == MOUSE_ABORT) 
			return CREATE_ABORT;
	}

	return TRUE;
}

static [!output CLASS_NAME]CreateCallBack [!output CLASS_NAME]CreateCB;



[!output CLASS_NAME]::[!output CLASS_NAME]()
	: Manipulator(nullptr)
	, pblock(nullptr)
{
	Get[!output CLASS_NAME]Desc()->MakeAutoParamBlocks(this);
	//TODO: Define the name of the Manipulator
	name = _T("Wizard Manipulator");
}

[!output CLASS_NAME]::[!output CLASS_NAME](INode* pnode)
	: Manipulator(pnode)
	, pblock(nullptr)
{
	Get[!output CLASS_NAME]Desc()->MakeAutoParamBlocks(this);
	//TODO: Define the name of the Manipulator
	name = _T("Wizard Manipulator");
}



[!output CLASS_NAME]::~[!output CLASS_NAME]()
{

}

void [!output CLASS_NAME]::BeginEditParams( IObjParam* ip, ULONG flags, Animatable* prev)
{
	Get[!output CLASS_NAME]Desc()->BeginEditParams(ip, this, flags, prev);
}

void [!output CLASS_NAME]::EndEditParams( IObjParam* ip, ULONG flags, Animatable* next)
{
	Get[!output CLASS_NAME]Desc()->EndEditParams(ip, this, flags, next);
}

int [!output CLASS_NAME]::HitTest(TimeValue /*t*/, INode* /*pNode*/, int /*type*/, int /*crossing*/,
							int /*flags*/, IPoint2* /*pScreenPoint*/, ViewExp* /*pVpt*/)
{
	// Return the result of the HitTest
	return 0;
}

int [!output CLASS_NAME]::Display(TimeValue /*t*/, INode* /*pNode*/, ViewExp* /*pVpt*/, int /*flags*/)
{
	return 0;
}

 // Used for manipulator set manager, which is always active.
bool [!output CLASS_NAME]::AlwaysActive() 
{ 
	//TODO: Return whether the manipulator can remain active
	return false; 
}

TSTR& [!output CLASS_NAME]::GetManipName() 
{
	//TODO: Return the name of the Manipulator
	return  name;
}

DisplayState  [!output CLASS_NAME]::MouseEntersObject(TimeValue /*t*/, ViewExp* /*pVpt*/, IPoint2& /*m*/, ManipHitData* /*pHitData*/)
{
	return kFullRedrawNeeded;
}
DisplayState  [!output CLASS_NAME]::MouseLeavesObject(TimeValue /*t*/, ViewExp* /*pVpt*/, IPoint2& /*m*/, ManipHitData* /*pHitData*/)
{
	return kFullRedrawNeeded;
}

void [!output CLASS_NAME]::OnMouseMove(TimeValue /*t*/, ViewExp* /*pVpt*/, IPoint2& /*m*/, DWORD /*flags*/, ManipHitData* /*pHitData*/)
{

}

void [!output CLASS_NAME]::OnButtonDown(TimeValue /*t*/, ViewExp* /*pVpt*/, IPoint2& /*m*/, DWORD /*flags*/, ManipHitData* /*pHitData*/)
{

}

void [!output CLASS_NAME]::OnButtonUp(TimeValue /*t*/, ViewExp* /*pVpt*/, IPoint2& /*m*/, DWORD /*flags*/, ManipHitData* /*pHitData*/)
{

}

INode* [!output CLASS_NAME]::GetINode() 
{
	return mpINode; 
}

void [!output CLASS_NAME]::InitNodeName(TSTR& s) 
{
	s = GetObjectName();
}

ObjectState  [!output CLASS_NAME]::Eval(int)
{
	return ObjectState(this); 
}


Interval [!output CLASS_NAME]::ObjectValidity(TimeValue /*t*/)
{
	return FOREVER;
}

void [!output CLASS_NAME]::GetWorldBoundBox(TimeValue /*t*/, INode* /*node*/, ViewExp* /*vpt*/, Box3& /*box*/)
{

}

void [!output CLASS_NAME]::GetLocalBoundBox(TimeValue /*t*/, INode* /*node*/, ViewExp* /*vpt*/, Box3& /*box*/)
{

}

void [!output CLASS_NAME]::GetDeformBBox(TimeValue /*t*/, Box3& /*box*/, Matrix3* /*transform*/, BOOL /*useSelection*/)
{

}


CreateMouseCallBack* [!output CLASS_NAME]::GetCreateMouseCallBack(void) 
{ 
	[!output CLASS_NAME]CreateCB.SetObj(this);
	return(&[!output CLASS_NAME]CreateCB);
}

RefTargetHandle [!output CLASS_NAME]::GetReference( int i)
{
	if (i == PBLOCK_REF)
	{
		return pblock;
	}
	return nullptr;
}

void [!output CLASS_NAME]::SetReference( int i, RefTargetHandle rtarg )
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
	//TODO: Add code to handle the various reference changed messages
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
	return REF_SUCCEED;
}

RefTargetHandle [!output CLASS_NAME]::Clone(RemapDir& remap) 
{
	[!output CLASS_NAME]* newob = new [!output CLASS_NAME]();	
	//TODO: Make a copy all the data and also clone all the references
	newob->ReplaceReference(0,remap.CloneRef(pblock));
	BaseClone(this, newob, remap);
	return(newob);
}


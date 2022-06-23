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

	// Parameter block handled by parent

	virtual void OnMouseMove(TimeValue t, ViewExp* pVpt, IPoint2& m, DWORD flags, ManipHitData* pHitData);
	virtual void OnButtonDown(TimeValue t, ViewExp* pVpt, IPoint2& m, DWORD flags, ManipHitData* pHitData);
	virtual void OnButtonUp(TimeValue t, ViewExp* pVpt, IPoint2& m, DWORD flags, ManipHitData* pHitData); 
	//from BaseObject
	virtual CreateMouseCallBack* GetCreateMouseCallBack(void);

	virtual void UpdateShapes(TimeValue t, TSTR& toolTip);
	virtual void GenerateShapes(TimeValue t);
	virtual void BeginEditParams( IObjParam  *ip, ULONG flags,Animatable *prev);
	virtual void EndEditParams( IObjParam *ip, ULONG flags,Animatable *next);

	//From Animatable
	virtual Class_ID ClassID() {return [!output CLASS_NAME]_CLASS_ID;}
	virtual SClass_ID SuperClassID() { return HELPER_CLASS_ID; }
	virtual void GetClassName(TSTR& s) {s = GetString(IDS_CLASS_NAME);}

	virtual RefTargetHandle Clone( RemapDir &remap );

	virtual int	NumParamBlocks() { return 1; }					// return number of ParamBlocks in this instance
	virtual IParamBlock2* GetParamBlock(int /*i*/) { return mpPblock; } // return i'th ParamBlock
	virtual IParamBlock2* GetParamBlockByID(BlockID id) { return (mpPblock->ID() == id) ? mpPblock : NULL; } // return id'd ParamBlock

	void DeleteThis() { delete this; }
};


[!output TEMPLATESTRING_CLASSDESC]

/*********************************************************************************************************
*
	If a manipulator applies to a node, this call will create an instance of it and add it to the 
	attached objects of the node.  It will then initialize the manipulator
*
\*****************************************************************************************************/
Manipulator * [!output CLASS_NAME]ClassDesc::CreateManipulator(RefTargetHandle /*hTarget*/, INode* /*pNode*/)
{
	return new [!output CLASS_NAME]();
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

class [!output CLASS_NAME]CreateCallBack : public CreateMouseCallBack {
	IPoint2 sp0;			//First point in screen coordinates
	[!output CLASS_NAME]* ob;		//Pointer to the object 
	Point3 p0;				//First point in world coordinates
public:	
	int proc( ViewExp *vpt,int msg, int point, int flags, IPoint2 m, Matrix3& mat);
	void SetObj([!output CLASS_NAME] *obj) {ob = obj;}
};


int [!output CLASS_NAME]CreateCallBack::proc(ViewExp *vpt,int msg, int point, int /*flags*/, IPoint2 m, Matrix3& mat )
{
	if ( ! vpt || ! vpt->IsAlive() )
	{
		// why are we here
		DbgAssert(!_T("Invalid viewport!"));
		return FALSE;
	}

	//TODO: Implement the mouse creation code here
	if (msg==MOUSE_POINT||msg==MOUSE_MOVE) {
		switch(point) {
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




[!output CLASS_NAME]::[!output CLASS_NAME]():SimpleManipulator(NULL)
{
	Get[!output CLASS_NAME]Desc()->MakeAutoParamBlocks(this);
}

[!output CLASS_NAME]::[!output CLASS_NAME](INode* pnode)
	: SimpleManipulator(pnode)
{
	Get[!output CLASS_NAME]Desc()->MakeAutoParamBlocks(this);
}


[!output CLASS_NAME]::~[!output CLASS_NAME]()
{

}

void [!output CLASS_NAME]::BeginEditParams( IObjParam * ip, ULONG flags,Animatable *prev)
{
	SimpleManipulator::BeginEditParams(ip,flags,prev);
	Get[!output CLASS_NAME]Desc()->BeginEditParams(ip, this, flags, prev);
}

void [!output CLASS_NAME]::EndEditParams( IObjParam* ip, ULONG flags,Animatable *next)
{
	SimpleManipulator::EndEditParams(ip,flags,next);
	Get[!output CLASS_NAME]Desc()->EndEditParams(ip, this, flags, next);
}


void [!output CLASS_NAME]::UpdateShapes(TimeValue t, TSTR& /*toolTip*/)
{
	GenerateShapes(t);
}

void [!output CLASS_NAME]::GenerateShapes(TimeValue )
{

}

void [!output CLASS_NAME]::OnMouseMove(TimeValue t, ViewExp* pVpt, IPoint2& m, DWORD flags, ManipHitData* pHitData)
{
	SimpleManipulator::OnMouseMove(t, pVpt, m, flags, pHitData);
}

void [!output CLASS_NAME]::OnButtonDown(TimeValue t, ViewExp* pVpt, IPoint2& m, DWORD flags, ManipHitData* pHitData)
{
	SimpleManipulator::OnButtonDown(t, pVpt, m, flags, pHitData);
}
	
void [!output CLASS_NAME]::OnButtonUp(TimeValue t, ViewExp* pVpt, IPoint2& m, DWORD flags, ManipHitData* pHitData)
{
	SimpleManipulator::OnButtonUp(t, pVpt, m, flags, pHitData);
}


CreateMouseCallBack* [!output CLASS_NAME]::GetCreateMouseCallBack(void) 
{ 
	[!output CLASS_NAME]CreateCB.SetObj(this);
	return(&[!output CLASS_NAME]CreateCB);
}


RefTargetHandle [!output CLASS_NAME]::Clone(RemapDir& remap) 
{
	[!output CLASS_NAME]* newob = new [!output CLASS_NAME]();	
	//TODO: Make a copy all the data and also clone all the references
	newob->ReplaceReference(0,remap.CloneRef(mpPblock));

	newob->mValid.SetEmpty();
	BaseClone(this, newob, remap);
	return(newob);
}


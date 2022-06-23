//**************************************************************************/
// Copyright 2016 Autodesk, Inc.  All rights reserved.
//
//  Use of this software is subject to the terms of the Autodesk license 
//  agreement provided at the time of installation or download, or which 
//  otherwise accompanies this software in either electronic or hard copy form.
//**************************************************************************/
// DESCRIPTION: SimplePolyObject sample
// AUTHOR: Tom Hudson
//
// This is an alternate version of the qtObjectDemo object which outputs
// an Editable Poly object via the SimplePolyObject class.  This is useful
// because these objects are handy building blocks for using the OpenSubdiv
// modifier.
//
// In this sample, a simple mesh is created and the slider control in the UI
// controls the Crease value, which gets applied to any edges between two
// faces.  By applying the OpenSubdiv modifier to this object, setting the
// Show End Result switch to ON, then checking the "Raised Center" switch
// and adjusting the slider, you will see the result of the Crease values
// which are being applied to the object, as the OpenSubdiv modifier alters
// the resulting geometry.
//***************************************************************************/

#include "QtObjectDemo.h"
#include "QtDemoObjectRollupWidget.h"
#include "MNMesh.h"
#include "SimplePolyObj.h"

// remove compiler errors:
#include <maxscript/maxscript.h>

#define QtPolyObjectDemo_CLASS_ID	Class_ID(0x5f402ed, 0x40c52fa7)

#define PBLOCK_REF	0

// block IDs:
enum { qt_params };

// enums for various parameters
enum {
	pb_float, pb_int, pb_bool, pb_creasevalue
};

class QtPolyObjectDemo : public SimplePolyObject
{
public:
	//Constructor/Destructor
	QtPolyObjectDemo();
	virtual ~QtPolyObjectDemo();

	
	static IObjParam *ip; // access to interface


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
	virtual Class_ID ClassID() {return QtPolyObjectDemo_CLASS_ID;}
	virtual SClass_ID SuperClassID() { return GEOMOBJECT_CLASS_ID; }
	virtual void GetClassName(TSTR& s) {s = GetString(IDS_POLY_CLASS_NAME);}
	virtual const MCHAR *GetObjectName() { return GetString(IDS_POLY_CLASS_NAME); }

	virtual RefTargetHandle Clone( RemapDir& remap );

	virtual int NumParamBlocks() { return 1; }					// return number of ParamBlocks in this instance
	virtual IParamBlock2* GetParamBlock(int /*i*/) { return pblock2; } // return i'th ParamBlock
	virtual IParamBlock2* GetParamBlockByID(BlockID id) { return (pblock2->ID() == id) ? pblock2 : NULL; } // return id'd ParamBlock

	void DeleteThis() { delete this; }

private: 
	// member variable
	double mObjSize=0;
};



class QtPolyObjectDemoClassDesc : public ClassDesc2 
{
public:
	virtual int IsPublic() 							{ return TRUE; }
	virtual void* Create(BOOL /*loading = FALSE*/) 		{ return new QtPolyObjectDemo(); }
	virtual const TCHAR *	ClassName() 			{ return GetString(IDS_POLY_CLASS_NAME); }
	virtual SClass_ID SuperClassID() 				{ return GEOMOBJECT_CLASS_ID; }
	virtual Class_ID ClassID() 						{ return QtPolyObjectDemo_CLASS_ID; }
	virtual const TCHAR* Category() 				{ return GetString(IDS_CATEGORY); }

	virtual const TCHAR* InternalName() 			{ return _T("QtPolyObjectDemo"); }	// returns fixed parsable name (scripter-visible name)
	virtual HINSTANCE HInstance() 					{ return hInstance; }					// returns owning module handle
	
	virtual MaxSDK::QMaxParamBlockWidget* CreateQtWidget(
		ReferenceMaker& /*owner*/,
		IParamBlock2& paramBlock,
		const MapID /*paramMapID*/,
		MSTR& rollupTitle,
		int& /*rollupFlags*/,
		int& /*rollupCategory*/) {

		if (paramBlock.ID() == qt_params) {
			rollupTitle = QtDemoObjectRollupWidget::tr("Qt PolyObject Demo Plugin");
			return new QtDemoObjectRollupWidget();
		}
		return nullptr;
	}

};


ClassDesc2* GetQtPolyObjectDemoDesc() {
	static QtPolyObjectDemoClassDesc qtPolyObjectDemoDesc;
	return &qtPolyObjectDemoDesc; 
}



static ParamBlockDesc2 qt_param_blk ( qt_params, _T("Qt_params"),  0, GetQtPolyObjectDemoDesc(), 
	P_AUTO_CONSTRUCT + P_AUTO_UI_QT, PBLOCK_REF, 
	// params
	pb_float, 			_T("qfloatspinbox"), 		TYPE_FLOAT, 	P_ANIMATABLE, 	IDS_SPIN, 
		p_default, 		10.0f, 
		p_range, 		0.1f,1000.0f, 
		p_end,	
	pb_int,	_T("dial"), TYPE_INT, P_ANIMATABLE, IDS_NUMVERTS,
		p_default, 10,
		p_range, 4, 100,
		p_end,
	pb_bool, _T("qcheckbox"), TYPE_BOOL, P_ANIMATABLE, IDS_NUMVERTS,
		p_default, FALSE,
		p_end,
	
	p_end
	);


IObjParam *QtPolyObjectDemo::ip = nullptr;

//--- QtPolyObjectDemo -------------------------------------------------------

QtPolyObjectDemo::QtPolyObjectDemo() 
{
	GetQtPolyObjectDemoDesc()->MakeAutoParamBlocks(this);
}

QtPolyObjectDemo::~QtPolyObjectDemo()
{
}

void QtPolyObjectDemo::BeginEditParams(IObjParam* ip, ULONG flags, Animatable* prev)
{
	this->ip = ip;
	SimplePolyObject::BeginEditParams(ip,flags,prev);
	GetQtPolyObjectDemoDesc()->BeginEditParams(ip, this, flags, prev);
}

void QtPolyObjectDemo::EndEditParams( IObjParam* ip, ULONG flags, Animatable* next )
{
	//TODO: Save plugin parameter values into class variables, if they are not hosted in ParamBlocks. 
	SimplePolyObject::EndEditParams(ip,flags,next);
	GetQtPolyObjectDemoDesc()->EndEditParams(ip, this, flags, next);
	this->ip = nullptr;
}

//From Object
BOOL QtPolyObjectDemo::HasUVW() 
{ 
	//TODO: Return whether the object has UVW coordinates or not
	return TRUE; 
}

void QtPolyObjectDemo::SetGenUVW(BOOL sw) 
{
	if (sw==HasUVW()) 
		return;
	//TODO: Set the plugin's internal value to sw
}

//Class for interactive creation of the object using the mouse
class QtPolyObjectDemoCreateCallBack : public CreateMouseCallBack {
	IPoint2 mScreenPt1;              //First point in screen coordinates
	QtPolyObjectDemo* mObj=nullptr; //Pointer to the object 
	Point3 mWorldPt1;                //First point in world coordinates
	Point3 mWorldPt2;	// We added this point.  Second point in world coords
public:	
	int proc( ViewExp *vpt,int msg, int point, int flags, IPoint2 m, Matrix3& mat);
	void SetObj(QtPolyObjectDemo *obj) {mObj = obj;}
};

int QtPolyObjectDemoCreateCallBack::proc(ViewExp *vpt,int msg, int point, int /*flags*/, IPoint2 m, Matrix3& mat )
{
	if ( ! vpt || ! vpt->IsAlive() )
	{
		// why are we here
		DbgAssert(!_T("Invalid viewport!"));
		return FALSE;
	}

	if (!mObj) {
		return FALSE;
	}

	if (msg == MOUSE_POINT || msg == MOUSE_MOVE) {
		switch(point) {
		case 0: // only happens with MOUSE_POINT msg
		{
			mScreenPt1 = m;
			mWorldPt1 = vpt->SnapPoint(m, m, NULL, SNAP_IN_PLANE);
			mat.SetTrans(mWorldPt1);

			// set size in paramblock:
			mObj->GetParamBlock(0)->SetValue(pb_float, mObj->ip->GetTime(), 0.0f);
			break;
		}

		case 1:
		{
			mWorldPt2 = vpt->SnapPoint(m, m, NULL, SNAP_IN_PLANE);

			float speedFactor = 24.0f;
			float theSize = (Length(mWorldPt2 - mWorldPt1) / speedFactor);

			//Set the overall size in parameter block
			mObj->GetParamBlock(0)->SetValue(pb_float, mObj->ip->GetTime(), theSize);

			//Invalidate and display the mesh in the viewport
			qt_param_blk.InvalidateUI();
			
			break;
		}
		case 2:
		{
			return CREATE_STOP;
		}
		

		}
	} else {
		if (msg == MOUSE_ABORT) return CREATE_ABORT;
	}

	return TRUE;
}

static QtPolyObjectDemoCreateCallBack qtObjectDemoCreateCB;

//From BaseObject
CreateMouseCallBack* QtPolyObjectDemo::GetCreateMouseCallBack() 
{

	qtObjectDemoCreateCB.SetObj(this);
	return(&qtObjectDemoCreateCB);
}

//From SimplePolyObject
void QtPolyObjectDemo::BuildMesh(TimeValue t)
{

	ivalid = FOREVER;
	float objSize;
	int numVerts;
	BOOL raiseCentre;
	int creaseIntValue;
	float creaseValue;

	pblock2->GetValue(pb_float, t, objSize, ivalid);
	pblock2->GetValue(pb_int, t, numVerts, ivalid);
	pblock2->GetValue(pb_bool, t, raiseCentre, ivalid);
	pblock2->GetValue(pb_creasevalue, t, creaseIntValue, ivalid);
	int numFaces = numVerts - 1;
	creaseValue = (float)creaseIntValue / 100.0f;

	polyMesh.setNumVerts(numVerts+1);
	polyMesh.setNumFaces(numFaces);

	// the centre
	Point3 centre= Point3(0.0, 0.0, 0.0);
	if (raiseCentre)
		centre = Point3(0.0, 0.0, objSize*0.1);
	
	polyMesh.v[0].p = objSize*centre;

	float step = 2 * pi / numVerts;

	// Create all the required vertices
	for (int i = 0; i < numVerts; i++) {
		float theta = i*step;
		polyMesh.v[i + 1].p = objSize*Point3( cos(theta), sin(theta), 0.0f);
	}

	// Create all the faces, all edges visible
	for (int i = 0; i < numFaces; i++) {
		MNFace& theFace = polyMesh.f[i];
		theFace.SetDeg(3);							// 3-sided face
		theFace.vtx[0] = 0;							// Set the vertex indexes
		theFace.vtx[1] = i + 1;
		theFace.vtx[2] = i + 2;
		theFace.visedg.SetAll();					// All edges visible
		theFace.smGroup = 2;						// Set smoothing
	}
	
	polyMesh.InvalidateGeomCache();

	// Set up the linkages in the mesh
	polyMesh.FillInMesh();

	// Now plug the crease values into the poly mesh edges
	
	// Set up crease data support
	polyMesh.setEDataSupport(EDATA_CREASE);
	// Get a pointer to the crease data
	float* creaseData = (float*)polyMesh.edgeData(EDATA_CREASE);
	// This data should be there.  If not, punt.
	if (!creaseData)
	{
		assert(0);
		return;
	}

	// Go through the edges and set a crease value on each...
	int numEdges = polyMesh.nume;
	for (int i = 0; i < numEdges; ++i) {
		MNEdge& theEdge = polyMesh.e[i];

		// Only edges with two faces get creased
		creaseData[i] = (theEdge.f2 >= 0) ? creaseValue : 0.0f;
	}
}

void QtPolyObjectDemo::InvalidateUI() 
{
	// Hey! Update the param blocks
	pblock2->GetDesc()->InvalidateUI();
}

Object* QtPolyObjectDemo::ConvertToType(TimeValue t, Class_ID obtype)
{
	//TODO: If the plugin can convert to a nurbs surface then check to see 
	//      whether obtype == EDITABLE_SURF_CLASS_ID and convert the object
	//      to nurbs surface and return the object
	//      If no special conversion is needed remove this implementation.
	
	return SimplePolyObject::ConvertToType(t,obtype);
}

int QtPolyObjectDemo::CanConvertToType(Class_ID obtype)
{
	//TODO: Check for any additional types the plugin supports
	//      If no special conversion is needed remove this implementation.
	return SimplePolyObject::CanConvertToType(obtype);
}

// From Object
int QtPolyObjectDemo::IntersectRay(TimeValue /*t*/, Ray& /*ray*/, float& /*at*/, Point3& /*norm*/)
{
	//TODO: Return TRUE after you implement this method
	return FALSE;
}

void QtPolyObjectDemo::GetCollapseTypes(Tab<Class_ID>& clist,Tab<TSTR*>& nlist)
{
	Object::GetCollapseTypes(clist, nlist);
	//TODO: Append any any other collapse type the plugin supports
}

// From ReferenceTarget
RefTargetHandle QtPolyObjectDemo::Clone(RemapDir& remap) 
{
	QtPolyObjectDemo* newob = new QtPolyObjectDemo();	
	//TODO: Make a copy all the data and also clone all the references
	newob->ReplaceReference(0,remap.CloneRef(pblock2));
	newob->ivalid.SetEmpty();
	BaseClone(this, newob, remap);
	return(newob);
}

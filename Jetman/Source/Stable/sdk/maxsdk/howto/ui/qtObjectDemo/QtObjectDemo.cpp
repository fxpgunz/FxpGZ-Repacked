//**************************************************************************/
// Copyright 2016 Autodesk, Inc.  All rights reserved.
//
//  Use of this software is subject to the terms of the Autodesk license 
//  agreement provided at the time of installation or download, or which 
//  otherwise accompanies this software in either electronic or hard copy form.
//**************************************************************************/
// DESCRIPTION: Simple GeomObject plug-in to illustrate creating a UI with Qt.
//
// This is a very SimpleObject2 plug-in that uses Qt as its UI.  It shows how
// to wire Qt widgets to Paramblock members of various types.  Most of these
// are contrived, but they show some of the steps necessary to integrate Qt.
// 
// AUTHOR: Drew Avis
//***************************************************************************/

#include "QtObjectDemo.h"
#include "QtDemoObjectRollupWidget.h"
#include <maxscript/maxscript.h>


#define QtObjectDemo_CLASS_ID	Class_ID(0x5567c0bb, 0xb2d50436)

#define PBLOCK_REF	0

// block IDs:
enum { qt_params };


// enums for various parameters
enum {
	pb_float, pb_int, pb_bool, pb_texmap, pb_point2, pb_point3, pb_point4, pb_matrix3
};

class QtObjectDemo : public SimpleObject2
{
public:
	//Constructor/Destructor
	QtObjectDemo();
	virtual ~QtObjectDemo();

	static IObjParam *ip; // access to interface

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
	virtual Class_ID ClassID() {return QtObjectDemo_CLASS_ID;}
	virtual SClass_ID SuperClassID() { return GEOMOBJECT_CLASS_ID; }
	virtual void GetClassName(TSTR& s) {s = GetString(IDS_CLASS_NAME);}

	virtual RefTargetHandle Clone( RemapDir& remap );

	virtual int NumParamBlocks() { return 1; }					// return number of ParamBlocks in this instance
	virtual IParamBlock2* GetParamBlock(int /*i*/) { return pblock2; } // return i'th ParamBlock
	virtual IParamBlock2* GetParamBlockByID(BlockID id) { return (pblock2->ID() == id) ? pblock2 : NULL; } // return id'd ParamBlock

	void DeleteThis() { delete this; }

private:
	// member variable
	double mObjSize;
	
};



class QtObjectDemoClassDesc : public ClassDesc2 
{
public:
	virtual int IsPublic() 							{ return TRUE; }
	virtual void* Create(BOOL /*loading = FALSE*/) 		{ return new QtObjectDemo(); }
	virtual const TCHAR *	ClassName() 			{ return GetString(IDS_CLASS_NAME); }
	virtual SClass_ID SuperClassID() 				{ return GEOMOBJECT_CLASS_ID; }
	virtual Class_ID ClassID() 						{ return QtObjectDemo_CLASS_ID; }
	virtual const TCHAR* Category() 				{ return GetString(IDS_CATEGORY); }

	virtual const TCHAR* InternalName() 			{ return _T("QtObjectDemo"); }	// returns fixed parsable name (scripter-visible name)
	virtual HINSTANCE HInstance() 					{ return hInstance; }					// returns owning module handle
	
	// This method handles creating our rollup from a Qt widget class.  In this case, we only create a single rollup, but
	// you can create multiple rollups based on the paramBlock.ID.
	virtual MaxSDK::QMaxParamBlockWidget* CreateQtWidget(
		ReferenceMaker& /*owner*/,
		IParamBlock2& paramBlock,
		const MapID /*paramMapID*/,
		MSTR& rollupTitle,
		int& /*rollupFlags*/,
		int& /*rollupCategory*/) {

		if (paramBlock.ID() == qt_params) {
			rollupTitle = QtDemoObjectRollupWidget::tr("Qt Demo Plugin");
			return new QtDemoObjectRollupWidget();
		}
		return nullptr;
	}

};


ClassDesc2* GetQtObjectDemoDesc() { 
	static QtObjectDemoClassDesc qtObjectDemoDesc;
	return &qtObjectDemoDesc; 
}

static ParamBlockDesc2 qt_param_blk ( qt_params, _T("Qt_params"),  0, GetQtObjectDemoDesc(), 
	// Note we use P_AUTO_UI_QT instead of P_AUTO_UI, and the widget-specific descriptors are not needed.
	P_AUTO_CONSTRUCT + P_AUTO_UI_QT, PBLOCK_REF, 
	// params
	pb_float, 			_T("qfloatspinbox"), 		TYPE_FLOAT, 	P_ANIMATABLE, IDS_NUMVERTS, // size
		p_default, 		10.0f, 
		p_range, 		0.1f,1000.0f, 
		p_end,	
	pb_int,	_T("dial"), TYPE_INT, P_ANIMATABLE, IDS_NUMVERTS,// # of vertices
		p_default, 10,
		p_range, 4, 100,
		p_end,
	pb_bool, _T("qcheckbox"), TYPE_BOOL, P_ANIMATABLE, IDS_NUMVERTS, // raise center
		p_default, FALSE,
		p_end,
	pb_texmap, _T("qbutton"), TYPE_TEXMAP, 0, IDS_NUMVERTS, // texmap button
		p_subtexno, 0,
		p_end,	
	/*
	pb_point2, _T("qspinPoint2"), TYPE_POINT2, P_ANIMATABLE,IDS_NUMVERTS, // qMaxPoint2Spinner
		p_default, Point2(0.0f,0.0f),
		p_end,
		*/

	// Note: These params connected to a qMax*Spinner require a default value, or max will crash
	pb_point3, _T("qspinPoint3"), TYPE_POINT3, P_ANIMATABLE, IDS_NUMVERTS, // qMaxPoint3Spinner	
		p_default, Point3(0.0f,0.0f,0.0f),
		p_end,
	pb_point4, _T("qspinPoint4"), TYPE_POINT4, P_ANIMATABLE, IDS_NUMVERTS, // qMaxPoint4Spinner
		p_default, Point4(0.0f, 0.0f, 0.0f, 0.0f),
		p_end,
	pb_matrix3, _T("qspinMatrix3"), TYPE_MATRIX3, P_ANIMATABLE, IDS_NUMVERTS, // qMaxMatrix3Spinner
		p_default, Matrix3(true),
		p_end,

	p_end
	);


IObjParam *QtObjectDemo::ip = nullptr;

//--- QtObjectDemo -------------------------------------------------------

QtObjectDemo::QtObjectDemo() 
{
	GetQtObjectDemoDesc()->MakeAutoParamBlocks(this);
}

QtObjectDemo::~QtObjectDemo()
{
}

void QtObjectDemo::BeginEditParams(IObjParam* ip, ULONG flags, Animatable* prev)
{
	this->ip = ip;
	SimpleObject2::BeginEditParams(ip,flags,prev);
	GetQtObjectDemoDesc()->BeginEditParams(ip, this, flags, prev);
}

void QtObjectDemo::EndEditParams( IObjParam* ip, ULONG flags, Animatable* next )
{
	SimpleObject2::EndEditParams(ip,flags,next);
	GetQtObjectDemoDesc()->EndEditParams(ip, this, flags, next);
	this->ip = nullptr;
}

//From Object
BOOL QtObjectDemo::HasUVW() 
{ 
	return TRUE; 
}

void QtObjectDemo::SetGenUVW(BOOL sw) 
{
	if (sw==HasUVW()) 
		return;
	//TODO: Set the plugin's internal value to sw
}

//Class for interactive creation of the object using the mouse
class QtObjectDemoCreateCallBack : public CreateMouseCallBack {
	IPoint2 mScreenPt1;              //First point in screen coordinates
	QtObjectDemo* mObj=nullptr; //Pointer to the object 
	Point3 mWorldPt1;                //First point in world coordinates
	Point3 mWorldPt2;	// We added this point.  Second point in world coords
public:	
	int proc( ViewExp *vpt,int msg, int point, int flags, IPoint2 m, Matrix3& mat);
	void SetObj(QtObjectDemo *obj) {mObj = obj;}
};

int QtObjectDemoCreateCallBack::proc(ViewExp *vpt,int msg, int point, int /*flags*/, IPoint2 m, Matrix3& mat )
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
			mObj->suspendSnap = TRUE;
			mScreenPt1 = m;
			mWorldPt1 = vpt->SnapPoint(m, m, NULL, SNAP_IN_PLANE);
			mat.SetTrans(mWorldPt1);

			// set size in paramblock:
			auto pb = mObj->GetParamBlock(qt_params);
			if (pb) {
				pb->SetValue(pb_float, mObj->ip->GetTime(), 0.0f);
			}
			break;
		}

		case 1:
		{
			
			mObj->suspendSnap = TRUE;
			mWorldPt2 = vpt->SnapPoint(m, m, NULL, SNAP_IN_PLANE);

			float speedFactor = 24.0f;
			float theSize = (Length(mWorldPt2 - mWorldPt1) / speedFactor);

			//Set the overall size in parameter block
			auto pb = mObj->GetParamBlock(qt_params);
			if (pb) {
				pb->SetValue(pb_float, mObj->ip->GetTime(), theSize);
			}

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

static QtObjectDemoCreateCallBack qtObjectDemoCreateCB;

//From BaseObject
CreateMouseCallBack* QtObjectDemo::GetCreateMouseCallBack() 
{

	qtObjectDemoCreateCB.SetObj(this);
	return(&qtObjectDemoCreateCB);

	/*return NULL;*/
}

//From SimpleObject
void QtObjectDemo::BuildMesh(TimeValue t)
{

	ivalid = FOREVER;
	float objSize;
	int numVerts;
	BOOL raiseCentre;
	Texmap* tx = nullptr;
	// Point2 pt2;
	Point3 pt3;
	Point4 pt4;
	Matrix3 mtx3;

	pblock2->GetValue(pb_float, t, objSize, ivalid);
	pblock2->GetValue(pb_int, t, numVerts, ivalid);
	pblock2->GetValue(pb_bool, t, raiseCentre, ivalid);
	pblock2->GetValue(pb_texmap, t, tx, ivalid);
	pblock2->GetValue(pb_point3, t, pt3, ivalid);
	pblock2->GetValue(pb_point4, t, pt4, ivalid);
	pblock2->GetValue(pb_matrix3, t, mtx3, ivalid);
	// Prove that we're getting data from the Qt rollup:
	mprintf(_T("pt3: %f %f %f\n"),pt3.x, pt3.y,pt3.z);
	mprintf(_T("pt4: %f %f %f %f\n"), pt4.x, pt4.y, pt4.z, pt4.w);
	for (int i = 0; i < 4; i++) {
		mprintf(_T("mtx3: row %d x:%f y:%f z:%f \n"), i, mtx3.GetRow(i).x, mtx3.GetRow(i).y, mtx3.GetRow(i).z );
	}
	
	mflush();


	// You don't normally associate a texmap with a geomobj, so this is a contrived
	// use of the param.  
	int divby = 1;
	if (tx) {
		divby = tx->NumSubTexmaps();
		if (divby == 0) divby = 1;
	}
	numVerts /= divby;

	int numFaces = numVerts - 1;	

	mesh.setNumVerts(numVerts+1);
	mesh.setNumFaces(numFaces);

	// the centre
	Point3 centre= Point3(0.0, 0.0, 0.0);
	if (raiseCentre)
		centre = Point3(0.0, 0.0, objSize*0.1);
	
	mesh.setVert(0, objSize*centre);

	float step = 2 * pi / numVerts;

	for (int i = 0; i < numVerts; i++) {
		float theta = i*step;
		mesh.setVert(i+1, objSize*Point3( cos(theta), sin(theta), 0.0f));
	}

	for (int i = 0; i < numFaces; i++) {
		mesh.faces[i].setVerts(0, i + 1, i + 2);
		mesh.faces[i].setEdgeVisFlags(1, 1, 1);
		mesh.faces[i].setSmGroup(2);
	}
	
	mesh.InvalidateGeomCache();

}

void QtObjectDemo::InvalidateUI() 
{
	// Hey! Update the param blocks
	pblock2->GetDesc()->InvalidateUI();
}

Object* QtObjectDemo::ConvertToType(TimeValue t, Class_ID obtype)
{
	//TODO: If the plugin can convert to a nurbs surface then check to see 
	//      whether obtype == EDITABLE_SURF_CLASS_ID and convert the object
	//      to nurbs surface and return the object
	//      If no special conversion is needed remove this implementation.
	
	return __super::ConvertToType(t,obtype);
}

int QtObjectDemo::CanConvertToType(Class_ID obtype)
{
	//TODO: Check for any additional types the plugin supports
	//      If no special conversion is needed remove this implementation.
	return __super::CanConvertToType(obtype);
}

// From Object
int QtObjectDemo::IntersectRay(TimeValue /*t*/, Ray& /*ray*/, float& /*at*/, Point3& /*norm*/)
{
	//TODO: Return TRUE after you implement this method
	return FALSE;
}

void QtObjectDemo::GetCollapseTypes(Tab<Class_ID>& clist,Tab<TSTR*>& nlist)
{
	Object::GetCollapseTypes(clist, nlist);
	//TODO: Append any any other collapse type the plugin supports
}

// From ReferenceTarget
RefTargetHandle QtObjectDemo::Clone(RemapDir& remap) 
{
	QtObjectDemo* newob = new QtObjectDemo();	
	//TODO: Make a copy all the data and also clone all the references
	newob->ReplaceReference(0,remap.CloneRef(pblock2));
	newob->ivalid.SetEmpty();
	BaseClone(this, newob, remap);
	return(newob);
}

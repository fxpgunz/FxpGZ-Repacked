/**********************************************************************
 *<
	FILE: Widget.cpp

	DESCRIPTION:	This plugin shows how to:
					
					Create a simple procedural object for 3ds Max.

					This Visual Studio project accompanies an article
					in the 3ds Max SDK Help file that walks a developer, 
					who is new to the 3ds Max SDK, through the process of
					writing a basic procedural object.

	OVERVIEW:		The widget object is a simple procedural object that can be
					created in the 3ds Max viewport. It is located in the create
					command panel under the category "Max SDK tutorials". 

					A widget looks like a collapsed oblique box with two attached
					'boxes' on each side.
					
					A widget has three parameters:

					Size: A float value that governs it's overall size.

					Left: A float value that determines the extension of the box
						  on the left side of the widget

				    Right: A float value that determines the extension of the box
						  on the right side of the widget.

				    This project was created using the Visual Studio Application
					wizard for 3ds Max. Hence many TODO comments are still in the
					body of the plugin.

					The most important functions that were implemented were:

					int WidgetCreateCallBack::proc(... ...)
					void Widget::BuildMesh(TimeValue t)

					The parameter block implemented was:

					static ParamBlockDesc2 widget_param_blk(... ...)
					
	CREATED BY:		Chris Johnson
		
	HISTORY:		Code started in April 2005

 *>	Copyright (c) 2000, All Rights Reserved.
 **********************************************************************/

#include "Widget.h"

#define Widget_CLASS_ID	Class_ID(0x986df9b4, 0x792ce6d7)

#define PBLOCK_REF	0

class Widget : public SimpleObject2 {
	friend class WidgetCreateCallBack;
	public:
		// Parameter block handled by parent

		//Class vars
		static IObjParam *ip;			//Access to the interface
		// From BaseObject
		CreateMouseCallBack* GetCreateMouseCallBack();
		
		// From Object
		BOOL HasUVW();
		void SetGenUVW(BOOL sw);
		void GetCollapseTypes(Tab<Class_ID> &clist,Tab<TSTR*> &nlist);
		int IntersectRay(TimeValue t, Ray& ray, float& at, Point3& norm);

		// From Animatable
		void BeginEditParams( IObjParam  *ip, ULONG flags,Animatable *prev);
		void EndEditParams( IObjParam *ip, ULONG flags,Animatable *next);

		// From SimpleObject
		void BuildMesh(TimeValue t);
		BOOL OKtoDisplay(TimeValue t);
		void InvalidateUI();
		
		//From Animatable
		Class_ID ClassID() {return Widget_CLASS_ID;}		
		SClass_ID SuperClassID() { return GEOMOBJECT_CLASS_ID; }
		void GetClassName(TSTR& s) {s = GetString(IDS_CLASS_NAME);}

		RefTargetHandle Clone( RemapDir &remap );

		//This returns a name that will be used to automatically name the object
		//when it gets created
		const TCHAR *GetObjectName() { return GetString(IDS_CLASS_NAME); }

		void DeleteThis() { delete this; }		
		
		//Constructor/Destructor
		Widget();
		~Widget();
};



class WidgetClassDesc : public ClassDesc2 {
	public:
	int 			IsPublic() { return TRUE; }
	void *			Create(BOOL loading = FALSE) { return new Widget(); }
	const TCHAR *	ClassName() { return GetString(IDS_CLASS_NAME); }
	SClass_ID		SuperClassID() { return GEOMOBJECT_CLASS_ID; }
	Class_ID		ClassID() { return Widget_CLASS_ID; }
	const TCHAR* 	Category() { return GetString(IDS_CATEGORY); }

	const TCHAR*	InternalName() { return _T("Widget"); }	// returns fixed parsable name (scripter-visible name)
	HINSTANCE		HInstance() { return hInstance; }					// returns owning module handle
	

};

static WidgetClassDesc WidgetDesc;
ClassDesc2* GetWidgetDesc() { return &WidgetDesc; }


//TODO: Add enumerations for each parameter block
enum { widget_params };


//TODO: Add enumerations for various parameters
enum { 
	widget_size,
	widget_left,
	widget_right
};


static ParamBlockDesc2 widget_param_blk ( 
	//Required arguments ----------------------------
	widget_params, _T("params"),  0, &WidgetDesc,
	//flags
	P_AUTO_CONSTRUCT + P_AUTO_UI,

	//Dependent arguments ---------------------------
	//required because P_AUTO_CONSTRUCT was flagged
	//This declares the number of rollouts
	PBLOCK_REF,
	//required because P_AUTO_UI was flagged. 
	//This is the Rollout description
	IDD_PANEL, IDS_PARAMS, 0, 0, NULL,

	//Parameter Specifications ----------------------
	// For each control create a parameter:
	widget_size, 		_T("Size"), 		TYPE_FLOAT, 	P_ANIMATABLE, 	IDS_SPIN, 
		//Zero or more optional tags
		p_default, 		1.0f, 
		p_range, 		0.0f,1000.0f, 
		p_ui, 			TYPE_SPINNER,		EDITTYPE_FLOAT, IDC_SIZE_EDIT,	IDC_SIZE_SPIN, 0.50f, 
		p_end,
	widget_left,		_T("Left"), 		TYPE_FLOAT, 	P_ANIMATABLE, 	IDS_SPIN, 
		//Zero or more optional tags	
		p_default, 		0.0f, 
		p_range, 		0.0f,100.0f, 
		p_ui, 			TYPE_SPINNER,		EDITTYPE_UNIVERSE, IDC_LEFT_EDIT,	IDC_LEFT_SPIN, 0.50f, 
		p_end,
	widget_right,		_T("Right"), 		TYPE_FLOAT, 	P_ANIMATABLE, 	IDS_SPIN, 
		//Zero or more optional tags	
		p_default, 		0.0f, 
		p_range, 		0.0f,100.0f, 
		p_ui, 			TYPE_SPINNER,		EDITTYPE_UNIVERSE, IDC_RIGHT_EDIT,	IDC_RIGHT_SPIN, 0.50f, 
		p_end,
	p_end
	);


IObjParam *Widget::ip			= NULL;

//--- Widget -------------------------------------------------------

Widget::Widget()
{
	WidgetDesc.MakeAutoParamBlocks(this);

}

Widget::~Widget()
{
}

void 
Widget::BeginEditParams(IObjParam *ip,ULONG flags,Animatable *prev)
{
	this->ip = ip;

	SimpleObject2::BeginEditParams(ip,flags,prev);
	WidgetDesc.BeginEditParams(ip, this, flags, prev);
}

void 
Widget::EndEditParams( IObjParam *ip, ULONG flags, Animatable *next )
{
	//TODO: Save plugin parameter values into class variables, if they are not hosted in ParamBlocks. 
	
	SimpleObject2::EndEditParams(ip,flags,next);
	WidgetDesc.EndEditParams(ip, this, flags, next);

	this->ip = NULL;
}

//From Object
BOOL 
Widget::HasUVW() 
{ 
	//TODO: Return whether the object has UVW coordinates or not
	return TRUE; 
}

void 
Widget::SetGenUVW(BOOL sw) 
{  
	if (sw==HasUVW()) return;
	//TODO: Set the plugin's internal value to sw				
}

//Class for interactive creation of the object using the mouse
class WidgetCreateCallBack : public CreateMouseCallBack {
	IPoint2 sp0;	//First point in screen coordinates
	Widget *ob;		//Pointer to the object 
	Point3 p0;		//First point in world coordinates
	Point3 p1;		//We added this point. Second point in world coordinates.
public:	
	int proc( ViewExp *vpt,int msg, int point, int flags, IPoint2 m, Matrix3& mat);
	void SetObj(Widget *obj) {ob = obj;}
};

int 
WidgetCreateCallBack::proc(ViewExp *vpt,int msg, int point, int flags, IPoint2 m, Matrix3& mat )
{

	if ( ! vpt || ! vpt->IsAlive() )
	{
		// why are we here
		DbgAssert(!_T("Invalid viewport!"));
		return FALSE;
	}

	if (msg==MOUSE_POINT||msg==MOUSE_MOVE) {
		switch(point) 
		{
			case 0: // only happens with MOUSE_POINT msg
				ob->suspendSnap = TRUE;
				sp0 = m;
				p0 = vpt->SnapPoint(m,m,NULL,SNAP_IN_PLANE);
				mat.SetTrans(p0);

				//Set a default overall size in the parameter block
				ob->pblock2->SetValue(widget_size, ob->ip->GetTime(), 0.0f);
				break;
			case 1:
			{
				ob->suspendSnap = TRUE; 
				p1 = vpt->SnapPoint(m,m,NULL,SNAP_IN_PLANE);
				
				float speedFactor = 24.0f;
				float theSize = (Length(p1 - p0) / speedFactor);
				
				//Set the overall size in parameter block
				ob->pblock2->SetValue(widget_size, ob->ip->GetTime(), theSize);

				//Invalidate and display the mesh in the viewport
				widget_param_blk.InvalidateUI();
				break;
			}
			case 2:
			{
				return CREATE_STOP;
			}
		}
	} 
	else {
		if (msg == MOUSE_ABORT) return CREATE_ABORT;
	}
	return TRUE;
}

static WidgetCreateCallBack WidgetCreateCB;

//From BaseObject
CreateMouseCallBack* Widget::GetCreateMouseCallBack() 
{
	WidgetCreateCB.SetObj(this);
	return(&WidgetCreateCB);
}

//From SimpleObject
void 
Widget::BuildMesh(TimeValue t)
{
	// SimpleObject has a ivalid member that stores the validity of the object.
	// We need to update the validity here. The mesh is supposed to be always valid.
	// If values controlling the mesh are animated they will updated the validity interval
	// when GetValue is called.
	ivalid = FOREVER;
	float size, sizeLeft, sizeRight;
	pblock2->GetValue(widget_size, t, size, ivalid );
	pblock2->GetValue(widget_left, t, sizeLeft, ivalid );
	pblock2->GetValue(widget_right, t, sizeRight, ivalid );

	// Update mesh using these parameters with the edited content of widgetmesh.h
	mesh.setNumVerts(24);
	mesh.setNumFaces(44);
	mesh.setVert(0,size*Point3(-40.0,-55.0,0.0)); 
	mesh.setVert(1,size*Point3(16.0,-55.0,0.0)); 
	mesh.setVert(2,size*Point3(-40.0,23.0,0.0)); 
	mesh.setVert(3,size*Point3(16.0,23.0,0.0)); 
	mesh.setVert(4,size*Point3(-40.0,-28.0,84.0)); 
	mesh.setVert(5,size*Point3(16.0,-28.0,84.0)); 
	mesh.setVert(6,size*Point3(-40.0,23.0,84.0)); 
	mesh.setVert(7,size*Point3(16.0,23.0,84.0)); 
	mesh.setVert(8,size*Point3(16.0,-23.0,4.6)); 
	mesh.setVert(9,size*Point3(16.0,18.0,4.6)); 
	mesh.setVert(10,size*Point3(16.0,18.0,80.0)); 
	mesh.setVert(11,size*Point3(16.0,-23.0,80.0)); 
	mesh.setVert(16,size*Point3(27.0+sizeRight,-23.0,4.6)); 
	mesh.setVert(17,size*Point3(27.0+sizeRight,18.0,4.6)); 
	mesh.setVert(18,size*Point3(27.0+sizeRight,18.0,80.0)); 
	mesh.setVert(19,size*Point3(27.0+sizeRight,-23.0,80.0)); 
	mesh.setVert(12,size*Point3(-40.0,18.0,4.6)); 
	mesh.setVert(13,size*Point3(-40.0,-23.0,4.6)); 
	mesh.setVert(14,size*Point3(-40.0,-23.0,80.0)); 
	mesh.setVert(15,size*Point3(-40.0,18.0,80.0)); 
	mesh.setVert(20,size*Point3(-50.0-sizeLeft,18.0,4.6)); 
	mesh.setVert(21,size*Point3(-50.0-sizeLeft,-23.0,4.6)); 
	mesh.setVert(22,size*Point3(-50.0-sizeLeft,-23.0,80.0)); 
	mesh.setVert(23,size*Point3(-50.0-sizeLeft,18.0,80.0)); 
	mesh.faces[0].setVerts(0, 2, 3);
	mesh.faces[0].setEdgeVisFlags(1,1,0);
	mesh.faces[0].setSmGroup(2);
	mesh.faces[1].setVerts(3, 1, 0);
	mesh.faces[1].setEdgeVisFlags(1,1,0);
	mesh.faces[1].setSmGroup(2);
	mesh.faces[2].setVerts(4, 5, 7);
	mesh.faces[2].setEdgeVisFlags(1,1,0);
	mesh.faces[2].setSmGroup(4);
	mesh.faces[3].setVerts(7, 6, 4);
	mesh.faces[3].setEdgeVisFlags(1,1,0);
	mesh.faces[3].setSmGroup(4);
	mesh.faces[4].setVerts(0, 1, 5);
	mesh.faces[4].setEdgeVisFlags(1,1,0);
	mesh.faces[4].setSmGroup(8);
	mesh.faces[5].setVerts(5, 4, 0);
	mesh.faces[5].setEdgeVisFlags(1,1,0);
	mesh.faces[5].setSmGroup(8);
	mesh.faces[6].setVerts(16, 17, 18);
	mesh.faces[6].setEdgeVisFlags(1,1,0);
	mesh.faces[6].setSmGroup(16);
	mesh.faces[7].setVerts(18, 19, 16);
	mesh.faces[7].setEdgeVisFlags(1,1,0);
	mesh.faces[7].setSmGroup(16);
	mesh.faces[8].setVerts(3, 2, 6);
	mesh.faces[8].setEdgeVisFlags(1,1,0);
	mesh.faces[8].setSmGroup(32);
	mesh.faces[9].setVerts(6, 7, 3);
	mesh.faces[9].setEdgeVisFlags(1,1,0);
	mesh.faces[9].setSmGroup(32);
	mesh.faces[10].setVerts(20, 21, 22);
	mesh.faces[10].setEdgeVisFlags(1,1,0);
	mesh.faces[10].setSmGroup(64);
	mesh.faces[11].setVerts(22, 23, 20);
	mesh.faces[11].setEdgeVisFlags(1,1,0);
	mesh.faces[11].setSmGroup(64);
	mesh.faces[12].setVerts(1, 3, 9);
	mesh.faces[12].setEdgeVisFlags(1,1,0);
	mesh.faces[12].setSmGroup(0);
	mesh.faces[13].setVerts(9, 8, 1);
	mesh.faces[13].setEdgeVisFlags(1,1,0);
	mesh.faces[13].setSmGroup(0);
	mesh.faces[14].setVerts(3, 7, 10);
	mesh.faces[14].setEdgeVisFlags(1,1,0);
	mesh.faces[14].setSmGroup(0);
	mesh.faces[15].setVerts(10, 9, 3);
	mesh.faces[15].setEdgeVisFlags(1,1,0);
	mesh.faces[15].setSmGroup(0);
	mesh.faces[16].setVerts(7, 5, 11);
	mesh.faces[16].setEdgeVisFlags(1,1,0);
	mesh.faces[16].setSmGroup(0);
	mesh.faces[17].setVerts(11, 10, 7);
	mesh.faces[17].setEdgeVisFlags(1,1,0);
	mesh.faces[17].setSmGroup(0);
	mesh.faces[18].setVerts(5, 1, 8);
	mesh.faces[18].setEdgeVisFlags(1,1,0);
	mesh.faces[18].setSmGroup(0);
	mesh.faces[19].setVerts(8, 11, 5);
	mesh.faces[19].setEdgeVisFlags(1,1,0);
	mesh.faces[19].setSmGroup(0);
	mesh.faces[20].setVerts(2, 0, 13);
	mesh.faces[20].setEdgeVisFlags(1,1,0);
	mesh.faces[20].setSmGroup(0);
	mesh.faces[21].setVerts(13, 12, 2);
	mesh.faces[21].setEdgeVisFlags(1,1,0);
	mesh.faces[21].setSmGroup(0);
	mesh.faces[22].setVerts(0, 4, 14);
	mesh.faces[22].setEdgeVisFlags(1,1,0);
	mesh.faces[22].setSmGroup(0);
	mesh.faces[23].setVerts(14, 13, 0);
	mesh.faces[23].setEdgeVisFlags(1,1,0);
	mesh.faces[23].setSmGroup(0);
	mesh.faces[24].setVerts(4, 6, 15);
	mesh.faces[24].setEdgeVisFlags(1,1,0);
	mesh.faces[24].setSmGroup(0);
	mesh.faces[25].setVerts(15, 14, 4);
	mesh.faces[25].setEdgeVisFlags(1,1,0);
	mesh.faces[25].setSmGroup(0);
	mesh.faces[26].setVerts(6, 2, 12);
	mesh.faces[26].setEdgeVisFlags(1,1,0);
	mesh.faces[26].setSmGroup(0);
	mesh.faces[27].setVerts(12, 15, 6);
	mesh.faces[27].setEdgeVisFlags(1,1,0);
	mesh.faces[27].setSmGroup(0);
	mesh.faces[28].setVerts(8, 9, 17);
	mesh.faces[28].setEdgeVisFlags(1,1,0);
	mesh.faces[28].setSmGroup(0);
	mesh.faces[29].setVerts(17, 16, 8);
	mesh.faces[29].setEdgeVisFlags(1,1,0);
	mesh.faces[29].setSmGroup(0);
	mesh.faces[30].setVerts(9, 10, 18);
	mesh.faces[30].setEdgeVisFlags(1,1,0);
	mesh.faces[30].setSmGroup(0);
	mesh.faces[31].setVerts(18, 17, 9);
	mesh.faces[31].setEdgeVisFlags(1,1,0);
	mesh.faces[31].setSmGroup(0);
	mesh.faces[32].setVerts(10, 11, 19);
	mesh.faces[32].setEdgeVisFlags(1,1,0);
	mesh.faces[32].setSmGroup(0);
	mesh.faces[33].setVerts(19, 18, 10);
	mesh.faces[33].setEdgeVisFlags(1,1,0);
	mesh.faces[33].setSmGroup(0);
	mesh.faces[34].setVerts(11, 8, 16);
	mesh.faces[34].setEdgeVisFlags(1,1,0);
	mesh.faces[34].setSmGroup(0);
	mesh.faces[35].setVerts(16, 19, 11);
	mesh.faces[35].setEdgeVisFlags(1,1,0);
	mesh.faces[35].setSmGroup(0);
	mesh.faces[36].setVerts(12, 13, 21);
	mesh.faces[36].setEdgeVisFlags(1,1,0);
	mesh.faces[36].setSmGroup(0);
	mesh.faces[37].setVerts(21, 20, 12);
	mesh.faces[37].setEdgeVisFlags(1,1,0);
	mesh.faces[37].setSmGroup(0);
	mesh.faces[38].setVerts(13, 14, 22);
	mesh.faces[38].setEdgeVisFlags(1,1,0);
	mesh.faces[38].setSmGroup(0);
	mesh.faces[39].setVerts(22, 21, 13);
	mesh.faces[39].setEdgeVisFlags(1,1,0);
	mesh.faces[39].setSmGroup(0);
	mesh.faces[40].setVerts(14, 15, 23);
	mesh.faces[40].setEdgeVisFlags(1,1,0);
	mesh.faces[40].setSmGroup(0);
	mesh.faces[41].setVerts(23, 22, 14);
	mesh.faces[41].setEdgeVisFlags(1,1,0);
	mesh.faces[41].setSmGroup(0);
	mesh.faces[42].setVerts(15, 12, 20);
	mesh.faces[42].setEdgeVisFlags(1,1,0);
	mesh.faces[42].setSmGroup(0);
	mesh.faces[43].setVerts(20, 23, 15);
	mesh.faces[43].setEdgeVisFlags(1,1,0);
	mesh.faces[43].setSmGroup(0);

	mesh.InvalidateGeomCache();
}

BOOL 
Widget::OKtoDisplay(TimeValue t) 
{
	BOOL displayOk = TRUE;
	float size = pblock2->GetFloat(widget_size, t);
	if ( size <= 0.0f ) {
		// Do not display when size is not valid.
		displayOk = FALSE;
	}
	return displayOk;
}

void 
Widget::InvalidateUI() 
{
	// Hey! Update the param blocks
	pblock2->GetDesc()->InvalidateUI();
}

// From Object
int 
Widget::IntersectRay(
		TimeValue t, Ray& ray, float& at, Point3& norm)
{
	//TODO: Return TRUE after you implement this method
	return FALSE;
}

void 
Widget::GetCollapseTypes(Tab<Class_ID> &clist,Tab<TSTR*> &nlist)
{
    Object::GetCollapseTypes(clist, nlist);
	//TODO: Append any any other collapse type the plugin supports
}

// From ReferenceTarget
RefTargetHandle Widget::Clone(RemapDir& remap) 
{
	Widget* newob = new Widget();	
	// Make a copy all the data and also clone all the references
	newob->ReplaceReference(0,remap.CloneRef(pblock2));
	newob->ivalid.SetEmpty();
	BaseClone(this, newob, remap);
	return(newob);
}

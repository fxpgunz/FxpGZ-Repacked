/**********************************************************************
 *<
	FILE: PrimitiveRenderer.cpp

	DESCRIPTION:	This plugin shows how to:
					
					Use diplayCallback to display primitive for 3ds Max.

					This Visual Studio project accompanies an article
					in the 3ds Max SDK Help file that walks a developer, 
					who is new to the 3ds Max SDK, through the process of
					writing a basic procedural object.

	OVERVIEW:		The PrimitiveRenderer is a simple procedure that show how to 
	                use displayCallback to display primitives. It is located in the 
					create command panel under the category	"Max SDK tutorials". 
															
					A PrimitiveRenderer has three parameters:

					Size: A float value that governs it's overall size.

					Left: A float value that determines the extension of the box
						  on the left side of the object

				    Right: A float value that determines the extension of the box
						  on the right side of the object.

					DrawMarker: A int value that choose to draw plane or marker.

					This project was created using the Visual Studio Application
					wizard for 3ds Max. Hence many TODO comments are still in the
					body of the plugin.

					The most important class that were implemented were:
					Class DemoCallback : IDisplayCallback

					The most important functions that were implemented were:
					void PrimitiveRenderer::BuildMesh(TimeValue t)
					bool PrimitiveRenderer::UpdateDisplay(...)					

					The parameter block implemented was:

					static ParamBlockDesc2 PrimitiveRenderer_param_blk(... ...)
					
	CREATED BY:		Tingzhu Zhou
		
	HISTORY:		Code started in Dec 2011

 *>	Copyright (c) 2000, All Rights Reserved.
 **********************************************************************/

#include "PrimitiveRenderer.h"
#include "DrawIndexedPrimitiveCallback.h"
#include <Graphics/ImmediateRenderItemHandle.h>

#define PrimitiveRenderer_CLASS_ID	Class_ID(0x986df9b4, 0x792ce6d9)

#define PBLOCK_REF	0

using namespace MaxSDK::Graphics;

// Add enumerations for various parameters
enum PrimitiveRenderer_DrawType
{
	Callback_DrawIndexPrimitive,	
	Callback_MaxNum
};

class PrimitiveRenderer : public SimpleObject2 {
	friend class PrimitiveRendererCreateCallBack;
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
		void InvalidateUI();
		
		//From Animatable
		Class_ID ClassID() {return PrimitiveRenderer_CLASS_ID;}		
		SClass_ID SuperClassID() { return GEOMOBJECT_CLASS_ID; }
		void GetClassName(TSTR& s) {s = GetString(IDS_CLASS_NAME);}

		// display functions from BaseObject
		//virtual int HitTest(TimeValue t, INode *inode, int type, int crossing, int flags, IPoint2 *p, ViewExp *vpt);
		bool PrepareDisplay(
			const MaxSDK::Graphics::UpdateDisplayContext& prepareDisplayContext);
		bool UpdatePerNodeItems(
			const MaxSDK::Graphics::UpdateDisplayContext& updateDisplayContext,
			MaxSDK::Graphics::UpdateNodeContext& nodeContext,
			MaxSDK::Graphics::IRenderItemContainer& targetRenderItemContainer);

		RefTargetHandle Clone( RemapDir &remap );

		//This returns a name that will be used to automatically name the object when it gets created
		const TCHAR *GetObjectName() { return GetString(IDS_CLASS_NAME); }

		void DeleteThis() { delete this; }		
		
		//Constructor/Destructor
		PrimitiveRenderer();
		~PrimitiveRenderer();
	protected:		
		BasePrimitiveDisplayCallbackPtr pDemoCallback;
};



class PrimitiveRendererClassDesc : public ClassDesc2 {
	public:
	int 			IsPublic() { return TRUE; }
	void *			Create(BOOL loading = FALSE) { return new PrimitiveRenderer(); }
	const TCHAR *	ClassName() { return GetString(IDS_CLASS_NAME); }
	SClass_ID		SuperClassID() { return GEOMOBJECT_CLASS_ID; }
	Class_ID		ClassID() { return PrimitiveRenderer_CLASS_ID; }
	const TCHAR* 	Category() { return GetString(IDS_CATEGORY); }

	const TCHAR*	InternalName() { return _T("PrimitiveRenderer"); }	// returns fixed parsable name (scripter-visible name)
	HINSTANCE		HInstance() { return hInstance; }					// returns owning module handle
	

};

static PrimitiveRendererClassDesc PrimitiveRendererDesc;
ClassDesc2* GetPrimitiveRendererDesc() { return &PrimitiveRendererDesc; }

//TODO: Add enumerations for each parameter block
enum { PrimitiveRenderer_params };


//TODO: Add enumerations for various parameters
enum { 	
	PrimitiveRenderer_drawPrimitive,
	PrimitiveRenderer_size,
	PrimitiveRenderer_left,
	PrimitiveRenderer_right	
};


static ParamBlockDesc2 PrimitiveRenderer_param_blk ( 
	//Required arguments ----------------------------
	PrimitiveRenderer_params, _T("params"),  0, &PrimitiveRendererDesc,
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
	PrimitiveRenderer_drawPrimitive,     _T("DrawPrimitive"),     TYPE_INT,     0,    IDS_SPIN, 
		p_default, 0,
		p_ui,      TYPE_RADIO,    1,  IDC_DRAW_PRIMITIVE, 
		p_end,
	PrimitiveRenderer_size, 		_T("Size"), 		TYPE_FLOAT, 	P_ANIMATABLE, 	IDS_SPIN, 
		//Zero or more optional tags
		p_default, 		1.0f, 
		p_range, 		0.0f,1000.0f, 
		p_ui, 			TYPE_SPINNER,		EDITTYPE_FLOAT, IDC_SIZE_EDIT,	IDC_SIZE_SPIN, 0.50f, 
		p_end,
	PrimitiveRenderer_left,		_T("Left"), 		TYPE_FLOAT, 	P_ANIMATABLE, 	IDS_SPIN, 
		//Zero or more optional tags	
		p_default, 		0.0f, 
		p_range, 		0.0f,100.0f, 
		p_ui, 			TYPE_SPINNER,		EDITTYPE_UNIVERSE, IDC_LEFT_EDIT,	IDC_LEFT_SPIN, 0.50f, 
		p_end,
	PrimitiveRenderer_right,		_T("Right"), 		TYPE_FLOAT, 	P_ANIMATABLE, 	IDS_SPIN, 
		//Zero or more optional tags	
		p_default, 		0.0f, 
		p_range, 		0.0f,100.0f, 
		p_ui, 			TYPE_SPINNER,		EDITTYPE_UNIVERSE, IDC_RIGHT_EDIT,	IDC_RIGHT_SPIN, 0.50f, 
		p_end,
	p_end
	);


IObjParam *PrimitiveRenderer::ip			= NULL;

//--- PrimitiveRenderer -------------------------------------------------------

PrimitiveRenderer::PrimitiveRenderer()
{
	pDemoCallback = NULL;	
	PrimitiveRendererDesc.MakeAutoParamBlocks(this);	
}

PrimitiveRenderer::~PrimitiveRenderer()
{
}

void 
PrimitiveRenderer::BeginEditParams(IObjParam *ip,ULONG flags,Animatable *prev)
{
	this->ip = ip;

	SimpleObject2::BeginEditParams(ip,flags,prev);
	PrimitiveRendererDesc.BeginEditParams(ip, this, flags, prev);
}

void 
PrimitiveRenderer::EndEditParams( IObjParam *ip, ULONG flags, Animatable *next )
{
	//TODO: Save plugin parameter values into class variables, if they are not hosted in ParamBlocks. 
	
	SimpleObject2::EndEditParams(ip,flags,next);
	PrimitiveRendererDesc.EndEditParams(ip, this, flags, next);

	this->ip = NULL;
}

//From Object
BOOL 
PrimitiveRenderer::HasUVW() 
{ 
	//TODO: Return whether the object has UVW coordinates or not
	return TRUE; 
}

void 
PrimitiveRenderer::SetGenUVW(BOOL sw) 
{  
	if (sw==HasUVW()) return;
	//TODO: Set the plugin's internal value to sw				
}

//Class for interactive creation of the object using the mouse
class PrimitiveRendererCreateCallBack : public CreateMouseCallBack {
	IPoint2 sp0;	//First point in screen coordinates
	PrimitiveRenderer *ob;		//Pointer to the object 
	Point3 p0;		//First point in world coordinates
	Point3 p1;		//We added this point. Second point in world coordinates.
public:	
	int proc( ViewExp *vpt,int msg, int point, int flags, IPoint2 m, Matrix3& mat);
	void SetObj(PrimitiveRenderer *obj) {ob = obj;}
};

int 
PrimitiveRendererCreateCallBack::proc(ViewExp *vpt,int msg, int point, int flags, IPoint2 m, Matrix3& mat )
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
				ob->pblock2->SetValue(PrimitiveRenderer_size, ob->ip->GetTime(), 0.0f);
				break;
			case 1:
			{					
				ob->suspendSnap = TRUE; 
				p1 = vpt->SnapPoint(m,m,NULL,SNAP_IN_PLANE);
							
				float speedFactor = 24.0f;
				float theSize = (Length(p1 - p0) / speedFactor);
				
				//Set the overall size in parameter block
				ob->pblock2->SetValue(PrimitiveRenderer_size, ob->ip->GetTime(), theSize);
								
				//Invalidate and display the mesh in the viewport
				PrimitiveRenderer_param_blk.InvalidateUI();
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

static PrimitiveRendererCreateCallBack PrimitiveRendererCreateCallback;

//From BaseObject
CreateMouseCallBack* PrimitiveRenderer::GetCreateMouseCallBack() 
{	
	PrimitiveRendererCreateCallback.SetObj(this);
	return(&PrimitiveRendererCreateCallback);
}

//From SimpleObject
void 
PrimitiveRenderer::BuildMesh(TimeValue t)
{
	// SimpleObject has a ivalid member that stores the validity of the object.
	// We need to update the validity here. The mesh is supposed to be always valid.
	// If values controlling the mesh are animated they will updated the validity interval
	// when GetValue is called.
	ivalid = FOREVER;
	float size, sizeLeft, sizeRight;
	pblock2->GetValue(PrimitiveRenderer_size, t, size, ivalid );
	pblock2->GetValue(PrimitiveRenderer_left, t, sizeLeft, ivalid );
	pblock2->GetValue(PrimitiveRenderer_right, t, sizeRight, ivalid );

	// Update mesh using these parameters with the edited content of widgetmesh.h
	mesh.setNumVerts(4);
	mesh.setNumFaces(2);
	mesh.setVert(0,size*Point3(50.0,0.0,0.0)); 
	mesh.setVert(1,size*Point3(0.0,-50.0,0.0)); 
	mesh.setVert(2,size*Point3(-50.0,0.0,0.0)); 
	mesh.setVert(3,size*Point3(0.0,50.0,0.0)); 	
	mesh.faces[0].setVerts(0, 1, 2);
	mesh.faces[0].setEdgeVisFlags(1,1,0);
	mesh.faces[0].setSmGroup(2);
	mesh.faces[1].setVerts(2, 3, 0);
	mesh.faces[1].setEdgeVisFlags(1,1,0);
	mesh.faces[1].setSmGroup(2);

	mesh.InvalidateGeomCache();
}

void 
PrimitiveRenderer::InvalidateUI() 
{
	// Hey! Update the param blocks
	pblock2->GetDesc()->InvalidateUI();
}

// From Object
int 
PrimitiveRenderer::IntersectRay(
		TimeValue t, Ray& ray, float& at, Point3& norm)
{
	//TODO: Return TRUE after you implement this method
	return FALSE;
}

void 
PrimitiveRenderer::GetCollapseTypes(Tab<Class_ID> &clist,Tab<TSTR*> &nlist)
{
    Object::GetCollapseTypes(clist, nlist);
	//TODO: Append any any other collapse type the plugin supports
}

// From ReferenceTarget
RefTargetHandle PrimitiveRenderer::Clone(RemapDir& remap) 
{
	PrimitiveRenderer* newob = new PrimitiveRenderer();	
	// Make a copy all the data and also clone all the references
	newob->ReplaceReference(0,remap.CloneRef(pblock2));
	newob->ivalid.SetEmpty();
	BaseClone(this, newob, remap);
	return(newob);
}

bool PrimitiveRenderer::PrepareDisplay(
	const MaxSDK::Graphics::UpdateDisplayContext& prepareDisplayContext)
{
	TimeValue t = prepareDisplayContext.GetDisplayTime();
	//Get UI	
	PrimitiveRenderer_DrawType drawSelection = static_cast<PrimitiveRenderer_DrawType> (pblock2->GetInt(PrimitiveRenderer_drawPrimitive, t));
	//New a DisplayCallback
	switch (drawSelection)
	{
	case Callback_DrawIndexPrimitive:
		pDemoCallback = new DrawIndexedPrimitiveCallback;
		break;
	default:
		break;
	}	
	if(NULL == pDemoCallback)
		return(false);

	UpdateMesh(t);
	//Set DisplayCallback 
	Point3 normal = Point3(0.0, 0.0, 1.0);
	pDemoCallback->ClearPositions();
	pDemoCallback->ClearNormals();
	pDemoCallback->ClearTextureCoords();
	//Set positions and normals
	for(int i = 0; i < mesh.getNumVerts(); i++)
	{
		pDemoCallback->AddPosition(mesh.getVert(i));
		pDemoCallback->AddNormal(normal);
		//Set texture coordinate
		switch(i % 4)
		{
		case 0:
			pDemoCallback->AddTextureCoord(Point3(0.0, 0.0, 0.0));
			break;
		case 1:
			pDemoCallback->AddTextureCoord(Point3(1.0, 0.0, 0.0));
			break;
		case 2:
			pDemoCallback->AddTextureCoord(Point3(0.0, 1.0, 0.0));
			break;
		case 3:
			pDemoCallback->AddTextureCoord(Point3(1.0, 1.0, 0.0));
			break;
		}
	}
	//Set vertex index
	int nFace = mesh.getNumFaces();
	pDemoCallback->SetPrimitiveType(PrimitiveTriangleList);
	for(int i = 0; i < nFace; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			pDemoCallback->AddVertexIndex(mesh.faces[i].GetVertIndex(j));
		}
	}

	//Set ImmediateRenderItemHandle
	ImmediateRenderItemHandle newHandle;
	if (!newHandle.Initialize())
	{
		return false;
	}
	IDisplayCallbackPtr IDemoCallback = (IDisplayCallbackPtr)(pDemoCallback);
	newHandle.SetDisplayCallback(IDemoCallback);
	mRenderItemHandles.ClearAllRenderItems();
	mRenderItemHandles.AddRenderItem(newHandle);
	return true;
}

bool PrimitiveRenderer::UpdatePerNodeItems(
						const MaxSDK::Graphics::UpdateDisplayContext& updateDisplayContext,
						MaxSDK::Graphics::UpdateNodeContext& nodeContext,
						MaxSDK::Graphics::IRenderItemContainer& targetRenderItemContainer)
{
	targetRenderItemContainer.AddRenderItems(mRenderItemHandles);
	return true;
}



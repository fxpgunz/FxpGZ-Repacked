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
// DESCRIPTION: Howto add realtime-rendered texture to an object.
//              This plugin define a geometry plugin that allow a user to
//              build a box that is smooth shaded of a color, with a texture
//              bitmap drawn as well.  BlockParam2 is used for color and 
//              texture path parameters.
//              mouse creation proc is copied from Dummy helper object.
// AUTHOR: Jean-Francois Yelle, Devtech M&E
//***************************************************************************/

#include "viewporttexture.h"
#include "viewporttextureclassdesc.h"
#include "viewporttexturedlgproc.h"
#include "helpers.h"
#include "resource.h"

#include "Graphics/IDisplayManager.h"
#include "Graphics/IMeshDisplay2.h"
#include "Graphics/RenderItemHandleDecorator.h"
#include "Graphics/TextureHandle.h"
#include "Graphics/RenderNodeHandle.h"

static const int MESH_VALID = 1;
static const int CREATING = 2;
static const int DISABLE_DISPLAY = 4;
static const float DD = .01f;

// Choice of making a proc static or not depend if it contains specific data.
// I chose to hide the proc in the cpp file, along with the param block descriptor.
static ViewportTextureDlgProc sg_DlgProc;

static ParamBlockDesc2 vptexture_param_blk (

	//Required arguments ----------------------------
	ViewportTexture::widget_params, GetString(IDS_PARAMS), 0, &ViewportTextureClassDesc::GetInstance(),
	//flags
	P_AUTO_CONSTRUCT + P_AUTO_UI,

	//Dependent arguments ---------------------------
	//required because P_AUTO_CONSTRUCT was flagged
	//This declares the reference number of the Parameter Block.
	ViewportTexture::pblock_ref,

	//required because P_AUTO_UI was flagged.
	//This is the Rollout description
	IDD_PANEL, IDS_PARAMS, 0, 0, &sg_DlgProc,

	//Parameter Specifications ----------------------
	// For each control create a parameter:
	ViewportTexture::widget_color,		_T("Color"),		TYPE_RGBA,		P_RESET_DEFAULT, IDS_COLOR, 
	p_default, GetUIColor(COLOR_DUMMY_OBJ),
	p_ui,        TYPE_COLORSWATCH,     IDC_COLOR,
	p_end,

	ViewportTexture::widget_texturepath,  _T("Texture path"), TYPE_FILENAME,  P_RESET_DEFAULT, IDS_TEXTUREPATH,
	p_default, _T("Enter a filename"),
	p_ui,        TYPE_EDITBOX,		IDC_TEXTUREPATH,
	p_end,

	p_end
	);

//--- ViewportTexture -------------------------------------------------------

ViewportTexture::ViewportTexture() :
	m_pblock(NULL),
	m_bitmap(NULL),	
	m_texHandle(-1),
	m_gw(NULL),
	m_flags(0),
	m_dirtyMaterial(true),
	m_dirtyTexture(true)
{
	ViewportTextureClassDesc::GetInstance().MakeAutoParamBlocks(this);
	assert(m_pblock);

	m_box.pmin = Point3(-DD,-DD,-DD);
	m_box.pmax = Point3(DD,DD,DD);
	m_flags = 0;
	m_valid.SetInfinite();	
}

ViewportTexture::~ViewportTexture()
{
	if (m_texHandle != -1)
	{
		m_gw->freeTextureHandle(m_texHandle);
		m_bitmap = NULL;
	}
	delete m_pblock;
}

void ViewportTexture::DeleteThis()
{
	delete this;
}

#define FLAGS_CHUNK 2540
#define BOX_CHUNK 2541

IOResult ViewportTexture::Load(ILoad* iload)
{
	IOResult res;

	res = GeomObject::Load(iload);
	if (res!=IO_OK) 
		return res;	

	ULONG nb;

	while (IO_OK==(res=iload->OpenChunk())) 
	{
		switch(iload->CurChunkID())  
		{
		case FLAGS_CHUNK:
			res = iload->Read(&m_flags,sizeof(long), &nb);
			break;
		case BOX_CHUNK:
			res = iload->Read(&m_box,sizeof(m_box), &nb);
			break;
		}
		res = iload->CloseChunk();	
	}

	m_dirtyMaterial = true;
	m_dirtyTexture = true;

	return IO_OK;
}

IOResult ViewportTexture::Save(ISave* isave)
{
	ULONG nb;

	GeomObject::Save(isave);

	isave->BeginChunk(FLAGS_CHUNK);
	long flags = m_flags&DISABLE_DISPLAY;
	isave->Write(&flags,sizeof(long), &nb);
	isave->EndChunk();

	isave->BeginChunk(BOX_CHUNK);
	isave->Write(&m_box,sizeof(m_box), &nb);
	isave->EndChunk();	

	return IO_OK;
}

Class_ID ViewportTexture::ClassID() 
{
	return ViewportTexture_CLASS_ID;
}		

SClass_ID ViewportTexture::SuperClassID() 
{ 
	return GEOMOBJECT_CLASS_ID; 
}

void ViewportTexture::GetClassName(TSTR& s) 
{
	s = GetString(IDS_CLASS_NAME);
}


void ViewportTexture::BeginEditParams(IObjParam *ip,ULONG flags,Animatable *prev)
{
	ViewportTextureClassDesc::GetInstance().BeginEditParams(ip, this, flags, prev);
	sg_DlgProc.SetParamBlock(m_pblock);
}

void ViewportTexture::EndEditParams( IObjParam *ip, ULONG flags,Animatable *next )
{	
	ViewportTextureClassDesc::GetInstance().EndEditParams(ip, this, flags, next);	
	sg_DlgProc.SetParamBlock(NULL);
}

//From Object
BOOL ViewportTexture::HasUVW() 
{ 
	return FALSE; 
}

void ViewportTexture::SetGenUVW(BOOL sw) 
{  
}

//Class for interactive creation of the object using the mouse
class ViewportTextureCreateCallBack : public CreateMouseCallBack {
	IPoint2 sp0;		//First point in screen coordinates
	ViewportTexture *ob;		//Pointer to the object 
	Point3 p0;			//First point in world coordinates
public:	
	int proc( ViewExp *vpt,int msg, int point, int flags, IPoint2 m, Matrix3& mat);
	void SetObj(ViewportTexture *obj) {ob = obj;}
};

Box3 ViewportTexture::GetBox() const 
{
	return m_box;
}

void ViewportTexture::SetBox(Box3& b) 
{
	m_box = b; 
	m_flags &= ~MESH_VALID;
}

int ViewportTextureCreateCallBack::proc(ViewExp *vpt,int msg, int point, int flags, IPoint2 m, Matrix3& mat )
{
	if ( ! vpt || ! vpt->IsAlive() )
	{
		// why are we here
		DbgAssert(!_T("Doing Snap() on invalid viewport!"));
		return FALSE;
	}

	float l;

	if (msg==MOUSE_POINT||msg==MOUSE_MOVE) 
	{
		switch(point) 
		{
		case 0:
			ob->SetBox(Box3(Point3(-DD,-DD,-DD), Point3(DD,DD,DD)));
			ob->m_flags |= CREATING;	// tell object we're building it so we can disable snapping to itself
			p0 = vpt->SnapPoint(m,m,NULL,SNAP_IN_3D);
			mat.SetTrans(p0);
			sp0 = m;
			ob->m_flags &= ~MESH_VALID;
			break;

		case 1:
			l =  (float)fabs(vpt->SnapLength(vpt->GetCPDisp(p0,Point3(0,0,1),sp0,m)));
			ob->m_box.pmin = Point3(-l,-l,-l);
			ob->m_box.pmax = Point3( l, l, l);
			ob->m_flags &= ~MESH_VALID;
			if (msg==MOUSE_POINT) 
			{
				ob->m_flags &= ~CREATING;	
				if (Length(m-sp0) < 4) return CREATE_ABORT;
				else return CREATE_STOP;
			}
			break;
		}
	}
	else if (msg == MOUSE_ABORT) 
	{
		ob->m_flags &= ~CREATING;	
		return CREATE_ABORT;
	}

	return TRUE;
}

static ViewportTextureCreateCallBack ViewportTextureCreateCB;

//From BaseObject
CreateMouseCallBack* ViewportTexture::GetCreateMouseCallBack() 
{
	ViewportTextureCreateCB.SetObj(this);
	return(&ViewportTextureCreateCB);
}

int ViewportTexture::Display(TimeValue t, INode* inode, ViewExp* vpt, int flags)
{
	if ( ! vpt || ! vpt->IsAlive() )
	{
		// why are we here
		DbgAssert(!_T("Doing Snap() on invalid viewport!"));
		return FALSE;
	}


	if (MaxSDK::Graphics::IsRetainedModeEnabled())
	{
		return 0;
	}

	if (m_flags&DISABLE_DISPLAY)
		return 0;

	if (m_dirtyMaterial)
		SetMaterial();
	if (m_dirtyTexture)
		LoadTexture();

	//first pass through need to build our texture handle
	if( m_texHandle == -1 ) 
	{
		GraphicsWindow *gw = vpt->getGW();
		BITMAPINFO *bi = BitmapToDIB(m_bitmap);
		m_texHandle = gw->getTextureHandle(bi);
		m_gw = gw;
	}

	GraphicsWindow *gw = vpt->getGW();
	Matrix3 m = inode->GetObjectTM(t);
	gw->setTransform(m);

	DWORD rlim = gw->getRndLimits();

	PlugTextureInfo();

	gw->setMaterial(m_mat);
	gw->setRndLimits(gw->getRndLimits()|GW_TEXTURE |GW_Z_BUFFER);

	//m_mesh test
	UpdateMesh();
	m_mesh.render(gw, &m_mat, NULL, COMP_ALL);

	gw->setRndLimits(rlim);
	return(0);
}

// from IDisplay2
unsigned long ViewportTexture::GetObjectDisplayRequirement() const
{
	return 0;
}

bool ViewportTexture::PrepareDisplay(const MaxSDK::Graphics::UpdateDisplayContext& prepareDisplayContext)
{
	mRenderItemHandles.ClearAllRenderItems();
	// create a mesh to display (leave it in cache)
	using namespace MaxSDK::Graphics;		
	UpdateMesh();
	GenerateMaterialHandle(m_customMaterialHandle);
	if(GenerateTextureHandle(m_customTextureHandle))
	{
		m_customMaterialHandle.SetDiffuseTexture(m_customTextureHandle);
	}

	IMeshDisplay2* pMeshDisplay = 
		static_cast<IMeshDisplay2*>(m_mesh.GetInterface(IMesh_DISPLAY2_INTERFACE_ID));
	if (NULL == pMeshDisplay)
	{
		return false;
	}
	GenerateMeshRenderItemsContext generateRenderItemsContext;
	generateRenderItemsContext.GenerateDefaultMeshElementDescriptions(
		prepareDisplayContext.GetRequiredComponents());
	generateRenderItemsContext.AddRequiredStreams(*m_customMaterialHandle.GetRequiredStreams());
	pMeshDisplay->PrepareDisplay(generateRenderItemsContext);
	return true;
}

bool ViewportTexture::GenerateMaterialHandle
	(MaxSDK::Graphics::StandardMaterialHandle& materialHandle)
{
	using namespace MaxSDK::Graphics;
	if(!materialHandle.Initialize())
	{
		return false;
	}

	materialHandle.SetTwoSided(false);
	Color color;
	m_pblock->GetValue(widget_color, GetCOREInterface()->GetTime(), color, m_valid);
	//diffuse, ambient and specular
	materialHandle.SetAmbient(AColor(color.r,color.g,color.b));
	materialHandle.SetDiffuse(AColor(color.r,color.g,color.b));
	materialHandle.SetSpecular(AColor(1.0f,1.0f,1.0f));
	// Set opacity
	materialHandle.SetOpacity(1.0f);
	m_dirtyMaterial = false;
	return true;
}

bool ViewportTexture::GenerateTextureHandle(MaxSDK::Graphics::TextureHandle& textureHandle)
{
	using namespace MaxSDK::Graphics;
	
	const TCHAR* lFilenamePtr = NULL;
	m_pblock->GetValue(widget_texturepath, GetCOREInterface()->GetTime(), lFilenamePtr, m_valid);
	
	if (_tcscmp(m_lastBitmapFilename, lFilenamePtr)!=0)  
	{
		m_dirtyTexture = true;
	}
	
	if(!m_dirtyTexture)
	{
		return false;
	}
	
	m_dirtyTexture = false;
	textureHandle.Release();
	if(!textureHandle.Initialize(lFilenamePtr))
	{
		return false;
	}
	m_lastBitmapFilename = lFilenamePtr;
	return true;
}

bool ViewportTexture::UpdatePerNodeItems(const MaxSDK::Graphics::UpdateDisplayContext& updateDisplayContext,
										 MaxSDK::Graphics::UpdateNodeContext& nodeContext,
										 MaxSDK::Graphics::IRenderItemContainer& targetRenderItemContainer)
{
	using namespace MaxSDK::Graphics;

	AutoRestoreRenderNodeMaterial autoRestore(nodeContext.GetRenderNode(), m_customMaterialHandle);
	GenerateMeshRenderItemsContext generateRenderItemsContext;

	generateRenderItemsContext.GenerateDefaultContext(updateDisplayContext);
	generateRenderItemsContext.RemoveInvisibleMeshElementDescriptions(nodeContext.GetRenderNode());
	IMeshDisplay2* pMeshDisplay = static_cast<IMeshDisplay2*>(m_mesh.GetInterface(IMesh_DISPLAY2_INTERFACE_ID));
	if (NULL == pMeshDisplay)
	{
		return false;
	}	
	RenderItemHandleArray renderItems;
	pMeshDisplay->GetRenderItems(generateRenderItemsContext,nodeContext,renderItems);

	for(size_t i = 0;  i < renderItems.GetNumberOfRenderItems(); ++ i)
	{
		MeshElementDescription meshDesc(renderItems.GetRenderItem(i).GetDescriptionBits());
		if(meshDesc.GetMeshElementType() != MeshElementTypeEdge)
		{
			RenderItemHandleDecorator decorator;
			decorator.Initialize(renderItems.GetRenderItem(i));
			decorator.SetCustomMaterial(m_customMaterialHandle);
			targetRenderItemContainer.AddRenderItem(decorator);
		}
		else
		{
			targetRenderItemContainer.AddRenderItem(renderItems.GetRenderItem(i));
		}
	}

	return true;
}


void ViewportTexture::UpdateMesh() 
{
	if (!(m_flags&MESH_VALID)) 
	{
		BuildMesh();
		m_flags |= MESH_VALID;
	}
}

void ViewportTexture::BuildMesh()
{
	int size = 20;

	int nverts = 8;
	int nfaces = 12;
	Point3 va = m_box.pmin;
	Point3 vb = m_box.pmax;
	if(m_box.IsEmpty()) {
		va = Point3(-DD, -DD, -DD);
		vb = Point3( DD,  DD,  DD);
	}	
	m_mesh.setNumVerts(nverts);
	m_mesh.setNumFaces(nfaces);

	m_mesh.setVert(0, Point3( va.x, va.y, va.z));
	m_mesh.setVert(1, Point3( vb.x, va.y, va.z));
	m_mesh.setVert(2, Point3( va.x, vb.y, va.z));
	m_mesh.setVert(3, Point3( vb.x, vb.y, va.z));
	m_mesh.setVert(4, Point3( va.x, va.y, vb.z));
	m_mesh.setVert(5, Point3( vb.x, va.y, vb.z));
	m_mesh.setVert(6, Point3( va.x, vb.y, vb.z));
	m_mesh.setVert(7, Point3( vb.x, vb.y, vb.z));

	MakeQuad(&(m_mesh.faces[ 0]), 0,2,3,1,  1);
	MakeQuad(&(m_mesh.faces[ 2]), 2,0,4,6,  2);
	MakeQuad(&(m_mesh.faces[ 4]), 3,2,6,7,  4);
	MakeQuad(&(m_mesh.faces[ 6]), 1,3,7,5,  8);
	MakeQuad(&(m_mesh.faces[ 8]), 0,1,5,4, 16);
	MakeQuad(&(m_mesh.faces[10]), 4,5,7,6, 32);
	m_mesh.InvalidateGeomCache();
	m_mesh.EnableEdgeList(1);

	// Apply mapping
	Matrix3 m(1);
	float fsize = size *0.5f;
	m.Scale(Point3(1.0f/fsize,1.0f/fsize,1.0f/fsize));
	m_mesh.ApplyUVWMap( MAP_BOX, 1.0f, 1.0f, 1.0f, 0, 0, 0, 0, m );
}

void ViewportTexture::SetMaterial()
{
	assert(m_dirtyMaterial);

	//Set Material properties
	m_mat.dblSided = 0;
	//diffuse, ambient and specular

	Color color;
	m_pblock->GetValue(widget_color, GetCOREInterface()->GetTime(), color, m_valid);

	m_mat.Ka =  Point3(color.r, color.g, color.b);
	m_mat.Kd = Point3(color.r, color.g, color.b);
	m_mat.Ks = Point3(1.0f,1.0f,1.0f);
	// Set opacity
	m_mat.opacity = 1.0f;
	//Set shininess
	m_mat.shininess = 0.0f;
	m_mat.shinStrength = 0.0f;

	m_dirtyMaterial = false;
}

#pragma message ("*** AssetManager TODO: Look at how ViewportTexture handles asset files here")

void ViewportTexture::LoadTexture()
{
	assert(m_dirtyTexture);

	// Load up the texture from disk
	const TCHAR* lFilenamePtr = NULL;
	m_pblock->GetValue(widget_texturepath, GetCOREInterface()->GetTime(), lFilenamePtr, m_valid);
	if (_tcscmp(m_lastBitmapFilename, lFilenamePtr)!=0)
	{
		if (m_texHandle != -1)
		{
			m_gw->freeTextureHandle(m_texHandle);
			m_bitmap = NULL;
		}
		BitmapInfo bi;

		bi.SetName(lFilenamePtr); 
		BMMRES status;
		m_bitmap = TheManager->Load(&bi, &status);
		m_texHandle = -1;	
	}	
	m_lastBitmapFilename = lFilenamePtr;

	m_dirtyTexture = false;
}


int ViewportTexture::HitTest(TimeValue t, INode* inode, int type, int crossing, 
							 int flags, IPoint2* p, ViewExp* vpt)
{
	if (MaxSDK::Graphics::IsHardwareHitTesting(vpt))
	{
		return 0;
	}

	if ( ! vpt || ! vpt->IsAlive() )
	{
		// why are we here
		DbgAssert(!_T("Doing Snap() on invalid viewport!"));
		return FALSE;
	}

	HitRegion hitRegion;
	Matrix3 m;
	if (m_flags&DISABLE_DISPLAY)
		return 0;
	GraphicsWindow *gw = vpt->getGW();
	DWORD rlim = gw->getRndLimits();
	gw->setRndLimits(GW_WIREFRAME|GW_EDGES_ONLY|GW_BACKCULL);
	Material *mtl = gw->getMaterial();
	MakeHitRegion(hitRegion,type,crossing,4,p);	
	m = inode->GetObjectTM(t);
	gw->setTransform(m);
	UpdateMesh();
	int res =m_mesh.select( gw,mtl, &hitRegion, flags & HIT_ABORTONHIT );
	gw->setRndLimits(rlim);
	return res;
}

void ViewportTexture::Snap(TimeValue t, INode* inode, SnapInfo* snap, IPoint2* p, ViewExp* vpt)
{
	if ( ! vpt || ! vpt->IsAlive() )
	{
		// why are we here
		DbgAssert(!_T("Doing Snap() on invalid viewport!"));
		return;
	}

	if (m_flags&CREATING)	// If creating this one, don't try to snap to it!
		return;

	Matrix3 tm = inode->GetObjectTM(t);	
	GraphicsWindow *gw = vpt->getGW();	
	gw->setTransform(tm);

	UpdateMesh();
	m_mesh.snap( gw, snap, p, tm );
}

const TCHAR* ViewportTexture::GetObjectName() 
{ 
	return GetString(IDS_CLASS_NAME); 
}

void ViewportTexture::GetWorldBoundBox(TimeValue t, INode* inode, ViewExp* /*vpt*/, Box3& m_box )
{
	if (m_flags&DISABLE_DISPLAY)
		return;
	Matrix3 matrix = inode->GetObjectTM(t);
	UpdateMesh();
	m_box = m_mesh.getBoundingBox();
	m_box = m_box * matrix;
}

void ViewportTexture::GetLocalBoundBox(TimeValue t, INode *mat, ViewExp * /*vpt*/, Box3& m_box )
{
	UpdateMesh();
	m_box = m_mesh.getBoundingBox();
}

void ViewportTexture::GetDeformBBox(TimeValue t, Box3& m_box, Matrix3* tm, BOOL useSel )
{
	UpdateMesh();
	m_box = m_mesh.getBoundingBox(tm);
}

void ViewportTexture::InitNodeName(TSTR& s) 
{ 
	s = GetString(IDS_CLASS_NAME); 
}

//From ReferenceMaker
RefResult ViewportTexture::NotifyRefChanged( const Interval& changeInt, RefTargetHandle hTarget, 
											PartID& partID, RefMessage message, BOOL propagate )
{
	m_dirtyMaterial = true;
	m_dirtyTexture = true;
	return(REF_SUCCEED);
}

int ViewportTexture::NumSubs() 
{ 
	return 1; 
}

TSTR ViewportTexture::SubAnimName(int i) 
{ 
	return GetString(IDS_PARAMS); 
}				

Animatable* ViewportTexture::SubAnim(int i) 
{ 
	return m_pblock; 
}

int ViewportTexture::NumRefs() 
{ 
	return 1; 
}

RefTargetHandle ViewportTexture::GetReference(int i) 
{ 
	return m_pblock; 
}

void ViewportTexture::SetReference(int i, RefTargetHandle rtarg) 
{ 
	m_pblock=(IParamBlock2*)rtarg; 
}

int	ViewportTexture::NumParamBlocks() 
{ 
	return pblock_count; 
}

IParamBlock2* ViewportTexture::GetParamBlock(int i) 
{ 
	return m_pblock; 
}

IParamBlock2* ViewportTexture::GetParamBlockByID(BlockID id) 
{ 
	return (m_pblock->ID() == id) ? m_pblock : NULL; 
}

Mesh* ViewportTexture::GetRenderMesh(TimeValue t, 
									 INode *inode, View& view, BOOL& needDelete)
{
	// returning NULL make this object not renderable (ie: through scanline or mental ray for instance)
	// otherwise, just return m_mesh.
	// A different render version can also be available. In this case, 2 mesh would be kept.  
	// I suppose reporting the change made to the display mesh into the render mesh and vice-versa would
	// be required.
	return NULL;
}


Object* ViewportTexture::ConvertToType(TimeValue t, Class_ID obtype)
{
	return NULL;
}

int ViewportTexture::CanConvertToType(Class_ID obtype)
{
	return 0;
}

// From Object
int ViewportTexture::IntersectRay(TimeValue t, Ray& ray, float& at, Point3& norm)
{
	return FALSE;
}

ObjectState ViewportTexture::Eval(TimeValue t) 
{ 
	return ObjectState(this); 
}

Interval ViewportTexture::ObjectValidity(TimeValue t)
{ 
	return FOREVER; 
}

void ViewportTexture::GetCollapseTypes(Tab<Class_ID> &clist,Tab<TSTR*> &nlist)
{
	Object::GetCollapseTypes(clist, nlist);
}

// From ReferenceTarget
RefTargetHandle ViewportTexture::Clone(RemapDir& remap) 
{
	ViewportTexture* newob = new ViewportTexture();	
	newob->ReplaceReference(0,remap.CloneRef(m_pblock));
	BaseClone(this, newob, remap);
	return(newob);
}

void ViewportTexture::PlugTextureInfo()
{

	m_mat.texture.setLengthUsed(1);
	//set our textures 
	TextureInfo &ti = m_mat.texture[0];
	ti.useTex = 1;
	ti.textHandle = m_texHandle;

	//set up the mapping and tiling info
	ti.faceMap = 0;
	ti.uvwSource = UVSOURCE_MESH; 
	ti.mapChannel = 1;

	ti.textTM = Matrix3(1);
	ti.useTex = 1;
	ti.uvwSource = UVSOURCE_MESH; 
	ti.mapChannel = 1;
	ti.tiling[0] = GW_TEX_NO_TILING;
	ti.tiling[1] = GW_TEX_NO_TILING;
	ti.tiling[2] = GW_TEX_NO_TILING;

	BOOL needDecal = (ti.tiling[0]==GW_TEX_NO_TILING || ti.tiling[1]==GW_TEX_NO_TILING);
	ti.alphaOp = GW_TEX_LEAVE;
	if (needDecal) 
	{
		ti.colorOp = GW_TEX_ALPHA_BLEND;
	}
	else {
		ti.colorOp = GW_TEX_MODULATE;
	}

}

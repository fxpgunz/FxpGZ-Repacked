/**********************************************************************
 *<
	FILE: GPUParticle.cpp

	DESCRIPTION:	This plugin shows how to:
					
					Use ICustomRenderItem to render markers

					This Visual Studio project accompanies an article
					in the 3ds Max SDK Help file that walks a developer, 
					who is new to the 3ds Max SDK, through the process of
					writing a basic procedural object.

	OVERVIEW:		The GPUParticle is a simple sample that show how to 
	                use stream-out feature to do GPGPU task using new API.
					It is located in the create command panel under the 
					category "Max SDK tutorials". 

					This sample only work with DX11 level GPU.
															
					A GPUParticle has two parameters:

					Radius: the particle disk size

				    Count: Number of particles in the disk.

					This project was created using the Visual Studio Application
					wizard for 3ds Max. Hence many TODO comments are still in the
					body of the plugin.

					The most important class that were implemented were:
					class ParticleRenderer : public MaxSDK::Graphics::ICustomRenderItem

					The most important functions that were implemented were:
					void ParticleRenderer::Realize(TimeValue t)
					which is used to calculate particle motion in the GPU

					bool ParticleRenderer::Display(...)
					which used to render markers

					The parameter block implemented was:

					static ParamBlockDesc2 ParticleRenderer_param_blk(... ...)
					
	CREATED BY:		Qinming Ren
		
	HISTORY:		Code started in January 2013

 *>	Copyright (c) 2013, All Rights Reserved.
 **********************************************************************/

#include "GPUParticle.h"
#include <Graphics/ICustomRenderItem.h>
#include <Graphics/CustomRenderItemHandle.h>
#include <Graphics/HLSLMaterialHandle.h>
#include <Graphics/IObjectDisplay2.h>
#include <Graphics/VertexBufferHandle.h>
#include <Graphics/IndexBufferHandle.h>
#include <Graphics/IVirtualDevice.h>
#include <Graphics/IDisplayManager.h>
#include <vector>

#define GPUPARTICLE_CLASS_ID	Class_ID(0x1210876e, 0x1bc44950)

#define PBLOCK_REF	0
#define COLOR_ARGB(a,r,g,b) ((DWORD)((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)))
#define PARTICLE_COUNT 100000

using namespace MaxSDK::Graphics;

//A utility function to load shaders from resource
bool LoadShaderData(CStr& outData, int iShaderID, HINSTANCE hInst, const MSTR& strTypeName)
{
	bool bResult = false;

	// Load from resource
	HRSRC hResourceInfo;
	HGLOBAL hResource;
	DWORD dwSize;
	// find the resource info
	hResourceInfo = FindResourceW(hInst, MAKEINTRESOURCEW(iShaderID), strTypeName);
	if (hResourceInfo != NULL)
	{
		// handle to the resource
		hResource = LoadResource(hInst, hResourceInfo);
		if (hResource != NULL)
		{
			byte* pStr = (byte*)LockResource(hResource);
			dwSize = SizeofResource(hInst, hResourceInfo);
			if (pStr != NULL && dwSize != 0)
			{
				outData.Resize(dwSize + 1); //Reserve '\0' at the end of string
				ZeroMemory(outData.dataForWrite(), outData.length());
				memcpy(outData.dataForWrite(), pStr, dwSize);
				outData.dataForWrite()[dwSize] = '\0';
				UnlockResource(hResource);
				bResult = true;
			}
		}
	}

	return bResult;
}

//This is a utility class to wrap marker effect
class MarkerEffect
{
private:
	HLSLMaterialHandle mMarkerShader;
	MaterialRequiredStreams mMarkerFormat;

	DepthStencilState mDepthState;
	BlendState mBlendState;

	DepthStencilState mDepthStateBackup;
	BlendState mBlendStateBackup;	
public:
	MarkerEffect()
	{
		CStr sMarkerShader;
		MaxSDK::Graphics::DeviceCaps caps;
		GetIDisplayManager()->GetDeviceCaps(caps);
		if (Level5_0 == caps.FeatureLevel)
		{
			LoadShaderData(sMarkerShader, IDR_MARKERSHADER, MaxSDK::GetHInstance(),  L"SHADER");
			mMarkerShader.InitializeWithString(sMarkerShader);
		}

		/* Marker shader needs format layout like this:
		Stream 0: base geometry, we use a 4-vertex quad here.
		Stream 1: Color(float4) + Position(float3)
		Stream 1 is the instance data.
		We need to create the stream format manually.
		*/
		MaterialRequiredStreamElement positionChannel;
		positionChannel.SetType(VertexFieldFloat2);
		positionChannel.SetChannelCategory(MeshChannelPosition);
		positionChannel.SetUsageIndex(0);
		mMarkerFormat.AddStream(positionChannel);


		MaterialRequiredStreamElement instanceColorField;
		instanceColorField.SetType(VertexFieldFloat4);
		instanceColorField.SetChannelCategory(MeshChannelVertexColor);
		instanceColorField.SetUsageIndex(0);
		//Set flag to true to indicate this is a instance stream.
		instanceColorField.SetIsInstanceStream(true);
		instanceColorField.SetStreamIndex(1);
		mMarkerFormat.AddStream(instanceColorField);

		MaterialRequiredStreamElement instancePositionField;
		instancePositionField.SetType(VertexFieldFloat3);
		instancePositionField.SetChannelCategory(MeshChannelTexcoord);
		instancePositionField.SetUsageIndex(0);
		//Note position stream index is same as color stream, but the offset is different.
		//Currently only last stream in a vertex buffer array can be used as instance data
		instancePositionField.SetStreamIndex(1);
		instancePositionField.SetOffset(sizeof(Point4));
		//Set flag to true to indicate this is a instance stream.
		instancePositionField.SetIsInstanceStream(true);
		mMarkerFormat.AddStream(instancePositionField);

		//Prepare some render states
		mBlendState.GetTargetBlendState(0).SetBlendEnabled(true);
		mBlendState.GetTargetBlendState(0).SetDestinationBlend(BlendSelectorOne);

		mDepthState.SetDepthWriteEnabled(false);

	}

	~MarkerEffect()
	{

	}
	
	void Activate(DrawContext& drawContext)
	{
		//Apply the effect and parameters to the device. 
		IVirtualDevice& vd = drawContext.GetVirtualDevice();
		mBlendStateBackup = vd.GetBlendState();
		mDepthStateBackup = vd.GetDepthStencilState();
		vd.SetDepthStencilState(mDepthState);
		vd.SetBlendState(mBlendState);
		mMarkerShader.Activate(drawContext);
		mMarkerShader.ActivatePass(drawContext, 0);
	}

	void Terminate(DrawContext& drawContext)
	{
		//Restore previous states
		IVirtualDevice& vd = drawContext.GetVirtualDevice();
		vd.SetDepthStencilState(mDepthStateBackup);
		vd.SetBlendState(mBlendStateBackup);
		mMarkerShader.PassesFinished(drawContext);
		mMarkerShader.Terminate();
	}

	MaterialRequiredStreams& GetStreamFormat()
	{
		return mMarkerFormat;
	}

	const HLSLMaterialHandle& GetEffect()
	{
		return mMarkerShader;
	}

};

#define INIT_FRAME_STAMP -1

class ParticleRenderer : public MaxSDK::Graphics::ICustomRenderItem
{
	MarkerEffect mMarkerEffect;
	HLSLMaterialHandle mMotionShader;
	VertexBufferHandle mQuad;
	IndexBufferHandle mQuadIdx;
	VertexBufferHandle mPositionStream;
	VertexBufferHandle mColorStream;

	VertexBufferHandle mStreamOutData;

	VertexBufferHandleArray mRawDataBuffers;
	SmartHandleArray mStreamOutBuffers;
	VertexBufferHandleArray mParticleBuffers;

	MaterialRequiredStreams mMotionFormat;
	DepthStencilState mDepthState;

	float mRadius;
	size_t mCount;
	int mRealizedFrame;
public:
	ParticleRenderer()
	{
		mRealizedFrame = INIT_FRAME_STAMP;
		mRadius = 100.0f;
		mCount = 1;
		CStr sMotionShader;
		MaxSDK::Graphics::DeviceCaps caps;
		GetIDisplayManager()->GetDeviceCaps(caps);
		if (Level5_0 == caps.FeatureLevel)
		{
			LoadShaderData(sMotionShader, IDR_MOTIONSHADER, MaxSDK::GetHInstance(),  L"SHADER");
			mMotionShader.InitializeWithString(sMotionShader);
		}

		/* Motion shader only calculate particle's position. Color is simply bypassed becase
		we have a limitation that only last stream can be instance data.
		So inputs of the motion shader is position and color in 2 streams, and the shader
		outputs 1 stream with composite of color and position.
		*/
		MaterialRequiredStreamElement positionField;
		positionField.SetType(VertexFieldFloat3);
		positionField.SetChannelCategory(MeshChannelPosition);
		positionField.SetStreamIndex(0);
		mMotionFormat.AddStream(positionField);

		MaterialRequiredStreamElement colorField;
		colorField.SetType(VertexFieldColor);
		colorField.SetChannelCategory(MeshChannelVertexColor);
		colorField.SetStreamIndex(1);
		mMotionFormat.AddStream(colorField);

		//This is a quad as instance base geometry
		mQuad.Initialize(sizeof(Point2), 4);
		Point2* pBuffer = (Point2*)(mQuad.Lock(0, 0));
		pBuffer[0] = Point2(-1, 1);
		pBuffer[1] = Point2(1, 1);
		pBuffer[2] = Point2(-1, -1);
		pBuffer[3] = Point2(1, -1);
		mQuad.Unlock();

		mQuadIdx.Initialize(IndexTypeShort, 6);
		SHORT* pIdx = (SHORT*)mQuadIdx.Lock(0, 0, WriteAcess);
		pIdx[0] = 0;
		pIdx[1] = 1;
		pIdx[2] = 2;
		pIdx[3] = 1;
		pIdx[4] = 2;
		pIdx[5] = 3;
		mQuadIdx.Unlock();

		//Prepare render state for stream out.
		//Motion shader doesn't needs pixel shader, so we also need to turn off
		//Z buffer to improve performance.
		mDepthState.SetDepthEnabled(false);
	}

	~ParticleRenderer()
	{
	}

	virtual size_t GetPrimitiveCount() const
	{
		return mCount;
	}

	float Radius()
	{
		return mRadius;
	}

	void Radius(float value)
	{
		if (mRadius != value)
		{
			mRadius = value;
			mRealizedFrame = INIT_FRAME_STAMP;
		}
	}

	size_t Count()
	{
		return mCount;
	}

	void Count(size_t value)
	{
		if (mCount != value)
		{
			mCount = value;
			mRealizedFrame = INIT_FRAME_STAMP;
		}
	}

	virtual void Realize(DrawContext& drawContext)
	{
		if (mRealizedFrame == drawContext.GetFrameStamp()
			|| 0 == mCount)
		{
			return;
		}
		else if (INIT_FRAME_STAMP == mRealizedFrame)
		{
			mPositionStream.Release();
			mColorStream.Release();
			mRawDataBuffers.removeAll();
			mStreamOutBuffers.removeAll();
			mParticleBuffers.removeAll();
			mPositionStream.Initialize(sizeof(Point3), mCount);
			float* pPosBuffer = (float*)(mPositionStream.Lock(0, 0));
			for (int i = 0; i < mCount; ++i)
			{
				pPosBuffer[0] = (float)rand() / RAND_MAX * mRadius; //radius
				pPosBuffer[1] = (float)rand() / RAND_MAX * 6.28f; //init rad
				pPosBuffer[2] = pPosBuffer[0] / mRadius * (3.1415926f / 8.0f) ; //rad speed
				pPosBuffer += 3;
			}
			mPositionStream.Unlock();

			mColorStream.Initialize(sizeof(DWORD), mCount);
			DWORD* dwColor = (DWORD*)mColorStream.Lock(0, 0);
			for (int i = 0; i < mCount; ++i)
			{
				int r = (float)rand() / RAND_MAX * 255;
				int g = (float)rand() / RAND_MAX * 255;
				int b = (float)rand() / RAND_MAX * 255;
				*dwColor = COLOR_ARGB(255, r, g, b);
				++dwColor;
			}
			mColorStream.Unlock();

			mStreamOutData.Initialize(sizeof(Point4) + sizeof(Point3), mCount, nullptr, (BufferUsageType)(BufferUsageStatic | BufferUsageTarget));

			mRawDataBuffers.append(mPositionStream);
			mRawDataBuffers.append(mColorStream);

			mStreamOutBuffers.append(mStreamOutData);

			mParticleBuffers.append(mQuad);
			mParticleBuffers.append(mStreamOutData);
		}
		mRealizedFrame = drawContext.GetFrameStamp();
		IVirtualDevice& vd = drawContext.GetVirtualDevice();
		DepthStencilState depthBack = vd.GetDepthStencilState();

		vd.SetDepthStencilState(mDepthState);
		
		mMotionShader.Activate(drawContext);
		mMotionShader.ActivatePass(drawContext, 0);
		vd.SetStreamFormat(mMotionFormat);
		vd.SetIndexBuffer(EmptyIndexBufferHandle);
		vd.SetVertexStreams(mRawDataBuffers);
		vd.SetOutputStreams(mStreamOutBuffers);
		vd.Draw(PrimitivePointList, 0, (int)mCount);
		vd.SetOutputStreams(EmptySmartHandleArray);
		mMotionShader.PassesFinished(drawContext);
		mMotionShader.Terminate();
		vd.SetDepthStencilState(depthBack);
	}

	virtual void Display(DrawContext& drawContext)
	{
		IVirtualDevice& vd = drawContext.GetVirtualDevice();

		vd.SetStreamFormat(mMarkerEffect.GetStreamFormat());
		vd.SetVertexStreams(mParticleBuffers);
		vd.SetIndexBuffer(mQuadIdx);

		mMarkerEffect.Activate(drawContext);
		vd.DrawInstanced(PrimitiveTriangleList, (int)mCount, 0, 2);
		mMarkerEffect.Terminate(drawContext);
	}
};

typedef RefPtr<ParticleRenderer> ParticleRendererPtr;

class GPUParticle : public SimpleObject2, public MaxSDK::Graphics::IObjectDisplay2{
	friend class GPUParticleCreateCallBack;
	public:
		// Parameter block handled by parent

		//Class vars
		static IObjParam *ip;			//Access to the interface
		// From BaseObject
		CreateMouseCallBack* GetCreateMouseCallBack();

		BaseInterface* GetInterface(Interface_ID id);
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
		void GetWorldBoundBox(TimeValue t, INode* inode, ViewExp* vpt, Box3& box );
		void GetLocalBoundBox(TimeValue t, INode* inode, ViewExp* vpt, Box3& box );
		void GetDeformBBox(TimeValue t, Box3& box, Matrix3 *tm, BOOL useSel );

		int HitTest(TimeValue t, INode *inode, int type, int crossing, int flags, IPoint2 *p, ViewExp *vpt) ;
		
		//From Animatable
		Class_ID ClassID() {return GPUPARTICLE_CLASS_ID;}		
		SClass_ID SuperClassID() { return GEOMOBJECT_CLASS_ID; }
		void GetClassName(TSTR& s) {s = GetString(IDS_CLASS_NAME);}

		// display functions from BaseObject
		//virtual int HitTest(TimeValue t, INode *inode, int type, int crossing, int flags, IPoint2 *p, ViewExp *vpt);
		virtual bool RequiresSupportForLegacyDisplayMode() const {return false;}

		RefTargetHandle Clone( RemapDir &remap );

		//This returns a name that will be used to automatically name the object when it gets created
		const TCHAR *GetObjectName() { return GetString(IDS_CLASS_NAME); }

		void DeleteThis() { delete this; }		
		
		//Constructor/Destructor
		GPUParticle();
		~GPUParticle();
	protected:		

		virtual bool PrepareDisplay(
			const UpdateDisplayContext& prepareDisplayContext);

		virtual bool UpdatePerNodeItems(
			const UpdateDisplayContext& updateDisplayContext,
			UpdateNodeContext& nodeContext,
			IRenderItemContainer& targetRenderItemContainer);

		ParticleRendererPtr mpParticleItem;

		float mRadius;
		int mCount;
};



class GPUParticleClassDesc : public ClassDesc2 {
	public:
	int 			IsPublic() { return TRUE; }
	void *			Create(BOOL loading = FALSE) { return new GPUParticle(); }
	const TCHAR *	ClassName() { return GetString(IDS_CLASS_NAME); }
	SClass_ID		SuperClassID() { return GEOMOBJECT_CLASS_ID; }
	Class_ID		ClassID() { return GPUPARTICLE_CLASS_ID; }
	const TCHAR* 	Category() { return GetString(IDS_CATEGORY); }

	const TCHAR*	InternalName() { return _T("GPUParticle"); }	// returns fixed parsable name (scripter-visible name)
	HINSTANCE		HInstance() { return hInstance; }					// returns owning module handle
	

};

static GPUParticleClassDesc GPUParticleDesc;
ClassDesc2* GetGPUParticleClassDesc() { return &GPUParticleDesc; }

//TODO: Add enumerations for each parameter block
enum { GPUParticle_params };


//TODO: Add enumerations for various parameters
enum { 	
	GPUParticle_radius,
	GPUParticle_count,
//	GPUParticle_receivelight	
};


static ParamBlockDesc2 GPUParticle_param_blk ( 
	//Required arguments ----------------------------
	GPUParticle_params, _T("params"),  0, &GPUParticleDesc,
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
	GPUParticle_radius, 		_T("Radius"), 		TYPE_FLOAT, 	P_ANIMATABLE, 	IDS_SPIN, 
		//Zero or more optional tags
		p_default, 		1.0f, 
		p_range, 		0.0f,10000.0f, 
		p_ui, 			TYPE_SPINNER,		EDITTYPE_FLOAT, IDC_SIZE_EDIT,	IDC_SIZE_SPIN, 10.0f, 
		p_end,
	GPUParticle_count,		_T("ParticleCount"), 		TYPE_INT, 	P_ANIMATABLE, 	IDS_SPIN, 
		//Zero or more optional tags	
		p_default, 		10000, 
		p_range, 		0,10000000, 
		p_ui, 			TYPE_SPINNER,		EDITTYPE_UNIVERSE, IDC_LEFT_EDIT,	IDC_LEFT_SPIN, 100, 
		p_end,
// 	GPUParticle_receivelight, 		_T("ReceiveLight"), 		TYPE_BOOL, 	0, 	IDS_NORMAL, 
// 		p_default, 		FALSE, 
// 		p_ui, 			TYPE_SINGLECHECKBOX,	IDC_NORMAL,	
// 		p_end,
	p_end
	);


IObjParam *GPUParticle::ip			= NULL;

//--- GPUParticle -------------------------------------------------------

GPUParticle::GPUParticle()
{
	GPUParticleDesc.MakeAutoParamBlocks(this);	
	mpParticleItem = nullptr;
	mRadius = 1.0f;
	mCount = 10000;
}

GPUParticle::~GPUParticle()
{
}

void 
GPUParticle::BeginEditParams(IObjParam *ip,ULONG flags,Animatable *prev)
{
	this->ip = ip;

	SimpleObject2::BeginEditParams(ip,flags,prev);
	GPUParticleDesc.BeginEditParams(ip, this, flags, prev);
}

void 
GPUParticle::EndEditParams( IObjParam *ip, ULONG flags, Animatable *next )
{
	//TODO: Save plugin parameter values into class variables, if they are not hosted in ParamBlocks. 
	
	SimpleObject2::EndEditParams(ip,flags,next);
	GPUParticleDesc.EndEditParams(ip, this, flags, next);

	this->ip = NULL;
}

//From Object
BOOL 
GPUParticle::HasUVW() 
{ 
	//TODO: Return whether the object has UVW coordinates or not
	return TRUE; 
}

void 
GPUParticle::SetGenUVW(BOOL sw) 
{  
	if (sw==HasUVW()) return;
	//TODO: Set the plugin's internal value to sw				
}

//Class for interactive creation of the object using the mouse
class GPUParticleCreateCallBack : public CreateMouseCallBack {
	IPoint2 sp0;	//First point in screen coordinates
	GPUParticle *ob;		//Pointer to the object 
	Point3 p0;		//First point in world coordinates
	Point3 p1;		//We added this point. Second point in world coordinates.
public:	
	int proc( ViewExp *vpt,int msg, int point, int flags, IPoint2 m, Matrix3& mat);
	void SetObj(GPUParticle *obj) {ob = obj;}
};

int 
GPUParticleCreateCallBack::proc(ViewExp *vpt,int msg, int point, int flags, IPoint2 m, Matrix3& mat )
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
				ob->pblock2->SetValue(GPUParticle_radius, ob->ip->GetTime(), 1.0f);
				break;
			case 1:
			{					
				ob->suspendSnap = TRUE; 
				p1 = vpt->SnapPoint(m,m,NULL,SNAP_IN_PLANE);
							
				float theSize = Length(p1 - p0);
				
				//Set the overall size in parameter block
				ob->pblock2->SetValue(GPUParticle_radius, ob->ip->GetTime(), theSize);
								
				//Invalidate and display the mesh in the viewport
				GPUParticle_param_blk.InvalidateUI();
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

static GPUParticleCreateCallBack sGPUParticleCreateCallBack;

//From BaseObject
CreateMouseCallBack* GPUParticle::GetCreateMouseCallBack() 
{	
	sGPUParticleCreateCallBack.SetObj(this);
	return(&sGPUParticleCreateCallBack);
}

BaseInterface* GPUParticle::GetInterface(Interface_ID id)
{
	if (IOBJECT_DISPLAY2_INTERFACE_ID == id)
	{
		return this;
	}
	return SimpleObject2::GetInterface(id);
}

//From SimpleObject
void 
GPUParticle::BuildMesh(TimeValue t)
{
	// SimpleObject has a ivalid member that stores the validity of the object.
	// We need to update the validity here. The mesh is supposed to be always valid.
	// If values controlling the mesh are animated they will updated the validity interval
	// when GetValue is called.
	ivalid = FOREVER;
}

void 
GPUParticle::InvalidateUI() 
{
	// Hey! Update the param blocks
	pblock2->GetDesc()->InvalidateUI();
}

// From Object
int 
GPUParticle::IntersectRay(
		TimeValue t, Ray& ray, float& at, Point3& norm)
{
	//TODO: Return TRUE after you implement this method
	return FALSE;
}

void 
GPUParticle::GetCollapseTypes(Tab<Class_ID> &clist,Tab<TSTR*> &nlist)
{
    Object::GetCollapseTypes(clist, nlist);
	//TODO: Append any any other collapse type the plugin supports
}

// From ReferenceTarget
RefTargetHandle GPUParticle::Clone(RemapDir& remap) 
{
	GPUParticle* newob = new GPUParticle();	
	// Make a copy all the data and also clone all the references
	newob->ReplaceReference(0,remap.CloneRef(pblock2));
	newob->ivalid.SetEmpty();
	BaseClone(this, newob, remap);
	return(newob);
}
bool GPUParticle::PrepareDisplay(const MaxSDK::Graphics::UpdateDisplayContext& displayContext)
{
	if (nullptr == mpParticleItem)
	{
		MaxSDK::Graphics::DeviceCaps caps;
		GetIDisplayManager()->GetDeviceCaps(caps);
		if (caps.FeatureLevel != Level5_0)
		{
			return false;
		}
		mpParticleItem = new ParticleRenderer;
	}

	TimeValue t = displayContext.GetDisplayTime();
	pblock2->GetValue(GPUParticle_radius, t, mRadius, ivalid );
	pblock2->GetValue(GPUParticle_count, t, mCount, ivalid );

	mpParticleItem->Radius(mRadius);
	mpParticleItem->Count(mCount);

	return true;
}

bool GPUParticle::UpdatePerNodeItems(
	const UpdateDisplayContext& updateDisplayContext,
	UpdateNodeContext& nodeContext,
	IRenderItemContainer& targetRenderItemContainer)
{
	CustomRenderItemHandle newHandle;
	newHandle.Initialize();
	//VisibilityGroup can be shaded or gizmo. By putting to shaded group,
	//the render item can take benefit from multipass effects like DOF.
	//Gizmo group will be rendered in every viewport.
	newHandle.SetVisibilityGroup(RenderItemVisible_Gizmo);
	newHandle.SetCustomImplementation(mpParticleItem.GetPointer());
	targetRenderItemContainer.AddRenderItem(newHandle);
	return true;
}

void GPUParticle::GetLocalBoundBox(TimeValue t, INode* inode, ViewExp* vpt, Box3& box )
{
	box.pmin = Point3(-mRadius, -mRadius, -10.0f);
	box.pmax = Point3(mRadius, mRadius, 10.0f);
}

void GPUParticle::GetWorldBoundBox(TimeValue t, INode* inode, ViewExp* vpt, Box3& box )
{
	box.pmin = Point3(-mRadius, -mRadius, -10.0f);
	box.pmax = Point3(mRadius, mRadius, 10.0f);
	Matrix3 mat = inode->GetObjectTM(t);
	box = box * mat;
}

void GPUParticle::GetDeformBBox(TimeValue t, Box3& box, Matrix3 *tm, BOOL useSel )
{
	box.pmin = Point3(-mRadius, -mRadius, -10.0f);
	box.pmax = Point3(mRadius, mRadius, 10.0f);
	box = box * (*tm);
}

int GPUParticle::HitTest(TimeValue t, INode *inode, int type, int crossing, int flags, IPoint2 *p, ViewExp *vpt)
{
	return 1;
}
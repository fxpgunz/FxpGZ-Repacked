/**********************************************************************
 *<
	FILE: VertexShaderMod.h

	DESCRIPTION: SineWave XTC Modifier

	CREATED BY: Neil Hazzard, Discreet

	HISTORY: Created 1/4/2001

 *>	Copyright (c) 2000, All Rights Reserved.
 **********************************************************************/


#ifndef __SINEWAVE__H
#define __SINEWAVE__H

#include "Max.h"
#include "resource.h"
#include "istdplug.h"
#include "iparamb2.h"
#include "iparamm2.h"

#include "meshadj.h"
#include "xtcobject.h"
#include "IDX9VertexShader.h"
#include "ID3D9GraphicsWindow.h"


#define SAFE_DELETE(p)			{ if (p) { delete (p);		(p)=NULL; } }
#define SAFE_DELETE_ARRAY(p)	{ if (p) { delete[] (p);	(p)=NULL; } }
#define SAFE_RELEASE(p)			{ if (p) { (p)->Release();	(p)=NULL; } }

extern TCHAR *GetString(int id);

extern HINSTANCE hInstance;

extern ClassDesc2* GetSineWaveModDesc();

#define SINEWAVEMOD_CLASS_ID	Class_ID(0x6d886a7b, 0x51376ee0)

#define PBLOCK_REF	0

class IDX9SineWave;

#define CV_WORLDVIEWPROJ_0 0
#define CV_WORLDVIEWPROJ_1 1
#define CV_WORLDVIEWPROJ_2 2
#define CV_WORLDVIEWPROJ_3 3

#define CV_WORLDVIEW_0     4
#define CV_WORLDVIEW_1     5
#define CV_WORLDVIEW_2     6
#define CV_WORLDVIEW_3     7

#define CV_WORLDVIEWIT_0   8
#define CV_WORLDVIEWIT_1   9
#define CV_WORLDVIEWIT_2  10

#define CV_ZERO           12
#define CV_ONE            13
#define CV_HALF           14

#define CV_VECSIN         15
#define CV_VECCOS         16

#define CV_TIME           17
#define CV_PIS            18

class SineWaveMod : public Modifier {
	public:
		// Parameter block
		IParamBlock2	*pblock;	//ref 0
		static IObjParam *ip;			//Access to the interface
		// From Animatable
		const TCHAR *GetObjectName() { return GetString(IDS_VSCLASS_NAME); }

		//From Modifier
		//TODO: Add the channels that the modifier needs to perform its modification
		ChannelMask ChannelsUsed()  { return GEOM_CHANNEL|TOPO_CHANNEL; }
		//TODO: Add the channels that the modifier actually modifies
		ChannelMask ChannelsChanged() { return EXTENSION_CHANNEL; }
		void ModifyObject(TimeValue t, ModContext &mc, ObjectState *os, INode *node);
		//TODO: Return the ClassID of the object that the modifier can modify
		Class_ID InputType() {return defObjectClassID;}
		Interval LocalValidity(TimeValue t);

		// From BaseObject
		//TODO: Return true if the modifier changes topology
		BOOL ChangeTopology() {return FALSE;}		

		
		CreateMouseCallBack* GetCreateMouseCallBack() {return NULL;}
		void BeginEditParams(IObjParam *ip, ULONG flags,Animatable *prev);
		void EndEditParams(IObjParam *ip, ULONG flags,Animatable *next);

		Interval GetValidity(TimeValue t);

		// Automatic texture support
		BOOL HasUVW();
		void SetGenUVW(BOOL sw);
		
		// Loading/Saving
		IOResult Load(ILoad *iload);
		IOResult Save(ISave *isave);

		//From Animatable
		Class_ID ClassID() {return SINEWAVEMOD_CLASS_ID;}		
		SClass_ID SuperClassID() { return OSM_CLASS_ID; }
		void GetClassName(TSTR& s) {s = GetString(IDS_VSCLASS_NAME);}
		
		RefTargetHandle Clone( RemapDir &remap );
		RefResult NotifyRefChanged(const Interval& changeInt, RefTargetHandle hTarget, 
			PartID& partID, RefMessage message, BOOL propagate);

		int NumSubs() { return 1; }
		TSTR SubAnimName(int i) { return GetString(IDS_VSPARAMS); }				
		Animatable* SubAnim(int i) { return pblock; }
		int NumRefs() { return 1; }
		RefTargetHandle GetReference(int i) { return pblock; }
private:
		virtual void SetReference(int i, RefTargetHandle rtarg) { pblock=(IParamBlock2*)rtarg; }
public:

		int	NumParamBlocks() { return 1; }					// return number of ParamBlocks in this instance
		IParamBlock2* GetParamBlock(int i) { return pblock; } // return i'th ParamBlock
		IParamBlock2* GetParamBlockByID(BlockID id) { return (pblock->ID() == id) ? pblock : NULL; } // return id'd ParamBlock
		void DeleteThis() { delete this; }		
		//Constructor/Destructor
		SineWaveMod();
		~SineWaveMod();		

};

class SineWaveModData : public LocalModData
{
		bool bValid;
public:
	SineWaveModData() : bValid(false) {};
	SineWaveModData(bool valid) : bValid(valid){}
	LocalModData *Clone(){ return new SineWaveModData(bValid);}
	void SetValid(bool bValid);
	bool GetValid();

};

class SineWaveModClassDesc:public ClassDesc2 {
	public:
	int 			IsPublic() {return 1;}
	void *			Create(BOOL loading = FALSE) {return new SineWaveMod();}
	const TCHAR *	ClassName() {return GetString(IDS_VSCLASS_NAME);}
	SClass_ID		SuperClassID() {return OSM_CLASS_ID;}
	Class_ID		ClassID() {return SINEWAVEMOD_CLASS_ID;}
	const TCHAR* 	Category() {return GetString(IDS_CATEGORY);}
	const TCHAR*	InternalName() { return _T("VertexShaderMod"); }	// returns fixed parsable name (scripter-visible name)
	HINSTANCE		HInstance() { return hInstance; }				// returns owning module handle
};


class XTCVShader : public XTCObject
{
	BaseObject *bo;
	IDX9SineWave *pvs;
	SineWaveModData *lmd;
public:
	XTCVShader(SineWaveModData *lmd,float amount,BOOL norm,float height,BOOL wire);
	XTCVShader(XTCVShader *mFrom);
	~XTCVShader();
	virtual Class_ID ExtensionID(){return SINEWAVEMOD_CLASS_ID;}
	virtual XTCObject *Clone(){return new XTCVShader(this);}
	virtual void DeleteThis(){delete this;}
	virtual ChannelMask DependsOn(){return GEOM_CHANNEL;}
	virtual ChannelMask ChannelsChanged(){return EXTENSION_CHANNEL;}

	virtual void PreChanChangedNotify(TimeValue t, ModContext &mc, ObjectState* os, INode *node,Modifier *mod, bool bEndOfPipeline){};
	virtual void PostChanChangedNotify(TimeValue t, ModContext &mc, ObjectState* os, INode *node,Modifier *mod, bool bEndOfPipeline);
		
	virtual BaseInterface* GetInterface(Interface_ID id);
	

};

class IDX9SineWave : public IDX9VertexShader
{
	bool	initDone;

	SineWaveModData  *lmd;

	LPDIRECT3DDEVICE9 pd3dDevice;

	LPDIRECT3DCUBETEXTURE9	pCubeTexture;

	// VertexShader Instructions
	LPD3DXBUFFER pCode;

	// VertexShader Constants
	Tab<D3DXVECTOR4> Constants;

	// VertexShader Handle
	LPDIRECT3DVERTEXSHADER9	pVertexShader;
	ID3D9GraphicsWindow *testgw;

	int size;
	float AmountConstant;
	BOOL Normals;
	float Height;
	BOOL Wire;

public:
	IDX9SineWave(SineWaveModData *pLMD,float amount,BOOL norm,float height,BOOL wire);
   IDX9SineWave(IDX9SineWave*);
	~IDX9SineWave();

   BaseInterface* CloneInterface(void* remapDir);

	// From FPInterface
	virtual LifetimeType	LifetimeControl() { return noRelease; }

	// From IVertexShader
	HRESULT ConfirmDevice(ID3D9GraphicsWindow *d3dgw);

	HRESULT ConfirmPixelShader(IDX9PixelShader *pps);

	HRESULT Initialize(Mesh *mesh, INode *node);
	HRESULT Initialize(MNMesh *mnmesh, INode *node);

	bool CanTryStrips();

	int GetNumMultiPass();

	LPDIRECT3DVERTEXSHADER9 GetVertexShaderHandle(int numPass) { return pVertexShader; }

	HRESULT SetVertexShader(ID3D9GraphicsWindow *d3dgw, int numPass);

	// Draw 3D mesh as TriStrips
	bool	DrawMeshStrips(ID3D9GraphicsWindow *d3dgw, MeshData *data);

	// Draw 3D mesh as wireframe
	bool	DrawWireMesh(ID3D9GraphicsWindow *d3dgw, WireMeshData *data);

	// Draw 3D lines
	void	StartLines(ID3D9GraphicsWindow *d3dgw, WireMeshData *data);
	void	AddLine(ID3D9GraphicsWindow *d3dgw, DWORD *vert, int vis);
	bool	DrawLines(ID3D9GraphicsWindow *d3dgw);
	void	EndLines(ID3D9GraphicsWindow *d3dgw, GFX_ESCAPE_FN fn);

	// Draw 3D triangles
	void	StartTriangles(ID3D9GraphicsWindow *d3dgw, MeshFaceData *data);
	void	AddTriangle(ID3D9GraphicsWindow *d3dgw, DWORD index, int *edgeVis);
	bool	DrawTriangles(ID3D9GraphicsWindow *d3dgw);
	void	EndTriangles(ID3D9GraphicsWindow *d3dgw, GFX_ESCAPE_FN fn);
};


#endif // __VERTEXSHADERMOD__H



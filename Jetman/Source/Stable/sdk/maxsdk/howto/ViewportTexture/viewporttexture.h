#pragma once

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

#include "max.h"
#include "Graphics/StandardMaterialHandle.h"
#include "Graphics/TextureHandle.h"

//SIMPLE TYPE

#define ViewportTexture_CLASS_ID	Class_ID(0x42c07c86, 0x5a4cef12)

extern TCHAR *GetString(int id);

extern HINSTANCE hInstance;

class ViewportTexture : public GeomObject 
{
public:
	enum
	{
		pblock_ref,
		pblock_count
	};

	enum
	{
		widget_params
	};

	enum
	{
		widget_color,
		widget_texturepath
	};

	//Constructor/Destructor
	ViewportTexture();
	virtual ~ViewportTexture();	

	virtual void DeleteThis();	

	Box3 GetBox() const;
	void SetBox(Box3& b);

	// From BaseObject
	virtual CreateMouseCallBack* GetCreateMouseCallBack();
	virtual int Display(TimeValue t, INode* inode, ViewExp *vpt, int flags);
	virtual int HitTest(TimeValue t, INode* inode, int type, int crossing, int flags, IPoint2 *p, ViewExp *vpt);
	virtual void Snap(TimeValue t, INode* inode, SnapInfo *snap, IPoint2 *p, ViewExp *vpt);
	//TODO: Return the name that will appear in the history browser (modifier stack)
	virtual const TCHAR *GetObjectName();

	virtual void GetWorldBoundBox(TimeValue t, INode *mat, ViewExp *vpt, Box3& m_box );
	virtual void GetLocalBoundBox(TimeValue t, INode *mat, ViewExp *vpt, Box3& m_box );

	virtual void GetDeformBBox(TimeValue t, Box3& m_box, Matrix3 *tm, BOOL useSel );
	//TODO: Return the default name of the node when it is created.
	virtual void InitNodeName(TSTR& s);

	// From Object
	virtual BOOL HasUVW();
	virtual void SetGenUVW(BOOL sw);
	virtual int CanConvertToType(Class_ID obtype);
	virtual Object* ConvertToType(TimeValue t, Class_ID obtype);
	virtual void GetCollapseTypes(Tab<Class_ID> &clist,Tab<TSTR*> &nlist);
	virtual int IntersectRay(TimeValue t, Ray& ray, float& at, Point3& norm);
	virtual ObjectState Eval(TimeValue t); 
	virtual Interval ObjectValidity(TimeValue t);

	// From Animatable
	virtual void BeginEditParams( IObjParam  *ip, ULONG flags,Animatable *prev);
	virtual void EndEditParams( IObjParam *ip, ULONG flags,Animatable *next);

	// From GeomObject
	virtual int IsRenderable() { return(0); }
	virtual Mesh* GetRenderMesh(TimeValue t, INode *inode, View& view, BOOL& needDelete);

	// Loading/Saving
	virtual IOResult Load(ILoad *iload);
	virtual IOResult Save(ISave *isave);

	//From Animatable
	virtual Class_ID ClassID();
	virtual SClass_ID SuperClassID();
	virtual void GetClassName(TSTR& s);

	virtual RefTargetHandle Clone( RemapDir &remap );
	virtual RefResult NotifyRefChanged(const Interval& changeInt, 
		RefTargetHandle hTarget, 
		PartID& partID,  
		RefMessage message, 
		BOOL propagate);


	virtual int NumSubs();
	virtual TSTR SubAnimName(int i);
	virtual Animatable* SubAnim(int i);

	// TODO: Maintain the number or references here
	virtual int NumRefs();
	virtual RefTargetHandle GetReference(int i);

	// from IObjectDisplay2
	unsigned long GetObjectDisplayRequirement() const;
	bool PrepareDisplay(const MaxSDK::Graphics::UpdateDisplayContext& prepareDisplayContext);

	bool UpdatePerNodeItems( 
		const MaxSDK::Graphics::UpdateDisplayContext& updateDisplayContext,
		MaxSDK::Graphics::UpdateNodeContext& nodeContext,
		MaxSDK::Graphics::IRenderItemContainer& targetRenderItemContainer);


private:
	virtual void SetReference(int i, RefTargetHandle rtarg);
	bool GenerateMaterialHandle
		(MaxSDK::Graphics::StandardMaterialHandle& materialHandle);
	bool GenerateTextureHandle
		(MaxSDK::Graphics::TextureHandle& textureHandle);
	MaxSDK::Graphics::StandardMaterialHandle m_customMaterialHandle;
	MaxSDK::Graphics::TextureHandle m_customTextureHandle;
public:

	virtual int	NumParamBlocks();
	virtual IParamBlock2* GetParamBlock(int i);
	virtual IParamBlock2* GetParamBlockByID(BlockID id);

	// local methods
	long m_flags;
	Box3 m_box;

private:
	void UpdateMesh();
	void BuildMesh();
	void SetMaterial();
	void LoadTexture();
	void PlugTextureInfo();

	// Parameter block
	IParamBlock2* m_pblock; //ref 0

	Interval m_valid;
	Mesh m_mesh;

	Material m_mat;
	DWORD_PTR m_texHandle;
	GraphicsWindow* m_gw;
	Bitmap* m_bitmap;
	TSTR m_lastBitmapFilename;

	bool m_dirtyMaterial;
	bool m_dirtyTexture;
};

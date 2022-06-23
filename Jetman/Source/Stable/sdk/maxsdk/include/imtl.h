//**************************************************************************/
// Copyright (c) 1998-2006 Autodesk, Inc.
// All rights reserved.
// 
//  Use of this software is subject to the terms of the Autodesk license 
//  agreement provided at the time of installation or download, or which 
//  otherwise accompanies this software in either electronic or hard copy form.
//**************************************************************************/
// FILE:        imtl.h
// DESCRIPTION: Renderer materials
// AUTHOR:      Dan Silva 
// HISTORY:      
//**************************************************************************/
#pragma once

// Include legacy files that contain things which used to be defined in this header
#include "Rendering/RenderGlobalContext.h"
#include "Rendering/ShadeContext.h"
#include "Rendering/ViewParams.h"
#include "Materials/AttachMatDlgUser.h"
#include "Materials/DADBitmapCarrier.h"
#include "Materials/IllumParams.h"
#include "Materials/IMtlParams.h"
#include "Materials/IReshadeFragment.h"
#include "Materials/IReshading.h"
#include "Materials/ISubMap.h"
#include "Materials/MapSampler.h"
#include "Materials/Mtl.h"
#include "Materials/MtlBase.h"
#include "Materials/MtlDADMgr.h"
#include "Materials/MtlLib.h"
#include "Materials/ParamDlg.h"
#include "Materials/PStamp.h"
#include "Materials/TexDADMgr.h"
#include "Materials/TexHandle.h"
#include "Materials/Texmap.h"
#include "Materials/TextureOutput.h"
#include "Materials/UVGen.h"
#include "Materials/XYZGen.h"
#include "NumList.h"

#pragma warning(push)
#pragma warning(disable:4100)

class PatchMesh;
class Mesh;
class BezierShape;
class MNMesh;

#define  X_AXIS 0
#define  Y_AXIS 1
#define  Z_AXIS 2

#define PS_SMALL 0
#define PS_LARGE 1
#define PS_TINY  2

#define PS_TINY_SIZE  24  // size of the tiny postage stamps
#define PS_SMALL_SIZE 32  // size of the small postage stamps
#define PS_LARGE_SIZE 88  // size of the large postage stamps
#define PSDIM(isz) ((isz==0)?PS_SMALL_SIZE:(isz==1)?PS_LARGE_SIZE:PS_TINY_SIZE)

#define ByteWidth(w) (((w*3+3)/4)*4)

// Meta-materials post this message to the MtlEditor when user
// clicks on a sub-material button.
#define WM_SUB_MTL_BUTTON  WM_USER + 0x04001

// Materials or Texture maps post this message to the MtlEditor when user
// clicks on a texture map button.
#define WM_TEXMAP_BUTTON   WM_USER + 0x04002

/*! \sa  <a href="class_i_node.html#A_GM_inode_renderdata">Class INode-Access To Render Data</a>, 
Class LightDesc.\n\n
\par Description:
This class has a single virtual method to delete the class. For example, when
<b>LightDesc</b> objects are deleted this is the method to do so.  */
class RenderData : public InterfaceServer {
	 public:
		/*! \remarks This method is used to delete an instance of the class.
		\par Default Implementation:
		<b>{delete this; }</b> */
		virtual void DeleteThis()=0;
		/*! \remarks		This is a general purpose function that allows the API to be extended in the
		future. The 3ds Max development team can assign new <b>cmd</b> numbers and
		continue to add functionality to this class without having to 'break' the
		API.\n\n
		This is reserved for future use.
		\par Parameters:
		<b>int cmd</b>\n\n
		The command to execute.\n\n
		<b>ULONG arg1=0</b>\n\n
		Optional argument 1 (defined uniquely for each <b>cmd</b>).\n\n
		<b>ULONG arg2=0</b>\n\n
		Optional argument 2.\n\n
		<b>ULONG arg3=0</b>\n\n
		Optional argument 3.
		\return  An integer return value (defined uniquely for each <b>cmd</b>).
		\par Default Implementation:
		<b>{ return 0; }</b> */
			virtual INT_PTR Execute(int cmd, ULONG_PTR arg1=0, ULONG_PTR arg2=0, ULONG_PTR arg3=0) { return 0;} 
	 };

/*! \sa  Class RenderData, Class LightRayTraversal, Class ObjLightDesc, Class ShadeContext, Class Color,  Class Point3.\n\n
\par Description:
This class has a method <b>Illuminate()</b> used to determine the color and
direction of the light striking the point <b>sc.P()</b> and a method to get the
position of the light. It also has two public data members that determine if
the diffuse and specular colors of objects are affected.
\par Data Members:
<b>BOOL affectDiffuse;</b>\n\n
This data member is available in release 2.0 and later only.\n\n
If TRUE the light affects the diffuse color; otherwise it does not.\n\n
<b>BOOL affectSpecular;</b>\n\n
This data member is available in release 2.0 and later only.\n\n
If TRUE the light affects the specular color; otherwise it does not.\n\n
<b>BOOL ambientOnly;</b>\n\n
This data member is available in release 3.0 and later only.\n\n
If TRUE the light affects the ambient color only; otherwise it does not.\n\n
<b>DWORD extra;</b>\n\n
This data member is available in release 3.0 and later only.\n\n
This is not currently used and is available for use in the future.  */
class LightDesc : public RenderData {
	 public:
			BOOL affectDiffuse;
			BOOL affectSpecular;
			BOOL  ambientOnly;
			DWORD  extra;
			/*! \remarks Constructor. The <b>affectDiffuse</b> and
			<b>affectSpecular</b> data members are set to TRUE. */
			LightDesc() { affectDiffuse = affectSpecular = TRUE; ambientOnly = FALSE; }
			// determine color and direction of illumination: return FALSE if light behind surface.
			// also computes dot product with normal dot_nl, and diffCoef which is to be used in
			// place of dot_n for diffuse light computations.
			 /*! \remarks This method is called to determine the color and direction of
			 illumination from the light that is hitting the point (<b>sc.P()</b>).
			 \par Parameters:
			 <b>ShadeContext\& sc</b>\n\n
			 Describes the properties of the point to shade. The point itself is
			 <b>sc.P()</b>.\n\n
			 <b>Point3\& normal</b>\n\n
			 The normal to the surface in camera space.\n\n
			 <b>Color\& color</b>\n\n
			 The color that is returned. This is the brightness of light striking the
			 point (<b>sc.P()</b>)\n\n
			 <b>Point3 \&dir</b>\n\n
			 The direction that the light is coming from.\n\n
			 <b>float \&dot_nl</b>\n\n
			 This provides a bit of optimization as most lights will have to
			 calculate this quantity. This is the dot product between the light
			 vector and the normal to the surface.\n\n
			 <b>float \&diffuseCoef</b>\n\n
			 This parameter should be set by the Illuminate function. The default
			 value is the same as dot_nl. It will be used by shading functions
			 instead of dot_nl to compute the diffuse illumination of the surface.
			 The built-in lights use the new "Contrast" parameter (which has a range
			 of [0..100]) to compute the diffuseCoef from the dot_nl by the Contrast function:
			\code
			// precomputed:
			float a = contrast/200.0f + 0.5f;				  // so "a" varies from .5 to 1.0
			kA = (2.0f-1.0f/a);
			kB = 1.0f-kA;
			
			// called by Illuminate() to compute diffuseCoef from dot_nl.
			float ContrastFunc(float nl)
			{
			// the "Bias" function described in Graphics Gems IV, pp. 401ff
				return (contrast==0.0f)? nl : nl/(kA*nl+kB);
			}
			\endcode 
			 \return  Returns FALSE if the hitpoint is outside the effective range of
			 the light or if the normal of the surface faces away from the light.
			 This is a hint to the material that the light did not calculate its
			 illumination because it is assumed it wasn't going to be used. If TRUE
			 the point is being illuminated.
			 \par Default Implementation:
			 <b>{ return 0;}</b> */
			 virtual BOOL Illuminate(ShadeContext& sc, Point3& normal, Color& color, Point3 &dir, float &dot_nl, float &diffuseCoef){ return 0;}
			
			/*! \remarks			Returns the position of the light.
			\par Default Implementation:
			<b>{ return Point3(0,0,0); }</b> */
			virtual Point3 LightPosition() { return Point3(0,0,0); } 

			CoreExport virtual void DeleteThis();

	 };

// Pre-defined categories of texture maps
CoreExport MCHAR TEXMAP_CAT_2D[];  // 2D maps
CoreExport MCHAR TEXMAP_CAT_3D[];  // 3D maps
CoreExport MCHAR TEXMAP_CAT_COMP[]; // Composite
CoreExport MCHAR TEXMAP_CAT_COLMOD[]; // Color modifier
CoreExport MCHAR TEXMAP_CAT_ENV[];  // Environment

// this interface is a dummy interface returned by texmap::getinterface
// if the map is a ray tracing type map.
#define IID_RAYTRACE_MAP   0xff8d87a5

class IRayTexmap : public InterfaceServer {
public:
	 bool isRayTraceMap() { return true; }
};

/*! \sa  Class ReferenceTarget, Class Texmap.\n\n
\par Description:
This is simply a class that provides an implementation of
<b>Animatable::SuperClassID()</b> to return <b>TEXMAP_CONTAINER_CLASS_ID</b>.
In 3ds Max 2.0 and later this new super class has been added. This is used by
the Standard material to contain its Texmaps. The track view filter code has
been modified so it now looks for this class and will filter it out when maps
are being filtered out, instead of having special purpose code for the Standard
Material. This will permit plug-in developers to put their Texmaps down in a
sub-directory like the Standard material does. */
class TexmapContainer: public ReferenceTarget {
	 SClass_ID SuperClassID() { return TEXMAP_CONTAINER_CLASS_ID; }
	 CoreExport SvGraphNodeReference SvTraverseAnimGraph(IGraphObjectManager *gom, Animatable *owner, int id, DWORD flags);
	 };

//! Class ID used for the Gnormal and VDM classes found in maxsdk\samples\materials\NormalBump\normalrender.h & vdm.h
#define GNORMAL_CLASS_ID	Class_ID(0x243e22c6, 0x63f6a014)
#define VDM_CLASS_ID		Class_ID(0x152c55c7, 0x26160e93)

//! Used for normal and vector displacement map generation. 
//! \note Moved to SDK since FBX also depends on this
//-- UI and Param Block
enum Normal_Block_IDs
{
	gnormal_params
};
enum Normal_Param_IDs
{ 
	gn_mult_spin,
	gn_bmult_spin,
	gn_map_normal,
	gn_map_bump,
	gn_map1on,
	gn_map2on,
	gn_method,
	gn_flip_red,
	gn_flip_green,
	gn_swap_rg
};

enum VDM_Block_IDs
{
	vdm_params
};

enum VDM_Param_IDs
{
	vdm_mult_spin,				// Scaling value (float)
	vdm_map_vector_enabled,		// Whether the map is enabled or not (bool)
	vdm_map_vector,				// A sub-map defining the actual vector data as a Texmap 
	vdm_is_hdr,					// Whether the sub-map is high-dynamic range and doesn't need re-scaling (i.e. value = pixel * 2.0 - 1.0). (bool)
	vdm_method					// Space the vector map is defined in [0==World, 1==Object, 2==Tangent] (int)
};

CoreExport void SetLoadingMtlLib(MtlLib *ml);
CoreExport void SetLoadingMtlBaseLib(MtlBaseLib *ml);

inline int IsMtl(Animatable *m) { return m && m->SuperClassID()==MATERIAL_CLASS_ID; }
inline int IsTex(Animatable *m) { return m && m->SuperClassID()==TEXMAP_CLASS_ID; }
inline int IsMtlBase(Animatable *m) { return IsMtl(m)||IsTex(m); }

// Combines the two materials into a multi-material.
// Either of the two input materials can themselves be multis.
// c1 and c2 will be set to the mat count for mat1 and mat2.
CoreExport Mtl* CombineMaterials(Mtl *mat1, Mtl *mat2, int &mat2Offset);

// Expand multi material size to the largest face ID
CoreExport Mtl* FitMaterialToMeshIDs(Mesh &mesh,Mtl *mat);
CoreExport Mtl* FitMaterialToShapeIDs(BezierShape &shape, Mtl *mat);
CoreExport Mtl* FitMaterialToPatchIDs(PatchMesh &patch, Mtl *mat);
CoreExport Mtl* FitMaterialToPolyMeshIDs (MNMesh & mesh, Mtl *mat);

// Adjust face IDs so that no face ID is greater then them multi size
CoreExport void FitMeshIDsToMaterial(Mesh &mesh, Mtl *mat);
CoreExport void FitShapeIDsToMaterial(BezierShape &shape, Mtl *mat);
CoreExport void FitPatchIDsToMaterial(PatchMesh &patch, Mtl *mat);
CoreExport void FitPolyMeshIDsToMaterial (MNMesh & mesh, Mtl *mat);

// Remove unused mats in a multi and shift face IDs.
CoreExport Mtl *CondenseMatAssignments(Mesh &mesh, Mtl *mat);
CoreExport Mtl *CondenseMatAssignments(BezierShape &shape, Mtl *mat);
CoreExport Mtl *CondenseMatAssignments(PatchMesh &patch, Mtl *mat);
CoreExport Mtl *CondenseMatAssignments (MNMesh &mesh, Mtl *mat);

#define IID_INewSubMapNotification Interface_ID(0x3fe013b2, 0x31815874)

// Optional interface which can be implemented by classes derived from MtlBase.
// If this interface is returned from GetInterface, then this method is called
// when a new sub-texmap is added to the material.  It lets the material set
// material-specific defaults on the texture map.  This is only called when
// adding newly created textures to the material.
class INewSubTexmapNotification: public BaseInterface
{
public:
		virtual void NewSubTexmapAdded(int i, Texmap *m) = 0;
	 Interface_ID   GetID() { return IID_INewSubMapNotification; }
};

#define DEFAULT_TEXTURE_MAP_SIZE_INCHES 48.0f
#define DEFAULT_TEXTURE_MAP_SIZE_METERS 1.0f

#pragma warning(pop)
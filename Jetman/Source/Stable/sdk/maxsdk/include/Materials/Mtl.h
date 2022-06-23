//////////////////////////////////////////////////////////////////////////////
//
//  Copyright 2015 Autodesk, Inc.  All rights reserved.
//
//  Use of this software is subject to the terms of the Autodesk license 
//  agreement provided at the time of installation or download, or which 
//  otherwise accompanies this software in either electronic or hard copy form.   
//
//////////////////////////////////////////////////////////////////////////////
#pragma once

#include "MtlBase.h"
#include "../Color.h"

#pragma warning(push)
#pragma warning(disable:4100)

class RenderData;
class Sampler;

// virtual material interface
/*! \sa  Class MtlBase, Class Texmap, Class ShadeContext, Class ShadeOutput, Class Interval, Class Color.\n\n
\par Description:
This is the base class for the creation of material plug-ins. This class
provides methods to do things such as compute the properties of the material
for a given location, return the number of sub-materials and access the
properties of the material for the interactive renderer.
\par Plug-In Information:
Class Defined In IMTL.H\n\n
Super Class ID MATERIAL_CLASS_ID\n\n
Standard File Name Extension DLT\n\n
Extra Include File Needed IMTL.H
*/
class Mtl: public MtlBase 
{
RenderData *renderData;
public:

	/*! \remarks Constructor. The active texture map is set to NULL. */
	CoreExport Mtl();
	SClass_ID SuperClassID() { return MATERIAL_CLASS_ID; }
	virtual void GetClassName(MSTR& s) { s= MSTR(_M("Mtl")); }  

//! \name Get/SetActiveTexmap
//! @{
	/*! \remarks Implemented by the System.
	Returns a pointer to the active texture map used in the interactive
	renderer. Returns NULL if there isn't any. */
	CoreExport MtlBase* GetActiveTexmap();
	/*! \remarks Implemented by the System.
	Stores the pointer to the active texture map used in the interactive
	renderer. Note that this method does not do everything required to update
	the viewports with the new Texmap. To accomplish that call
	Interface::ActivateTexture(). */
	CoreExport void SetActiveTexmap(MtlBase *txm);
//! @}

	CoreExport void RefDeleted();
	CoreExport void RefAdded(RefMakerHandle rm);  

//! \name Get Properties Methods
//! @{			
	// Must call Update(t) before calling these functions!
	// Their purpose is for setting up the material for the
	// GraphicsWindow renderer.
	/*! \remarks Returns the ambient color of the specified material for use in the
	interactive renderer.
	\param mtlNum This is the material index for mult-materials.
	\param backFace If the surface normal of the face is pointing away from the viewer this
	will be TRUE; otherwise FALSE. */
	virtual Color GetAmbient(int mtlNum=0, BOOL backFace=FALSE)=0;
	/*! \remarks Returns the diffuse color of the specified material for use in the
	interactive renderer.
	\param mtlNum This is the material index for mult-materials.
	\param backFace If the surface normal of the face is pointing away from the viewer this
	will be TRUE; otherwise FALSE. */
	virtual Color GetDiffuse(int mtlNum=0, BOOL backFace=FALSE)=0;     
	/*! \remarks Returns the specular color of the specified material for use in the
	interactive renderer.
	\param mtlNum This is the material index for mult-materials.
	\param backFace If the surface normal of the face is pointing away from the viewer this
	will be TRUE; otherwise FALSE. */
	virtual Color GetSpecular(int mtlNum=0, BOOL backFace=FALSE)=0;
	/*! \remarks Returns the shininess value of the specified material for use in the
	interactive renderer.
	\param mtlNum This is the material index for mult-materials.
	\param backFace If the surface normal of the face is pointing away from the viewer this
	will be TRUE; otherwise FALSE. */
	virtual float GetShininess(int mtlNum=0, BOOL backFace=FALSE)=0;
	/*! \remarks Returns the shininess strength value of the specified material for use in
	the interactive renderer.
	\param mtlNum This is the material index for mult-materials.
	\param backFace If the surface normal of the face is pointing away from the viewer this
	will be TRUE; otherwise FALSE. */
	virtual  float GetShinStr(int mtlNum=0, BOOL backFace=FALSE)=0;      
	/*! \remarks Returns the transparency value of the specified material for use in the
	interactive renderer.
	\param mtlNum This is the material index for mult-materials.
	\param backFace If the surface normal of the face is pointing away from the viewer this
	will be TRUE; otherwise FALSE. */
	virtual float GetXParency(int mtlNum=0, BOOL backFace=FALSE)=0;

	// >>>>> Self-Illum
	/*! \remarks			Returns TRUE if the Self Illumination Color is on (checked) for the
	specified material; otherwise FALSE.
	\param mtlNum This is the material index for mult-materials.
	\param backFace If the surface normal of the face is pointing away from the viewer this
	will be TRUE; otherwise FALSE.
	\par Default Implementation:
	<b>{ return TRUE; }</b> */
	virtual BOOL GetSelfIllumColorOn(int mtlNum=0, BOOL backFace=FALSE) { return TRUE; }
	/*! \remarks Returns the self illumination value of the specified material for use in
	the interactive renderer.
	\param mtlNum This is the material index for mult-materials.
	\param backFace If the surface normal of the face is pointing away from the viewer this
	will be TRUE; otherwise FALSE.
	\par Default Implementation:
	<b>{ return 0.0f; }</b> */
	virtual float GetSelfIllum(int mtlNum=0, BOOL backFace=FALSE) { return 0.0f; }
	/*! \remarks			Returns the Self Illumination Color of the specified material for use in
	the interactive renderer.
	\param mtlNum This is the material index for mult-materials.
	\param backFace If the surface normal of the face is pointing away from the viewer this
	will be TRUE; otherwise FALSE.
	\par Default Implementation:
	<b>{ Color c( .0f,.0f,.0f); return c; }</b> */
	virtual Color GetSelfIllumColor(int mtlNum=0, BOOL backFace=FALSE){ Color c( .0f,.0f,.0f); return c; }

	// sampler
	/*! \remarks Returns a pointer to the Sampler used for the specified sub-material.
	\param mtlNum This is the material index for mult-materials.
	\param backFace If the surface normal of the face is pointing away from the viewer this
	will be TRUE; otherwise FALSE.
	\par Default Implementation:
	<b>{ return NULL; }</b> */
	virtual Sampler * GetPixelSampler(int mtlNum=0, BOOL backFace=FALSE){ return NULL; }

	// used by the scanline renderer
	/*! \remarks Returns the wire size of the material.
	\param mtlNum This is the material index for mult-materials.
	\param backFace If the surface normal of the face is pointing away from the viewer this
	will be TRUE; otherwise FALSE.
	\par Default Implementation:
	<b>{ return 1.0f; }</b> */
	virtual float WireSize(int mtlNum=0, BOOL backFace=FALSE) { return 1.0f; }
//! @}

//! \name Set Properties Methods
//! @{
	// why incomplete sets?? 
	// used onlt for default material creation....add no more
	/*! \remarks This method saves the specified color at the specified time.
	\param c The color to store.
	\param t The time to set this color. */
	virtual void SetAmbient(Color c, TimeValue t)=0;      
	/*! \remarks This method saves the specified color at the specified time.
	\param c The color to store.
	\param t The time to set this color. */
	virtual void SetDiffuse(Color c, TimeValue t)=0;      
	/*! \remarks This method saves the specified color at the specified time.
	\param c The color to store.
	\param t The time to set this color. */
	virtual void SetSpecular(Color c, TimeValue t)=0;
	/*! \remarks This method saves the specified shininess at the specified time.
	\param v The shininess value to store.
	\param t The time to set this color. */
	virtual void SetShininess(float v, TimeValue t)=0;
//! @}

	/*! \remarks			This method returns TRUE if the material supports shaders, otherwise
	FALSE.
	\par Default Implementation:
	<b>{ return FALSE; }</b> */
	virtual BOOL SupportsShaders() { return FALSE; } // moved from class StdMat

	//! Returns true if it supports render elements
	virtual BOOL SupportsRenderElements() { return FALSE; } 

//! \name Shade
//! @{
	/*! \remarks This is the main method of the material. This is called by the renderer
	to compute the color and transparency output returned in <b>sc.out</b>.
	\param sc Describes properties of the pixel to be shaded. The result of this method
	is returned in the <b>ShadeOutput</b> data member of <b>sc</b>. */
	virtual void Shade(ShadeContext& sc)=0;
//! @}

//! \name Sub-material Access Methods
//! @{
	// Methods to access sub-materials of meta-materials 
	/*! \remarks Returns the number of sub-materials managed by this material.
	\par Default Implementation:
	<b>{ return 0; }</b> */
	virtual int NumSubMtls() { return 0; }
	/*! \remarks Returns a pointer to the 'i-th' sub-material of this material.
	\param i The index of the material to return.
	\par Default Implementation:
	<b>{ return NULL; }</b> */
	virtual Mtl* GetSubMtl(int i) { return NULL; }
	/*! \remarks Stores the 'i-th' sub-material of this material.
	\param i The index of the material to store.
	\param m The material pointer to store.
	\par Default Implementation:
	<b>{}</b> */
	virtual void SetSubMtl(int i, Mtl *m) { }
	/*! \remarks			This method returns which sub-mtl is to display in the viewport, a return
	value of:-1 indicates not implemented.\n\n
	Note that when a material, such as Blend, has a method of selecting which
	sub-map is to be shown in the viewport, and implements this method, the
	materials editor lets you turn on Show Map In Viewport (SMIV) in all the
	sub maps at once. When the material, such as top-bottom, doesn't have a
	selector, and doesn't implement this method, then the materials editor
	will only let you turn on SMIV for one map/mtl in the entire sub-tree of
	the material.
	\par Default Implementation:
	<b>{ return -1; }</b> */
	virtual int VPDisplaySubMtl() { return -1; }  // which sub-mtl to display in the viewport: -1 indicates not implemented.
//! @}

//! \name Naming Methods
//! @{
	/*! \remarks This method returns the slot name of the 'i-th' sub-material. This name
	appears in the materials editor dialog. For instance, if you are in a
	material and then you go down into a sub-material, this is the name that
	appears just below the 'Get Material' icon. For example, in the
	Multi/Sub-Object material when you choose one of the sub-materials, the
	map name appears to let you know which slot you are working on. For the
	Multi/Sub-Object material, this is the number of the slot, i.e."#1:",
	"#2:", "#3:", etc.
	\param i The index of the sub-materials whose slot name should be returned. */
	CoreExport virtual MSTR GetSubMtlSlotName(int i);
	/*! \remarks Returns the name of the 'i-th' sub-material that should appear in track
	view.
	\param i The index of the sub-materials whose track view name should be returned. */
	CoreExport MSTR GetSubMtlTVName(int i);
//! @}

	CoreExport void CopySubMtl(HWND hwnd, int ifrom, int ito);

//! \name Displacement Mapping
//! @{
	// Displacement mapping
	/*! \remarks			Returns the amount of displacement along the normal of the surface at the
	point as specified by the <b>ShadeContext</b>.
	\param sc This contains the details of the point being displaced.
	\par Default Implementation:
	<b>{ return 0.0f; }</b> */
	virtual float EvalDisplacement(ShadeContext& sc) { return 0.0f; }
	/*! \remarks			Returns the validity interval of the displacement mapping around the
	specified time.
	\param t The Interval returned reflects the validity around this time.
	\par Default Implementation:
	<b>{ return FOREVER; }</b> */
	virtual Interval DisplacementValidity(TimeValue t) { return FOREVER; }
//! @}

	// Schematic View Animatable Overrides...
	CoreExport bool SvCanInitiateLink(IGraphObjectManager *gom, IGraphNode *gNode);

//! \name Replace Material Dialog
//! @{				
	// allows plugins to control the display of the Discard/Keep Old Material dialog when being
	// created in a MtlEditor slot.  Added by JBW for scripted material plugins.
	// return TRUE to prevent the Keep Old Material dialog from pooping.
	/*! \remarks			Return TRUE to prevent the Replace Material (Discard old material? / Keep
	old material as sub-material?) dialog from being presented to the user;
	FALSE to allow it to be presented. This allows a plug-in to control the
	display of this dialog when being created in a Material Editor slot.
	\par Default Implementation:
	<b>{ return FALSE; }</b>\n\n
		*/
	virtual BOOL DontKeepOldMtl() { return FALSE; }
//! @}

//! \name Set Properties Methods
//! @{			
	// allow renderer to attach auxiliary data to each material.
	/*! \remarks			This method allows the renderer to attach auxiliary data to each
	material.
	\param rdata The auxiliary data you wish to attach.
	\par Default Implementation:
	<b>{ renderData = rdata; }</b> */
	virtual void SetRenderData( RenderData *rdata) { renderData = rdata; }
//! @}

//! \name Get Properties Methods
//! @{
	/*! \remarks			This method returns the auxiliary data attached to the material by the
	renderer.
	\par Default Implementation:
	<b>{ return renderData; }</b> */
	virtual RenderData *GetRenderData() { return renderData; }

	// --- Material evaluation ---

	// Returns true if the evaluated color\value (output) is constant 
	// over all possible inputs. 
	/*! \remarks			This method returns TRUE if the evaluated color/value (output) is constant over
	all possible inputs described by the shade context. If it cannot determine the
	correct answer, it returns FALSE.
	\param sc This describes the context of the question.
	\param stdID The ID of the channel in question. 
	See \ref Material_TextureMap_IDs.
	\par Default Implementation:
	<b>{ return FALSE; }</b> */
	virtual bool IsOutputConst( ShadeContext& sc, int stdID ) { return false; }
//! @}

//! \name Evaluation Methods
//! @{
	// Evaluate the material on a single standard texmap channel (ID_AM, etc) 
	// at a UVW coordinated and over an area described in the ShadingContext
	/*! \remarks			This method evaluates the material on a single standard texmap channel over an
	area described in the ShadeContext. A return value of FALSE indicates that the
	color could not be evaluated.\n\n
	If there's no texmap defined for a channel or the output of the texmap is not
	"meaningful", the raw color stored by the material or shader is returned. (The
	output of a texmap is meaningful in a given ShadeContext if it is the same as
	when the scene is rendered. If the map cannot determine whether the output
	value is the same as when rendered, it should not be meaningful.)\n\n
	Note that the output color is not clamped. If the method is called with a
	monochrome channel ID, the result value is copied in the R, G and B components
	of the Color structure.\n\n
	As a default implementation, this method sets the output color to black and
	returns FALSE.
	\param sc This describes the context in which the material should be evaluated.
	\param stdID The ID of the channel to perform evaluation on. 
	See \ref Material_TextureMap_IDs.
	\param outClr The result of the evaluation.
	\par Default Implementation:
	<b>{ return FALSE; }</b> */
	virtual bool EvalColorStdChannel( ShadeContext& sc, int stdID, Color& outClr) { return false; }
	CoreExport virtual bool EvalMonoStdChannel( ShadeContext& sc, int stdID, float& outVal) ;
//! @}

    //! \brief Resolves any wrapper materials and returns the actual material that you should use.
    /* This method is used to get rid of "wrapper" materials, like the XRef material and
    the bake shell material, that usually have no effect on the output of the material.
    This method can be used by renderer or exporter plugins to get to the actual material that
    should be used for rendering or exporting.
    \param forUseInViewport - Set to true to get a material to be used in the viewport, false
        for any other uses. This affects, for example, which sub-material of the bake shell material
        will be used. */
    CoreExport Mtl* ResolveWrapperMaterials(bool forUseInViewport);

};

#pragma warning(pop)

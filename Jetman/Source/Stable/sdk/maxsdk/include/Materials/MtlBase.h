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

#include "../Quat.h"
#include "../Ref.h"
#include "TexHandle.h"
#include "ISubMap.h"

#pragma warning(push)
#pragma warning(disable:4100)

class MtlMakerCallback;
class RenderMapsContext;
class PStamp;
class TexHandleMaker;
class Material;
class Texmap;
class TextureInfo;
class IMtlParams;
class ShadeContext;

/*! \defgroup materialFlags Material Flags
\sa Class MtlBase
*/
//@{
#define MTL_IN_SCENE             (1<<0) //!< The material is being used in the scene.
#define MTL_BEING_EDITED      (1<<1)  //!< The material's parameters are being displayed in the Material Editor.
#define MTL_SUB_BEING_EDITED  (1<<2)  //!< This material OR sub-material texmap is being displayed in the Material Editor. 
#define MTL_TEX_DISPLAY_ENABLED (1<<3)  //!< Viewport display enabled for this material (see MTL_SUB_DISPLAY_ENABLED).
#define MTL_MEDIT_BACKGROUND    (1<<8)  //!< Material has the background shown in Material Editor.
#define MTL_MEDIT_BACKLIGHT      (1<<9)  //!< Material is backlight in the Material Editor.

#define MTL_OBJTYPE_SHIFT     10 //!< 
#define MTL_MEDIT_OBJTYPE     (1<<MTL_OBJTYPE_SHIFT) //!< Object type displayed in Material Editor
#define MTL_MEDIT_OBJTYPE_MASK   ((1<<MTL_OBJTYPE_SHIFT)|(1<<(MTL_OBJTYPE_SHIFT+1))|(1<<(MTL_OBJTYPE_SHIFT+2)))

#define MTL_TILING_SHIFT      13 //!< 
#define MTL_MEDIT_TILING      (1<<MTL_TILING_SHIFT) // Object type displayed in Material Editor.
#define MTL_MEDIT_TILING_MASK ((1<<MTL_TILING_SHIFT)|(1<<(MTL_TILING_SHIFT+1))|(1<<(MTL_TILING_SHIFT+2)))

#define MTL_MEDIT_VIDCHECK    (1<<16) //!< 

#define MTL_BROWSE_OPEN1      (1<<18) //!< For internal use.
#define MTL_BROWSE_OPEN2      (1<<19) //!< For internal use.
#define MTL_SUB_DISPLAY_ENABLED (1<<20)  //!< Indicates that texture display is enabled for map or material in this subtree

#define MTL_CLONED            (1<<28)  //!< Indicates material or texture was created by being cloned from another material or texture. 
#define MTL_HW_MAT_PRESENT    (1<<29)  //!< 
#define MTL_HW_MAT_ENABLED    (1<<30)  //!< 

#define MTL_WORK_FLAG         (1<<31)  //!< 

#define MTL_DISPLAY_ENABLE_FLAGS (MTL_TEX_DISPLAY_ENABLED|MTL_SUB_DISPLAY_ENABLED)  //!<  Interactive texture display enabled for THIS mtl base

#define MTL_HW_TEX_ENABLED    (1<<17)  //!<  Indicates that the texture should be part of the realtime shader
//@}

/*! \defgroup materialRequirementsFlags Material Requirements Flags
\sa Class MtlBase, Class RenderInstance */
//@{
#define MTLREQ_2SIDE          (1<<0)  //!< The material is 2-sided.
#define MTLREQ_WIRE           (1<<1)  //!< The material is wire frame material.
#define MTLREQ_WIRE_ABS       (1<<2)  //!< Wire frame material, absolute size
#define MTLREQ_TRANSP         (1<<3)   //!< The material uses transparency.
#define MTLREQ_UV          (1<<4)  //!< The material requires UVW coordinates.
#define MTLREQ_FACEMAP        (1<<5)  //!< The material uses "face map" UV coordinates.
#define MTLREQ_XYZ            (1<<6)  //!< The material requires object XYZ coordinates.
#define MTLREQ_OXYZ        (1<<7)  //!< The material requires object ORIGINAL XYZ coordinates.
#define MTLREQ_BUMPUV         (1<<8)  //!< The Material requires UV bump vectors.
#define MTLREQ_BGCOL       (1<<9)  //!< The material requires background color (e.g. Matte mtl).
#define MTLREQ_PHONG       (1<<10) //!< The material requires interpolated normal.
#define MTLREQ_AUTOREFLECT       (1<<11) //!< The material needs to build auto-reflect map.
#define MTLREQ_AUTOMIRROR     (1<<12) //!< The material needs to build auto-mirror map.
/*! This is used by the Matte material for example. The matte material samples the background
itself and handles the fogging characteristics, so it does not need to have the atmospheric
shader do this again. */
#define MTLREQ_NOATMOS        (1<<13) //!< This suppresses the atmospheric shader. (used by Matte mtl)
/*! Normally, if this is not specified, the background color is attenuated. For instance, for a
material that 30% transparent (70% opaque), you take 30% of the background color and 70% of the
material color. \n\n
Setting this flag causes the computation to be done by multiplying 0.7 times the
material color and then adding this to the background color, which is left un-attenuated. */
#define MTLREQ_ADDITIVE_TRANSP   (1<<14) //!< Specifies that transparent composites are done additively.
#define MTLREQ_VIEW_DEP       (1<<15) //!< Maps or materials which depend on the view should set this bit in their Requirements() method. 
#define MTLREQ_UV2            (1<<16)  //!< The material requires second uv channel values (vertex colors).
#define MTLREQ_BUMPUV2        (1<<17)  //!< The material requires second uv channel bump vectors.
#define MTLREQ_PREPRO         (1<<18)  //!< Pre-processing. MtlBase::BuildMaps is called on every frame.
#define MTLREQ_DONTMERGE_FRAGMENTS  (1<<19) //!< No longer used.
#define MTLREQ_DISPLACEMAP       (1<<20) //!< Material has a Displacement map channel
/*! This tells the scanline renderer that you want super sampling - the Standard material uses
this. This takes multiple samples spread around in the pixel which provides an additional level
of anti-aliasing. */
#define MTLREQ_SUPERSAMPLE       (1<<21) //!< The material requires supersampling
/*! This flag is set by UVGen and XYZGen when world coordinates are involved. The renderer looks
at this and if it is set, takes the validity interval of the Node's transform matrix into account
in computing the validity of a displacement mapped mesh. */
#define MTLREQ_WORLDCOORDS       (1<<22) //!< The material has world coordinates are used in material/map evaluation.
/*! This flag should be returned true for any material that wants to be transparent in the viewport. */
#define MTLREQ_TRANSP_IN_VP   (1<<23) //!< The material is transparent in the viewport ( opacity < 100, or opacity map & ShowMapInVP set).
#define MTLREQ_FACETED        (1<<24) //!< The material should be rendered faceted in the viewports.

/* JH 5/21/03 Adjusting for R6
#define MTLREQ_2SIDE_SHADOW   (1<<25) // USED IN RENDERER.
#define MTLREQ_REND2          (1<<26) // USED IN RENDERER.
#define MTLREQ_REND3          (1<<27) // USED IN RENDERER.
#define MTLREQ_REND4          (1<<28) // USED IN RENDERER.
#define MTLREQ_REND5          (1<<29) // USED IN RENDERER.
#define MTLREQ_REND6          (1<<30) // USED IN RENDERER.
#define MTLREQ_NOEXPOSURE     (1<<31) // don't do the tone-op (ie, for matte/shadow material, etc)
*/
#define MTLREQ_NOEXPOSURE     (1<<25) //!< Do not do the tone-op (ie, for matte/shadow material, etc)
#define MTLREQ_SS_GLOBAL      (1<<26) //!< Material requires supersampling but use global sampler

#define MTLREQ_REND1          (1<<28) //!< USED IN RENDERER.
#define MTLREQ_REND2          (1<<29) //!< USED IN RENDERER.
#define MTLREQ_REND3          (1<<30) //!< USED IN RENDERER.
#define MTLREQ_REND4          (1<<31) //!< USED IN RENDERER.
//@}

// Base class from which materials and textures are subclassed.
/*! \sa  Class ReferenceTarget, Class ISubMap, Class Mtl,  Class Texmap,  Class ShadeContext, Class RenderMapsContext, Class RenderGlobalContext, Class UVGen, Class XYZGen, Class PStamp,  Class Quat.\n\n
\par Description:
This is the base class from which materials and textures are subclassed.
Methods are provided to access the name, flags, and sub-materials/maps. There
is also a method that is called when the material or texture is to be displayed
in the material editor parameters area.\n\n
Note the following about dialog proc processing of sub-map buttons:\n\n
When you post the message:\n\n
<b>PostMessage(hwmedit, WM_TEXMAP_BUTTON, i,(LPARAM)theMtl);</b>\n\n
You are telling the system that the user clicked on the button for the 'i-th'
sub-map of <b>theMtl</b>. The message doesn't propagate up -- it goes directly
to the materials editor. It then uses calls on <b>theMtl-\>SetSubTexmap()</b>
and <b>theMtl-\>GetSubTexmap()</b> to assign the new map. So even if your
material has some complicated internal hierarchical structure of references, to
the system it is still a simple "logical" hierarchy of a material with some
sub-texmaps.
\par Data Members:
<b>Quat meditRotate;</b>\n\n
This data member is available in release 2.0 and later only.\n\n
This describes the rotation of the sample geometry in the materials editor.\n\n
<b>ULONG gbufID;</b>\n\n
This is the G-Buffer ID of the material or texmap. This is a "effects number"
assigned in the materials editor to a map or material, and it will be written
into the effects channel of the G-Buffer when a pixel with that material on it
is rendered. To implement this, each map or material, in the beginning of its
<b>Shade()</b>, <b>EvalColor()</b>, or <b>EvalMono()</b> methods should have
the code:\n\n
<b>if (gbufID) sc.SetGBufferID(gbufID);</b>
*/
class MtlBase: public ReferenceTarget, public ISubMap 
{
	 friend class Texmap;
	 MSTR name;
	 ULONG mtlFlags;
	 int defaultSlotType;
	 int activeCount;
	 class MtlBaseImplData;
	 const MaxSDK::Util::AutoPtr<MtlBaseImplData> mMtlBaseImplData;
	public:
			Quat meditRotate;
			ULONG gbufID;
			/*! \remarks Constructor. The flags and G-buffer id are initialized. */
			CoreExport MtlBase();
			CoreExport ~MtlBase();
			CoreExport virtual Class_ID ClassID();

//! \name Naming Methods (MtlBase)
//! @{			
			/*! \remarks Implemented by the System.\n\n
			Returns the name of the material or texture. */
			MSTR& GetName() { return name; }
			/*! \remarks Implemented by the System.\n\n
			Stores the name of the material or texture. */
			CoreExport void SetName(MSTR s);

			// Return the "className(instance Name)". 
			// The default implementation should be used in most cases.
			/*! \remarks Implemented by the System.\n\n
			This method returns the name that appears in the track view. It returns
			the "Instance Name(class Name)". For example "Green Glass (Standard)".
			The default implementation should be used. */
			CoreExport virtual MSTR GetFullName();
//! @}

//! \name Flag Access Methods
//! @{	
			/*! \remarks Implemented by the System.\n\n
			Alters the flags, either setting or clearing them, using the mask and
			method passed.
			\param mask The flags to alter. 
			See \ref materialFlags.
			\param val If TRUE the mask is ORed into the flags (<b>mtlFlags |= mask</b>);
			otherwise (<b>mtlFlags \&= ~mask</b>). */
			CoreExport void SetMtlFlag(int mask, BOOL val=TRUE);
			/*! \remarks Implemented by the System.\n\n
			Clears the specified flags.
			\param mask The flags to clear. 
			See \ref materialFlags. */
			CoreExport void ClearMtlFlag(int mask);
			/*! \remarks Implemented by the System.\n\n
			Tests the specified flags. Returns nonzero if the flags are set;
			otherwise zero. See \ref materialFlags.
			\param mask The flags to test. */
			CoreExport int TestMtlFlag(int mask);
//! @}

			// Used internally by materials editor.
			/*! \remarks This method is used internally. */
			int GetMeditObjType() { return (mtlFlags&MTL_MEDIT_OBJTYPE_MASK)>>MTL_OBJTYPE_SHIFT; } 
			/*! \remarks This method is used internally. */
			void SetMeditObjType(int t) { mtlFlags &= ~MTL_MEDIT_OBJTYPE_MASK; mtlFlags |= t<<MTL_OBJTYPE_SHIFT; }

			/*! \remarks This method is used internally. */
			int GetMeditTiling() { return (mtlFlags&MTL_MEDIT_TILING_MASK)>>MTL_TILING_SHIFT; } 
			/*! \remarks This method is used internally. */
			void SetMeditTiling(int t) { mtlFlags &= ~MTL_MEDIT_TILING_MASK; mtlFlags |= t<<MTL_TILING_SHIFT; }

			// recursively determine if there are any multi-materials or texmaps 
			// in tree
			/*! \remarks Implemented by the System.\n\n
			This method may be called to recursively determine if there are any
			multi-materials or texmaps in the tree.
			\return  TRUE if the material or texture map has any mult-materials;
			otherwise FALSE. */
			CoreExport BOOL AnyMulti();

			/*! \remarks This method is used internally.\n\n
				*/
			BOOL TextureDisplayEnabled() { return TestMtlFlag(MTL_TEX_DISPLAY_ENABLED); }

			// Mtls and Texmaps must use this to copy the common portion of 
			// themselves when cloning
			/*! \remarks Implemented by the System.\n\n
			Materials and Texmaps must use this operator to copy the common portion
			of themselves when cloning. */
			CoreExport MtlBase& operator=(const MtlBase& m);

//! \name BuildMaps Method
//! @{
			/*! \remarks This method is called for a plug-in to do any work it needs to do prior
			to rendering. For example this is used by the 3ds Max Mirror and Auto
			Reflect materials to render their bitmaps.
			\param t The current time.
			\param rmc Provides information about the view being rendered and can provide access
			to the global rendering environment information via
			<b>RenderGlobalContext *gc = rmc.GetGlobalContext()</b>. See
			Class RenderMapsContext and Class RenderGlobalContext.
			\return  Nonzero on success; zero on failure. In the case of failure the
			renderer is halted (rendering is cancelled). */
			virtual int BuildMaps(TimeValue t, RenderMapsContext &rmc) { return 1; }
//! @}

//! \name Requirement Methods
//! @{
			// This gives the cumulative requirements of the mtl and its
			// tree. The default just OR's together the local requirements
			// of the Mtl with the requirements of all its children.
			// For most mtls, all they need to implement is LocalRequirements,
			// if any.
			/*! \remarks This method returns the cumulative requirements of the material and its
			sub-materials and maps. The default implementation just ORs together the
			local requirements of the material with the requirements of all its
			children. Most materials will only need to implement
			<b>LocalRequirements()</b>.
			\param subMtlNum Specifies the number of the sub-material whose requirements should be
			returned. <b>-1</b> may be used to return a value generated by looping
			over all the sub-materials and ORing together the requirements.
			\return  See \ref materialRequirementsFlags.
			\par Default Implementation:
			The default implementation automatically traverses the sub-material/map
			tree. On any MtlBase that returns TRUE for <b>IsMultiMtl()</b> it will
			only recursively call the sub material (or map) for the <b>subMtlNum</b>
			called. The exception to this is when <b>subMtlNum\<0</b>: in this case
			all sub-mtls and submaps are enumerated. Therefore the
			<b>LocalRequirements()</b> method below only needs to consider the
			material or map itself, not the sub-mtls and sub-maps. */
			CoreExport virtual ULONG Requirements(int subMtlNum); 

			// Specifies various requirements for the material: Should NOT
			// include requirements of its sub-mtls and sub-maps.
			/*! \remarks Specifies various requirements for <b>this</b> material. The value
			returned from this method should <b>not</b> include requirements of its
			sub-materials and sub-maps.
			\param subMtlNum Specifies the number of the sub-material whose requirements should be returned.
			\return  See \ref materialRequirementsFlags.
			\par Default Implementation:
			<b>{ return 0; }</b> */
			virtual ULONG LocalRequirements(int subMtlNum) { return 0; } 

			// This gives the UVW channel requirements of the mtl and its
			// tree. The default implementation just OR's together the local mapping requirements
			// of the Mtl with the requirements of all its children.
			// For most mtls, all they need to implement is LocalMappingsRequired, if any.
			// mapreq and bumpreq will be initialized to empty sets with MAX_MESHMAPS elements
			// set 1's in mapreq for uvw channels required
			// set 1's in bumpreq for bump mapping channels required
			/*! \remarks			This method gives the UVW channel requirements of the material and its
			tree. The default implementation just OR's together the local mapping
			requirements of the material with the requirements of all its children.
			For most materials, all they need to implement is the
			<b>LocalMappingsRequired()</b> method.
			\param subMtlNum Specifies the number of the sub-material whose mapping information is retrieved.
			\param mapreq This array of bits is initialized to an empty set with MAX_MESHMAPS
			elements. Each bit corresponds to a mapping channel. Set a bit to one to
			indicate the material requires the corresponding UVW channel.
			\param bumpreq This array of bits is initialized to an empty set with MAX_MESHMAPS
			elements. Each bit corresponds to a mapping channel. Set a bit to one to
			indicate the material requires the corresponding bump mapping channel. */
			CoreExport virtual void MappingsRequired(int subMtlNum, BitArray & mapreq, BitArray &bumpreq);

			// Specifies UVW channel requirements for the material: Should NOT
			// include UVW channel requirements of its sub-mtls and sub-maps.
			/*! \remarks			This method specifies UVW channel requirements for the material: This
			method should <b>not</b> include UVW channel requirements of any
			sub-materials and sub-maps.
			\param subMtlNum Specifies the number of the sub-material whose mapping information is retrieved.
			\param mapreq This array of bits is initialized to an empty set with MAX_MESHMAPS
			elements. Each bit corresponds to a mapping channel. Set a bit to one to
			indicate the material requires the corresponding UVW channel.
			\param bumpreq This array of bits is initialized to an empty set with MAX_MESHMAPS
			elements. Each bit corresponds to a mapping channel. Set a bit to one to
			indicate the material requires the corresponding bump mapping channel.
			\par Default Implementation:
			<b>{}</b>
			\par Sample Code:
			All 2D textures that use UVGen or otherwise select mapping channels need
			to implement this method Here's an example:\n\n
			\code	
			void LocalMappingsRequired(int subMtlNum, BitArray & mapreq, BitArray &bumpreq)
			{
				uvGen->MappingsRequired(subMtlNum,mapreq,bumpreq);
			}
			\endcode
			All 3D textures that use the XYZGen to put up a coordinates rollup must
			implement this method. Here's an example:
			\code
			void LocalMappingsRequired(int subMtlNum, BitArray & mapreq,BitArray &bumpreq)
			{
				xyzGen->MappingsRequired(subMtlNum,mapreq,bumpreq);
			}
			\endcode */
			virtual void LocalMappingsRequired(int subMtlNum, BitArray & mapreq, BitArray &bumpreq) {  }
//! @}

//! \name Methods to Access Sub Texture Maps of Materials or Texmaps
//! @{			
			// This returns true for materials or texmaps that select sub-
			// materials based on mesh faceMtlIndex. Used in 
			// interactive render.
			/*! \remarks This method returns TRUE for materials or textures that select
			sub-materials based on submaterial number (for example a mesh
			<b>faceMtlIndex</b>).\n\n
			The only materials for which this method should return TRUE are those
			that choose to use one of their sub-maps or sub-mtls based on the
			submaterial number. For the majority of materials and maps, they should
			return FALSE, and in that case all of the submaterials and maps are
			enumerated by <b>MtlBase::Requirements()</b>.
			\par Default Implementation:
			<b>{ return FALSE; }</b> */
			virtual  BOOL IsMultiMtl() { return FALSE; }

			// Methods to access sub texture maps of material or texmap
			virtual int MapSlotType(int i) { return defaultSlotType; }

			// This must be called on a sub-Mtl or sub-Map when it is removed,
			// in case it or any of its submaps are active in the viewport.
			/*! \remarks Implemented by the System.\n\n
			This method must be called on a sub-material or sub-map when it is
			removed, in case it or any of its sub-maps are active in the viewport. */
			CoreExport void DeactivateMapsInTree();
//! @}

//! \name Update/Reset/Validity
//! @{			
			// To make texture & material evaluation more efficient, this function is
			// called whenever time has changed.  It will be called at the
			// beginning of every frame during rendering.
			/*! \remarks A material has a <b>Shade()</b> method, and a texture map has a
			<b>EvalColor()</b> method. These are called by the renderer for every
			pixel. This method is called before rendering begins to allow the plug-in
			to evaluate anything prior to the render so it can store this
			information. In this way this information does not need to be calculated
			over and over for every pixel when <b>Shade()</b> or <b>EvalColor()</b>
			is called. This allows texture and material evaluation to be more
			efficient. It is generally called once at the start of each frame, or
			during interactive playback when the time changes. It is <b>not</b>
			guaranteed to get called on every frame, because if you returned a
			validity interval that covers several frames, your parent material won't
			bother to call you if the current frame is still in that interval.
			\param t The current time.
			\param valid The validity interval to update to reflect the validity interval of the
			material or texture at the time passed. */
			virtual  void Update(TimeValue t, Interval& valid)=0;

			/*! \remarks This method is called to reset the material or texmap back to its default
			values. This should be done by calling the Reset method of the material or 
			texture map plugin's class descriptor (ClassDesc2::Reset). In addition, the 
			plugin can execute any other initialization code it has. 
			Example from maxsdk\\samples\\materials\\dblsided.cpp
			\code
			void DoubleSided::Reset() {
				dblsidedCD.Reset(this, TRUE); // Resets all pb2's
				Init();
			} \endcode
			\note When migrating plugins from ClassDesc to ClassDesc2 and IParamBlock2,
			do not forget to update this method to call the class descriptor's Reset method.
			If the plugin's Reset tries to replace parameter block objects based on
			IParamBlock2 that use the P_AUTO_UI flag, the UI code will continue to hold 
			a pointer to the replaced parameter block, and this will lead to problems.
			*/
			virtual void Reset()=0;

			// call this to determine the validity interval of the mtl or texture
			/*! \remarks This method is called to determine the validity interval of the material
			or texture.
			\param t Specifies the time about which to compute the validity interval.
			\return  The validity interval of the item at the specified time. */
			virtual Interval Validity(TimeValue t)=0;
//! @}

//! \name User Interface Method
//! @{			
			// this gets called when the mtl or texture is to be displayed in the
			// mtl editor params area.
			/*! \remarks This method gets called when the material or texture is to be displayed
			in the material editor parameters area. The plug-in should allocate a new
			instance of a class derived from <b>ParamDlg</b> to manage the user
			interface.\n\n
			<b>Note:</b> The following is a discussion of this
			<b>CreateParamDlg()</b> method, and the <b>SetDlgThing()</b> method, and
			the way they relate to the ParamMap2 system. A good example for this
			discussion is a texture map which typically has several rollouts, say its
			texmap parameters, a UVW coordinates rollout, and an Output rollout.\n\n
			The normal way for a texmap to implement these other (common) rollouts is
			for the it to create a UVGen instance and a TextureOutput instance as
			'sub-objects' in the map and then ask them to put up their rollouts when
			it is asked to do so by the Materials Editor (in CreateParamDlg()). The
			Materials Editor requires a ParamDlg pointer back from the
			CreateParamDlg() on which it calls methods to control the UI, such as
			time change updates or loading up a new texmap of the same class into the
			UI when the user switches them, so that the whole UI doesn't need to be
			rebuilt.\n\n
			Prior to the ParamMap2 system, a texmap would implement its own ParamDlg
			subclass and would keep track of the UVGen and TextureOutput ParamDialogs
			and pass on any time change or SetThing() calls to them. ParamMap2
			introduced its own ParamDlg subclass
			(Class IAutoMParamDlg) that
			you can ask it to build for you and have manage all interaction with the
			Materials Editor automatically. As before, this still needs to know about
			the other (sub-object) ParamDlgs, and so it has the ability to keep a
			list of 'secondary' ParamDlgs to which it passes on the SetTime()s and
			SetThing()s.\n\n
			When the Materials Editor asks the texmap to CreateParamDlg(), the texmap
			asks its ClassDesc2 to build one of these
			(ClassDesc2::CreateParamDlgs()). If the texmap itself has multiple
			ParamBlock2s, CreateParamDlgs() builds one ParamDlg per pblock/rollout,
			makes the first of them a 'master' and adds the rest as secondary
			ParamDlgs. The texmap then asks the UVGen and TextureOutput objects to
			CreateParamDlg() for their rollouts and adds them to the master
			IAutoMParamDlg also.\n\n
			Now consider the need for the <b>SetDlgThing()</b> method below. It is
			related to the SetThing() method that the Materials Editor calls on the
			'master' ParamDlg to switch into the UI a texmap of the same class as
			that currently in the UI. Normally, the master IAutoParamDlg would
			propagate the SetThing() to its registered secondary ParamDlgs. In the
			case of multiple paramblock2s in the texmap, this would be correct, since
			the 'thing' in this case is the incoming texmap. But this doesn't work
			for the secondary UVGen and TextureOutput ParamDlgs; their 'things' are
			the UVGen and TextureOutput subobjects of the incoming map. So,
			IAutoParamDlg first calls SetDlgThing() on the incoming texmap so that it
			gets a chance to call the correct SetThing()s on the sub-object ParamDlgs
			with the appropriate incoming sub-objects. A clear example of this is in
			<b>Gradient::SetDlgThing()</b> in
			<b>/MAXSDK/SAMPLES/MATERIALS/GRADIENT.CPP</b>. It is called once for each
			secondary ParamDlg. For those ParamDlgs that have special SetThing()
			requirements, it does the appropriate sub-object SetThing() and returns
			TRUE. If it does no special handling for a particular ParamDlg, it
			returns FALSE, signalling to the IAutoMParamDlg that it should do the
			standard SetThing() propagation for that dialog.\n\n
			The Render Effects dialog has a similar arrangement to the Materials Editor
			for controlling Effect UI and so there is an <b>IAutoEParamDlg</b> that
			works exactly the same way as the <b>IAuotMParamDlg</b>.
			\param hwMtlEdit The window handle of the materials editor.
			\param imp The interface pointer for calling methods in 3ds Max.
			\return  A pointer to the created instance of a class derived from
			<b>ParamDlg</b>. */
			virtual ParamDlg* CreateParamDlg(HWND hwMtlEdit, IMtlParams *imp)=0;
//! @}

//! \name Loading and Saving Methods
//! @{
			// save the common mtlbase stuff.
			// these must be called in a chunk at the beginning of every mtl and
			// texmap.
			/*! \remarks Implemented by the System.\n\n
			This method saves the plug-in's data to disk.. The common MtlBase data
			must be saved as well. The base class method must be called in a chunk at
			the beginning of every Mtl and Texmap.
			\param isave An interface pointer available for saving data. See Class ISave, \ref ioResults.
			\par Sample Code:
			Note in the code below the base class method is called in a chunk before
			the rest of the plug-ins data is saved.\n
			\code
			IOResult Gradient::Save(ISave *isave)
			{
				IOResult res;
				// Save common stuff
				isave->BeginChunk(MTL_HDR_CHUNK);
				res = MtlBase::Save(isave);
				if (res!=IO_OK) return res;
				isave->EndChunk();
			
				for (int i=0; i<NSUBTEX; i++) {
					if (mapOn[i]==0) {
						isave->BeginChunk(MAPOFF_CHUNK+i);
						isave->EndChunk();
					}
				}
				return IO_OK;
			}
			\endcode */
			CoreExport IOResult Save(ISave *isave);
			/*! \remarks Implemented by the System.\n\n
			This method is called to load the material or texture from disk. The
			common MtlBase data must be loaded as well. See the code below.
			\param iload An interface pointer for calling methods to load data. See Class ILoad, \ref ioResults.
			\par Sample Code:
			\code
			IOResult Gradient::Load(ILoad *iload)
			{
				IOResult res;
				int id;
				while (IO_OK==(res=iload->OpenChunk())) {
					switch(id = iload->CurChunkID()) {
						case MTL_HDR_CHUNK:
							res = MtlBase::Load(iload);
							break;
						case MAPOFF_CHUNK+0:
						case MAPOFF_CHUNK+1:
						case MAPOFF_CHUNK+2:
							mapOn[id-MAPOFF_CHUNK] = 0;
							break;
					}
					iload->CloseChunk();
					if (res!=IO_OK)
						return res;
				}
				iload->RegisterPostLoadCallback(new ParamBlockPLCB(versions,NUM_OLDVERSIONS,&curVersion,this,1));
				return IO_OK;
			}
			\endcode */
			CoreExport IOResult Load(ILoad *iload);
//! @}

//! \name GBuffer (System) Methods
//! @{	
			// GBuffer functions
			/*! \remarks Implemented by the System.\n\n
			Returns the G-buffer ID of this material. */
			ULONG GetGBufID() { return gbufID; }
			/*! \remarks Implemented by the System.\n\n
			Sets the G-buffer ID of this material. */
			void SetGBufID(ULONG id) { gbufID = id; }
//! @}
			
//! \name Enumerate Auxiliary Files Implementation
//! @{			
			// Default File enumerator.
			/*! \remarks			This is an implementation of the <b>Animatable</b> method. This default
			implementation simply recurses, skipping inactive subTexmaps if
			appropriate. */
			CoreExport void EnumAuxFiles(AssetEnumCallback& nameEnum, DWORD flags);
//! @}

//! \name Postage Stamp Image Methods
//! @{			
			// Postage Stamp
			/*! \remarks			Implemented by the System.\n\n
			Returns a pointer to the postage stamp image for the file.
			\param sz One of the following values:\n\n
			<b>PS_SMALL</b> for small (32x32) images.\n
			<b>PS_LARGE</b> for large (88x88) images.\n
			<b>PS_TINY</b> for tiny (24x24) images. */
			CoreExport virtual PStamp* GetPStamp(int sz); // sz = 0: small(32x32), 1: large(88x88), 2: tiny(24x24)      
			/*! \remarks			Implemented by the System.\n\n
			Creates a postage stamp image and returns a pointer to it. If the postage
			stamp image already exists then it is simply returned.\n\n
			Here's an example using this method to display a small material	sample.\n\n
			\code
			void DisplayMB(MtlBase *mb, HDC hdc, int x, int y) {
				mb->CreatePStamp(0,TRUE); // create and render a small pstamp
				PStamp *ps = mb->GetPStamp(0);
				if (ps) {
					int d = PSDIM(0);
					int scanw = ByteWidth(d);
					int nb = scanw*d;
					UBYTE *workImg = new UBYTE[nb];
					if (NULL == workImg)
						return;
					ps->GetImage(workImg);
					Rect rect;
					rect.left = x;
					rect.top = y;
					rect.right = x+d;
					rect.bottom = y+d;
					GetGPort()->DisplayMap(hdc, rect,0,0, workImg, scanw);
					delete [] workImg;
				}
			}
			\endcode
			\param sz One of the following values:\n\n
			<b>PS_SMALL</b> for small (32x32) images.\n
			<b>PS_LARGE</b> for large (88x88) images.\n
			<b>PS_TINY</b> for tiny (24x24) images.
			\param Render If set to true, the postage stamp bitmap will have the <b>MtlBase</b>
			rendered into it automatically. The bitmap can then be retrieved using
			<b>PStamp::GetImage</b>, for drawing in the UI. */
			CoreExport virtual PStamp* CreatePStamp(int sz, BOOL Render = FALSE);         
			/*! \remarks			Implemented by the System.\n\n
			Discards the postage stamp image.
			\param sz One of the following values:\n\n
			<b>PS_SMALL</b> for small (32x32) images.\n
			<b>PS_LARGE</b> for large (88x88) images.\n
			<b>PS_TINY</b> for tiny (24x24) images. */
			CoreExport virtual void DiscardPStamp(int sz); 
//! @}

			// Schematic View Animatable Overrides...
			CoreExport SvGraphNodeReference SvTraverseAnimGraph(IGraphObjectManager *gom, Animatable *owner, int id, DWORD flags);
			CoreExport bool SvHandleDoubleClick(IGraphObjectManager *gom, IGraphNode *gNode);
			CoreExport MSTR SvGetName(IGraphObjectManager *gom, IGraphNode *gNode, bool isBeingEdited);
			CoreExport bool SvCanSetName(IGraphObjectManager *gom, IGraphNode *gNode);
			CoreExport bool SvSetName(IGraphObjectManager *gom, IGraphNode *gNode, const MSTR &name);
			CoreExport COLORREF SvHighlightColor(IGraphObjectManager *gom, IGraphNode *gNode);
			CoreExport bool SvIsSelected(IGraphObjectManager *gom, IGraphNode *gNode);
			CoreExport MultiSelectCallback* SvGetMultiSelectCallback(IGraphObjectManager *gom, IGraphNode *gNode);
			CoreExport bool SvCanSelect(IGraphObjectManager *gom, IGraphNode *gNode);

//! \name Interactive Texture Display 
//! @{			
			// Methods for doing interactive texture display
			/*! \remarks Returns TRUE if this texture supports being used in the interactive
			renderer; otherwise FALSE. If the texture does return TRUE it is expected
			to implement the methods <b>ActivateTexDisplay()</b> and
			<b>GetActiveTexHandle()</b>.
			\par Default Implementation:
			<b>{ return FALSE; }</b> */
			virtual BOOL SupportTexDisplay() { return FALSE; }   // supports map display in viewport
			/*! \remarks This method is called to retrieve a texture handle to this texture map.
			\param t The time to return the texture handle.
			\param thmaker This class provides methods for creating a texture handle from a 3ds Max bitmap
			and a Windows DIB. It also has a method to retrieve the required size of the
			texture map. See Class TexHandleMaker.
			\return  The texture handle.
			\par Default Implementation:
			<b>{return 0;}</b> */
			virtual DWORD_PTR GetActiveTexHandle(TimeValue t, TexHandleMaker& thmaker) {return 0;}
			/*! \remarks This method is used internally. */
			CoreExport void IncrActive();
			/*! \remarks This method is used internally. */
			CoreExport void DecrActive(); 
			/*! \remarks This method is used internally. */
			int Active() { return activeCount; }
			/*! \remarks This method is called when the usage of the texture the interactive
			renderer changes. This method must only be implemented if
			<b>SupportTexDisplay()</b> returns TRUE. This method does not cause the
			texture map to be drawn in the viewport but should be called with TRUE as
			the argument before this can occur. For viewport drawing of textures
			refer to <b>Interface::ActivateTexture()</b> and
			<b>Interface::DeActivateTexture()</b> instead.
			\param onoff TRUE if the texture is being used; FALSE if it is no longer being used.
			\par Default Implementation:
			<b>{}</b> */
			virtual void ActivateTexDisplay(BOOL onoff) {}
//! @}

			//! Get the active MtlBase
			/*! Previous to r9, activeMB was held as a raw pointer. Due to problems where the
			// MtlBase was deleted without clearing this pointer, activeMB is now held indirectly. 
			// Methods GetActiveMB and SetActiveMB are used to get/set the activeMB;
			\return The active MtlBase*/
			CoreExport MtlBase* GetActiveMB();
			//! Set the active MtlBase
			/*! \param[in] activeMB The active MtlBase*/
			CoreExport void SetActiveMB(MtlBase* activeMB);

//! \name Multiple Map Display in the Viewports
//! @{			
			// Multiple textures in viewports:
			/*! \remarks						Returns TRUE if this material supports the display of multi-maps in the
			viewports (interactive renderer); FALSE if it doesn't.
			\par Default Implementation:
			<b>{ return FALSE; }</b> */
			virtual BOOL SupportsMultiMapsInViewport() { return FALSE; }
			/*! \remarks						This method is called to initialize the interactive renderer Material
			passed with the properties of this MtlBase.\n\n
			If a <b>MtlBase</b> (material or texmap) wants to display multiple
			textures in the viewports, it implements\n\n
			<b>SupportsMultiMapsInViewport()</b> to return TRUE, and implements
			<b>SetupGfxMultiMaps</b> to store the necessary information in the
			Material passed in, including the <b>TextureInfo</b>'s for each
			texture.\n\n
			The <b>MtlMakerCallback</b> passed in to <b>SetupGfxMultiMaps</b>
			provides functions to help in setting up the "Material" data structure.
			The function <b>NumberTexturesSupported()</b> lets you know the
			capabilities of the current hardware, so you can adapt accordingly. The
			function <b>GetGfxTexInfoFromTexmap</b> fills in the fields of a
			<b>TextureInfo</b> except the texHandle and texture ops.\n\n
			The implementation of <b>SetupGfxMultiMaps</b> must create the
			"texHandle" for each of the textures described in its <b>TextureInfo</b>
			array. It typically does this by calling the submap's
			<b>GetVPDisplayDIB()</b> method, and then creates the texHandle by
			calling the callBack function <b>MakeHandle(bmi)</b>. To avoid doing this
			calculation when not necessary it is best to save the texHandles along
			with their validity intervals. Then when <b>SetupGfxMultiMaps</b> is
			called, if valid texHandles are already in hand they can just be used
			without recomputing.
			\param t The time at which to evaluate the material.
			\param mtl Points to the interactive renderer material to update.
			\param cb This callback object is provided as a helper to fill in the Material
			properties above. See Class MtlMakerCallback.
			\par Default Implementation:
			<b>{}</b> */
			virtual void SetupGfxMultiMaps(TimeValue t, Material *mtl, MtlMakerCallback &cb) {}
//! @}

			CoreExport virtual void* GetInterface(ULONG id);
			
            virtual BaseInterface *GetInterface(Interface_ID id) { return ReferenceTarget::GetInterface(id); }
			virtual ReferenceTarget *GetRefTarget(){return this;}

			// Animatable Properties
			CoreExport virtual int SetProperty(ULONG id, void *data);
			CoreExport virtual void *GetProperty(ULONG id);
			CoreExport virtual void BaseClone(ReferenceTarget *from, ReferenceTarget *to, RemapDir &remap);

			/*! Transparency hint indicates whether the material is potentially transparent for both rendering
			and viewport display, so that the renderers or viewport can decide whether to optimize it or not.
			\param t The time to get the transparency hint at.
			\param valid The validity interval of the returned value.
			\return Nonzero if this material is potentially transparent; Zero if it isn't.
			\par Default Implementation:
			<b>{ return TRUE; }</b> */
			virtual BOOL GetTransparencyHint(TimeValue t, Interval& valid) { return TRUE; }
};


// Callback used with the new multi-texture interface by function SetGfxMtl. 
/*! \sa  Class TexHandleMaker, Class TextureInfo, Class MtlBase, Class Texmap, Class Material.\n\n
\par Description:
This is the callback used with the new multi-texture interface provided by
method <b>MtlBase::SetupGfxMultiMaps()</b>.  */
class MtlMakerCallback: public TexHandleMaker {
	 public:
	 // This fills all the fields of texinf exept the texhandle and the texture ops.
	 // note TexInfo is defined in mlt.h
	 /*! \remarks This method updates all the fields of the TextureInfo instance
	 exept the texture handle and the texture ops.
	 \par Parameters:
	 <b>TimeValue t</b>\n\n
	 The time at which the texture is evaluated.\n\n
	 <b>TextureInfo\& texinf</b>\n\n
	 The texture info which is updated.\n\n
	 <b>Texmap *txm</b>\n\n
	 Points to the texmap as the source for the texture info update. */
	 virtual void GetGfxTexInfoFromTexmap(TimeValue t, TextureInfo& texinf, Texmap *txm)=0;
	 /*! \remarks This method returns the number of textures that the
	 hardware+driver supports. */
	 virtual BOOL NumberTexturesSupported()=0; // how many textures does the hardware+driver support
};

#pragma warning(pop)

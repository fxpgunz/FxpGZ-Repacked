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

#pragma warning(push)
#pragma warning(disable:4100)

class MapSampler;

#define AXIS_UV 0
#define AXIS_VW 1
#define AXIS_WU 2

// This class generates UV coordinates based on the results of a UV 
// Source and user specified transformation.
// A reference to one of these is referenced by all 2D texture maps.
/*! \sa  Class MtlBase, 
Class ShadeContext, Class MapSampler, Class Point2,  Class Point3,  Class Matrix3.\n\n
\par Description:
Most texture maps that use UV coordinates will use an instance of this class.
This class encapsulates much of the user interface functionality for setting
mirroring, tiling and so on. These are the settings found in the 'Coordinate'
and 'Noise' rollup pages. This class generates UV coordinates based on the
results of a UV source provided by the plug-in and the user specified
transformations. An instance of this class is referenced by all the 2D texture
maps. All methods of this class are implemented by the system. <br>  The
following four methods are the only ones a plug-in needs to call typically: */
class UVGen: public MtlBase {
	 public:
			// Get texture coords and derivatives for antialiasing
			/*! \remarks This method is not normally called directly. It retrieves
			the UV coordinates (depending on if they are explicit, shrinked-wrapped
			environment, etc.) and transforms them using the offsets and noise
			parameters entered by the user. It does not do the mirroring or tiling.
			It then returns the texture coordinates and derivatives for antialiasing.
			\par Parameters:
			<b>ShadeContext\& sc</b>\n\n
			Describes the properties of the point to evaluate.\n\n
			<b>Point2\& UV</b>\n\n
			The UV texture coordinates for the point.\n\n
			<b>Point2\& dUV</b>\n\n
			The derivatives of UV for the point to shade. This describes how big of a
			range in UV space the current pixel covers. */
			virtual void GetUV( ShadeContext& sc, Point2& UV, Point2& dUV)=0;
			
			// This is how a Texmap evaluates itself
			/*! \remarks This method is called to evaluate the color of the map
			based on the ShadeContext. It takes into account the users selections for
			mirroring, tiling, noise, etc. from the UVGen rollups.
			\par Parameters:
			<b>ShadeContext \&sc</b>\n\n
			Describes the properties of the point to evaluate.\n\n
			<b>MapSampler* samp</b>\n\n
			This is a callback used to sample the map. The plug-in creates an
			instance of this class, and implements its methods. A pointer to the
			instance is passed here. The methods of the class sample the texture for
			a single UV coordinate.\n\n
			<b>BOOL filter=TRUE</b>\n\n
			If TRUE the texture will be filtered.
			\return  The result as an AColor. */
			virtual AColor EvalUVMap(ShadeContext &sc, MapSampler* samp,  BOOL filter=TRUE)=0;
			/*! \remarks This method is called to evaluate the value of the map
			based on the ShadeContext. It takes into account the users selections for
			mirroring, tiling, noise, etc. from the UVGen rollups.
			\par Parameters:
			<b>ShadeContext \&sc</b>\n\n
			Describes the properties of the point to evaluate.\n\n
			<b>MapSampler* samp</b>\n\n
			This is a callback used to sample the map. The plug-in creates an
			instance of this class, and implements its methods. A pointer to the
			instance is passed here. The methods of the class sample the texture for
			a single UV coordinate.\n\n
			<b>BOOL filter=TRUE</b>\n\n
			If TRUE the texture will be filtered.
			\return  The result as a float.\n\n
			The following two methods are called for bump mapping to retrieve a
			normal perturbation. The calling sequence for these methods is shown in
			the sample code below. This code takes the sum of the U derivative times
			the U bump vector, and the V derivative times the V bump vector. The
			result is the normal perturbation returned from EvalNormalPerturb().\n
			\code
			Point3 BMTex::EvalNormalPerturb(ShadeContext& sc)
			{
				Point3 dPdu, dPdv;
				if (thebm==NULL)
					return Point3(0,0,0);
				uvGen->GetBumpDP(sc,dPdu,dPdv);				  // get bump basis vectors
				Point2 dM =(.01f)*uvGen->EvalDeriv(sc,&mysamp,filterType!=FILTER_NADA);
				return texout->Filter(dM.x*dPdu+dM.y*dPdv);
			}
			\endcode 
			Note: In the following code you'll see the 'text book' Blinn's algorithm
			and the 'Lazy' approach that is actually used. During testing it was
			determined that these two methods are visually identical. The 'Lazy'
			algorithm avoids the cross products however.
			\code
			Point3 Gradient::EvalNormalPerturb(ShadeContext\& sc) {
				Point3 dPdu, dPdv;
				Point2 dM = uvGen->EvalDeriv(sc,\&mysamp);
				uvGen->GetBumpDP(sc,dPdu,dPdv);
				// Lazy algorithm
				return texout->Filter(dM.x*dPdu+dM.y*dPdv);
			} 
			\endcode */
			virtual float EvalUVMapMono(ShadeContext &sc, MapSampler* samp, BOOL filter=TRUE)=0;
			/*! \remarks This gets the amount that U and V are changing. It takes
			into account the users selections for mirroring, tiling, noise, etc. from
			the UVGen rollups.
			\par Parameters:
			<b>ShadeContext\& sc</b>\n\n
			Describes the properties of the point to evaluate.\n\n
			<b>MapSampler* samp</b>\n\n
			This is a callback used to sample the map. The plug-in creates an
			instance of this class, and implements its methods. A pointer to the
			instance is passed here. The methods of the class sample the texture for
			a single UV coordinate.\n\n
			<b>BOOL filter=TRUE</b>\n\n
			If TRUE the texture will be filtered.
			\return  <b>df/du</b>, <b>df/dv</b> as a Point2. */
			virtual  Point2 EvalDeriv( ShadeContext& sc, MapSampler* samp, BOOL filter=TRUE)=0;

			// Get dPdu and dPdv for bump mapping
			/*! \remarks This method may be called to compute <b>dPdu</b> and
			<b>dPdv</b> bump basis vectors for bump mapping. These are two vector in
			3D space that give you the U and V axis of the map space. This is like
			taking the map space into 3D space so you can determine where the U axis
			is and where the V axis is.\n\n
			Note: In 3ds Max 3.0 and later these vectors are always normalized. This
			change makes it so bump mapping is invariant when a Rescale World Units
			is performed.\n\n
			See the Advanced Topics section on
			~{ Materials, Textures and Maps }~ for more information on bump mapping.
			\par Parameters:
			<b>ShadeContext\& sc</b>\n\n
			Describes the properties of the point to evaluate.\n\n
			<b>Point3\& dPdu, Point3\& dPdv</b>\n\n
			The bump basis vectors are returned here. */
			virtual  void GetBumpDP( ShadeContext& sc, Point3& dPdu, Point3& dPdv)=0;

			/*! \remarks This method is used internally to provide information to
			the interactive renderer. */
			virtual void GetUVTransform(Matrix3 &uvtrans)=0;

			/*! \remarks This method is used internally to provide information to
			the interactive renderer. */
			virtual int GetTextureTiling()=0;
			virtual void SetTextureTiling(int t)=0;

			/*! \remarks This method returns the source for the texture vertices.
			\return  One of the following values:\n\n
			<b>UVWSRC_EXPLICIT</b>\n\n
			Use explicit mesh texture vertices from channel 1.\n\n
			<b>UVWSRC_EXPLICIT2</b>\n\n
			Use explicit mesh texture vertices from channel 2.\n\n
			<b>UVWSRC_OBJXYZ</b>\n\n
			Generate planar UVW mapping coordinates from the object XYZ on-the-fly.
			*/
			virtual int GetUVWSource()=0;
			virtual int GetMapChannel () { return 1; }   // only relevant if above returns UVWSRC_EXPLICIT
			/*! \remarks This method sets the source for the texture vertices.
			\par Parameters:
			<b>int s</b>\n\n
			One of the following values:\n\n
			<b>UVWSRC_EXPLICIT</b>\n\n
			Use explicit mesh texture vertices from one of the mapping channels (as
			established by <b>SetMapChannel()</b> below).\n\n
			<b>UVWSRC_EXPLICIT2</b>\n\n
			Use explicit mesh texture vertices from the Vertex Color Channel.\n\n
			<b>UVWSRC_OBJXYZ</b>\n\n
			Generate planar UVW mapping coordinates from the object XYZ
			on-the-fly.\n\n
			<b>UVWSRC_WORLDXYZ</b>\n\n
			This parameter is available in release 3.0 and later only.\n\n
			Generate planar uvw mapping from world xyz on-the-fly. */
			virtual void SetUVWSource(int s)=0;
			/*! \remarks			Sets the mapping channel used when <b>UVWSRC_EXPLICIT</b> is in effect.
			\par Parameters:
			<b>int s</b>\n\n
			The mapping channel value to set.
			\par Default Implementation:
			<b>{}</b> */
			virtual void SetMapChannel (int s) { }

			/*! \remarks This method is used internally. */
			virtual int SymFlags()=0;
			/*! \remarks			This method is used internally. */
			virtual void SetSymFlags(int f)=0;

			virtual int GetSlotType()=0; 
			/*! \remarks Developers typically don't need to call this method. In the
			Coordinates rollup in the user interface for a texture map are two
			options. These options are Texture or Environment. The slot type is one
			of these two options. There are a variety of texture coordinate types.
			There are the type assigned to the object and the environment type
			(Spherical, Cylindrical, Shrink-wrap, Screen). This method is used
			initialize the radio button in the dialog based on the <b>sType</b>
			passed and update the drop down list.
			\par Parameters:
			<b>int sType</b>\n\n
			One of the following values:\n\n
			<b>MAPSLOT_TEXTURE</b>\n\n
			This is either texture coordinates.\n\n
			<b>MAPSLOT_ENVIRON</b>\n\n
			Environment coordinates. */
			virtual void InitSlotType(int sType)=0; 

			/*! \remarks			Return the axis (mapping coordinate system) used.
			\return  One of the following values:\n\n
			<b>AXIS_UV</b>\n\n
			<b>AXIS_VW</b>\n\n
			<b>AXIS_WU</b> */
			virtual int GetAxis()=0;  // returns AXIS_UV, AXIS_VW, AXIS_WU
			/*! \remarks			Sets the axis (mapping coordinate system) used.
			\par Parameters:
			<b>int ax</b>\n\n
			One of the following values:\n\n
			<b>AXIS_UV</b>\n\n
			<b>AXIS_VW</b>\n\n
			<b>AXIS_WU</b> */
			virtual void SetAxis(int ax)=0;  //  AXIS_UV, AXIS_VW, AXIS_WU

			// The clip flag controls whether the U,V values passed to MapSampler by 
			//  EvalUVMap and EvalUVMapMono are clipped to the [0..1] interval or not. 
			// It defaults to ON (i.e., clipped).
			/*! \remarks			This method sets the clip flag. The clip flag controls whether the U,V
			values passed to <b>MapSampler</b> by <b>EvalUVMap()</b> and
			<b>EvalUVMapMono()</b> are clipped to the [0..1] interval or not. It
			defaults to ON (i.e., clipped).\n\n
			If an object is covered by a texture, and that texture repeats (is
			tiled), this methods can be used to tell you which tile you're in. This
			allows one to do something depending on which tile it is. This is done,
			for example, in the Bitmap texture, when 'Jitter Placement' is on, so
			each tile can be moved about slightly. For an example of this see
			<b>/MAXSDK/SAMPLES/MATERIALS/BMTEX.CPP</b>. If you need this information,
			you can just call <b>uvGen-\>SetClipFlag(FALSE)</b> (for instance in your
			<b>Update()</b> method) and then use <b>int(u)</b> and <b>int(v)</b> to
			get the this info. (You'd use <b>frac(u)</b> and <b>frac(v)</b> to get
			clipped versions of u,v. )
			\par Parameters:
			<b>BOOL b</b>\n\n
			TRUE to clip; FALSE to not clip. */
			virtual void SetClipFlag(BOOL b)=0; 
			/*! \remarks			This method returns the clip flag. The clip flag controls whether the U,V
			values passed to <b>MapSampler</b> by <b>EvalUVMap()</b> and
			<b>EvalUVMapMono()</b> are clipped to the [0..1] interval or not. It
			defaults to ON (i.e., clipped). See <b>SetClipFlag()</b> above.
			\return  TRUE for clipped; FALSE if not clipped. */
			virtual BOOL GetClipFlag()=0;
			/*! \remarks			This method asks the question 'Is this class an instance of <b>StdUVGen</b>?'.
			The derived class <b>StdUVGen</b> implements this to return TRUE. Others use
			the default implementation to return FALSE.
			\par Default Implementation:
			<b>{ return FALSE; }</b> */
			virtual BOOL IsStdUVGen() { return FALSE; } // class StdUVGen implements this to return TRUE

			SClass_ID SuperClassID() { return UVGEN_CLASS_ID; }

			/*! \remarks			Sets the UVGen rollup state to open or closed.
			\par Parameters:
			<b>BOOL open</b>\n\n
			TRUE for open; FALSE for closed. */
			virtual void SetRollupOpen(BOOL open)=0;
			/*! \remarks			Returns the open or closed state of the UVGen rollup.
			\return  TRUE is open; FALSE is closed. */
			virtual BOOL GetRollupOpen()=0;
			CoreExport SvGraphNodeReference SvTraverseAnimGraph(IGraphObjectManager *gom, Animatable *owner, int id, DWORD flags);
			CoreExport MSTR SvGetName(IGraphObjectManager *gom, IGraphNode *gNode, bool isBeingEdited)
				 {
				 return Animatable::SvGetName(gom, gNode, isBeingEdited);
				 }
};

CoreExport UVGen* GetNewDefaultUVGen();

#pragma warning(pop)

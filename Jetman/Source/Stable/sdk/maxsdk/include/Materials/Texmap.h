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
#include "../Acolor.h"

#pragma warning(push)
#pragma warning(disable:4100)

class UVGen;
class XYZGen;

/*! \defgroup textureSymmetryFlags Texture Symmetry Flags
\sa Class StdUVGen, Class TexHandleMaker */
//@{
#define U_WRAP   (1<<0) //!< Indicates the texture map is tiled in the U direction.
#define V_WRAP   (1<<1) //!< Indicates the texture map is tiled in the V direction.
#define U_MIRROR (1<<2) //!< Indicates the texture map is mirrored in the U direction.
#define V_MIRROR (1<<3) //!< Indicates the texture map is mirrored in the V direction.
#define UV_NOISE (1<<4)
#define UV_NOISE_ANI (1<<5)
//@}

// Where to get texture vertices: returned by GetUVWSource();
#define UVWSRC_EXPLICIT    0  // use explicit mesh texture vertices (whichever map channel)
#define UVWSRC_OBJXYZ      1  // generate planar uvw mapping from object xyz on-the-fly
#define UVWSRC_EXPLICIT2   2  // use explicit mesh map color vertices as a tex map
#define UVWSRC_WORLDXYZ    3  // generate planar uvw mapping from world xyz on-the-fly
#define UVWSRC_FACEMAP     5  // use face map
#define UVWSRC_HWGEN    6  // use hardware generated texture coords

// virtual texture map interface
/*! \sa  Class MtlBase, Class ShadeContext, Class UVGen, Class XYZGen, Class TexHandleMaker, Class NameAccum, Class AColor, Class Matrix3, Noise and Misc Functions</a>.\n\n
\par Description:
This is the base class for the creation of texture map plug-ins. It provides
methods for things such as calculating the color of the map for a given
location, and computing a perturbation to apply to a normal for bump
mapping.\n\n
Note: Developers creating procedural textures should be aware that these
textures may appear less than perfect when a 3ds Max user is using the "Quick
Renderer" in the Materials Editor. This is not the fault of the plug-in
texture, it is simply that the "Quick Renderer" uses an algorithm that is
quicker but less accurate than the standard scanline renderer. Therefore, don't
be concerned if your texture does not show up perfectly when using "Quick
Renderer".
\par Plug-In Information:
Class Defined In IMTL.H\n\n
Super Class ID TEXMAP_CLASS_ID\n\n
Standard File Name Extension DLT\n\n
Extra Include Files Needed IMTL.H (and optionally TEXUTIL.H)
\par Additional Information Related to Bump Mapping:
The following function evaluates the normal perturbation vector in the Bitmap texture.
\snippet samples/materials/bmtex.cpp EvalNormalPerturb

\par The function GetBumpDP() returns the "bump basis vectors". These are the gradient vectors of the UVW mapping
in the object coordinate space. The following function is used to compute the U and V bump basis vectors for
a triangle given the texture coordinates at the three vertices of the triangle ( tv[ ] ) and the 3D coordinates
at the vertices ( v[ ] ). It is simply a solution using linear algebra for the U and V axes in terms of the XYZ
coordinates. It returns:\n\n
\code
 b[0] = DP/DU 
 b[1] = DP/DV
\endcode


\code
void ComputeBumpVectors(const Point3 tv[3], const Point3 v[3], Point3 bvec[3]) {
	Point3 U1,U2;
	Point3 P1,P2,R;

	// three points define triangle in XYZ space
	P1 = v[1]-v[0];
	P2 = v[2]-v[0];						  
	// compute normal vector to the triangle in XYZspace
	Point3 Nxyz = Normalize(CrossProd(P1,P2));
	
	// three texture verts define corresponding triangle in UVW space
	U1 = tv[1]-tv[0];
	U2 = tv[2]-tv[0];

	// compute normal vector to the triangle in UVW space
	Point3 N = Normalize(CrossProd(U1,U2));

	// Compute a scale factor to use in calculating the portion of the
	// bump vectors normal to the face 	-- use the average of the U
	// and V scale factors. We are assuming a lot here.
	float l1 = Length(U1);
	float l2 = Length(U2);
	if (l1==0.0f) l1 = .0000001f;
	if (l2==0.0f) l2 = .0000001f;
	float r1 = Length(P1)/l1;
	float r2 = Length(P2)/l2;
	float r = (r1+r2)/2.0f;  // a relative metric for scaling the normal direction

	// First find how a unit vector along the U-axis moves into XYZ space.
	Point3 UAxis(1.0f,0.0f,0.0f);

	// Will resolve the UAxis into three vectors: one normal to
	// triangle, and two in triangle. The one normal to the 
	// triangle is N.x*N, because DotProd(UAxis,N) = N.x.
	// The remainder is
	R = UAxis-N.x*N;
	
	// now compute the barycentric coords for R in terms of U1 (s)  and U2 (t)
	Point3 bc = BaryCoords(pOrigin, U1, U2, R);

	// now compose the equivalent vector in XYZ space
	bvec[0] = Normalize(bc[1]*P1 + bc[2]*P2 + r*N.x*Nxyz);

	// Similarly for VAxis
	R = Point3(0.0f,1.0f,0.0f) - N.y*N;
	bc = BaryCoords(pOrigin, U1, U2, R);
	bvec[1] =  Normalize(bc[1]*P1 + bc[2]*P2 + r*N.y*Nxyz);

	// Similarly for WAxis
	R = Point3(0.0f,0.0f,1.0f) - N.z*N;
	bc = BaryCoords(pOrigin, U1, U2, R);
	bvec[2] =  Normalize(bc[1]*P1 + bc[2]*P2 + r*N.z*Nxyz);
	}
\endcode
The three Point3's returned in bvec are stored away, and then simply returned in the function SContext::GetBumpDP(). 
The function UVGen::EvalDeriv() evaluates the local derivative of the texture map in the U and V directions, taking into account symmetry and scaling. 
The resulting derivative, dM, is scaled down from the value returned by EvalDeriv to keep the bump perturbations in a more reasonable range. 
The perturbation vector is calculated as:\n\n
\code
dM.x*dPdu+dM.y*dPdv
\endcode
And, then passed through the texout->Filter function, which is just the following:\n\n
\code
Point3 Texout::Filter(Point3 c) { 
 if (outAmt!=1.0f) c *= outAmt 
 if (flags&TEXOUT_INVERT) c = -c 
 return c 
 }  
\endcode
*/
class Texmap: public MtlBase 
{
	 public:
			int cacheID;    // used by renderer for caching returned values
						
			/*! \remarks Constructor. The number of active uses of this texture is
			set to 0. */
			CoreExport Texmap();

			SClass_ID SuperClassID() { return TEXMAP_CLASS_ID; }
			virtual void GetClassName(MSTR& s) { s= MSTR(_M("Texture")); }  

//! \name Evaluation Methods
//! @{			
			// Evaluate the color of map for the context.
			/*! \remarks This method is called to evaluate the color of the texture map for the
			context. This is for channels that have a color such as diffuse,
			specular, ambient, etc. This method is called for every pixel of the
			texture.
			\par Parameters:
			<b>ShadeContext\& sc</b>\n\n
			Describes the properties of the pixel to evaluate.
			\return  An <b>AColor</b> object which stores the <b>r, g, b, a</b>
			values. Note: The alpha is pre multiplied, and the alpha value goes into
			<b>AColor::a</b>. */
			virtual  AColor EvalColor(ShadeContext& sc)=0;
	 
			// Evaluate the map for a "mono" channel.
			// this just permits a bit of optimization 
			/*! \remarks Evaluate the map for a "mono" channel. Mono channels are those that don't
			have a color, but rather a single value. This is for things like
			shininess, transparency, etc. This just permits a bit of optimization.
			\par Parameters:
			<b>ShadeContext\& sc</b>\n\n
			Describes the properties of the pixel to evaluate.
			\return  A floating point value for the mono channel.
			\par Default Implementation:
			<b>{return Intens(EvalColor(sc));}</b> */
			virtual  float  EvalMono(ShadeContext& sc) {
				 return Intens(EvalColor(sc));
				 }
			
			// For Bump mapping, need a perturbation to apply to a normal.
			// Leave it up to the Texmap to determine how to do this.
			/*! \remarks This method is used for bump mapping to retrieve a perturbation to apply
			to a normal.
			\par Parameters:
			<b>ShadeContext\& sc</b>\n\n
			Describes the properties of the pixel to evaluate.
			\return  A deflection vector for perturbing the normal. */
			virtual  Point3 EvalNormalPerturb(ShadeContext& sc)=0;

			// This query is made of maps plugged into the Reflection or 
			// Refraction slots:  Normally the view vector is replaced with
			// a reflected or refracted one before calling the map: if the 
			// plugged in map doesn't need this , it should return TRUE.
			/*! \remarks			This query is made of maps plugged into the Reflection or Refraction
			slots: Normally the view vector is replaced with a reflected or refracted
			one before calling the map: if the plugged in map doesn't need this, it
			should return TRUE.
			\par Default Implementation:
			<b>{ return FALSE; }</b> */
			virtual BOOL HandleOwnViewPerturb() { return FALSE; }
//! @}			
			
			// This gets the viewport display bitmap in DIB format, useful when combining several maps
			// for hardware-supported multiple texture display.  // DS 4/17/00
			// IF mono=TRUE, the map should do a mono evaluation and place the result in RGB.
			// forceW and forceH, if non-zero, override dimensions specified by thmaker.
			/*! \remarks			This gets the viewport display bitmap in DIB format, useful when
			combining several maps for hardware-supported multiple texture display.
			If <b>mono</b> is TRUE, the map should do a mono evaluation and place the
			result in RGB. <b>forceW</b> and <b>forceH</b>, if non-zero, override
			dimensions specified by <b>thmaker</b>.
			\par Parameters:
			<b>TimeValue t</b>\n\n
			The time to get the bitmap at.\n\n
			<b>TexHandleMaker\& thmaker</b>\n\n
			This class provides methods for creating a texture handle from a 3ds Max
			bitmap and a Windows DIB. It also has a method to retrieve the required
			size of the texture map. See Class TexHandleMaker.\n\n
			<b>Interval \&valid</b>\n\n
			The validity interval of the returned bitmap.\n\n
			<b>BOOL mono</b>\n\n
			Indicates whether a map should do mono evaluation.\n\n
			<b>int forceW</b>\n\n
			Overrides the bitmap width usually supplied by <b>thmaker</b>.\n\n
			<b>int forceH</b>\n\n
			Overrides the bitmap height usually supplied by <b>thmaker</b>. */
			CoreExport virtual BITMAPINFO* GetVPDisplayDIB(TimeValue t, TexHandleMaker& thmaker, Interval &valid, BOOL mono=FALSE, int forceW=0, int forceH=0);  

//! \name Get UV Transform/Texture Tiling
//! @{			
			/*! \remarks This method is called to retrieve the UV transformation matrix for use in
			the viewports. If a developer is using an instance of UVGen, a method of
			that class may be called to retrieve the value:\n\n
			(i.e. <b>{ uvGen-\>GetUVTransform(uvtrans); }</b> ).
			\par Parameters:
			<b>Matrix3 \&uvtrans</b>\n\n
			The transformation matrix is returned here.
			\par Default Implementation:
			<b>{}</b> */
			virtual  void GetUVTransform(Matrix3 &uvtrans) { uvtrans.IdentityMatrix(); }
			/*! \remarks This method is called to get the tiling state of the texture for use in
			the viewports. This is described by a set of symmetry flags that may be
			ORed together. If you are using an instance of UVGen to handle the UV
			user interface you may simply call a method of UVGen to handle this.\n\n
			For example: <b>{ return uvGen-\>GetTextureTiling(); }</b>
			\return  See \ref textureSymmetryFlags.
			\par Default Implementation:
			<b>{ return U_WRAP|V_WRAP; }</b> */
			virtual int GetTextureTiling() { return  U_WRAP|V_WRAP; }

			/*! \remarks			Returns a value indicating where to get the texture vertices for the
			Texmap.
			\return  One of the following values:\n\n
			<b>UVWSRC_EXPLICIT</b>\n\n
			Use explicit mesh texture vertices from one of the mapping channels (see
			<b>GetMapChannel()</b> below to determine which one). This uses the UVW
			coordinates assigned to the object, either through the Generate Mapping
			Coordinates option in the object's creation parameters, or through
			mapping modifiers, such as UVW Map.\n\n
			<b>UVWSRC_EXPLICIT2</b>\n\n
			Use explicit mesh texture vertices from the Vertex Color Channel.\n\n
			<b>UVWSRC_OBJXYZ</b>\n\n
			Generate planar UVW mapping coordinates from the object local XYZ
			on-the-fly. This corresponds to the "Planar from Object XYZ" option.\n\n
			<b>UVWSRC_WORLDXYZ</b>\n\n
			This value is available in release 3.0 and later only.\n\n
			Generate planar UVW mapping coordinates from the world XYZ on-the-fly.
			This corresponds to the "Planar From World XYZ" option. Note: this value
			used for the UVW is the world XYZ, taken directly, with out normalization
			to the objects bounding box. This differs from "Planar from Object XYZ",
			where the values are normalized to the object's bounding box.
			\par Default Implementation:
			<b>{ return UVWSRC_EXPLICIT; }</b> */
			virtual int GetUVWSource() { return UVWSRC_EXPLICIT; }
			/*! \remarks			Returns the map channel being used by the texmap if <b>GetUVWSource()</b>
			returns <b>UVWSRC_EXPLICIT</b>. The return value should be at least 1. A
			value of 0 is not acceptable.
			\par Default Implementation:
			<b>{ return 1; }</b> */
			virtual int GetMapChannel () { return 1; }   // only relevant if above returns UVWSRC_EXPLICIT
//! @}

//! \name Slot Type
//! @{								
			/*! \remarks This method is called to initialize the slot type. This sets the proper
			button in the coordinate user interface rollup page. If you are using an
			instance of UVGen to handle the UV user interface you may simply call a
			method of UVGen to handle this. For example: <b>{ if (uvGen)
			uvGen-\>InitSlotType(sType); }</b>
			\par Parameters:
			<b>int sType</b>\n\n
			See \ref mapSlotTypes.
			\par Default Implementation:
			<b>{}</b> */
			virtual void InitSlotType(int sType) { defaultSlotType = sType; }          
			virtual int MapSlotType(int i) { return defaultSlotType; }
//! @}

//! \name UVGen and XYZGen Access
//! @{
			/*! \remarks			Texture maps that use a UVGen should implement this method to return a
			pointer to it.
			\par Default Implementation:
			<b>{ return NULL; }</b> */
			virtual UVGen *GetTheUVGen() { return NULL; }  // maps with a UVGen should implement this
			/*! \remarks			Texture maps that use a XYZGen should implement this method to return a
			pointer to it.
			\par Default Implementation:
			<b>{ return NULL; }</b> */
			virtual XYZGen *GetTheXYZGen() { return NULL; } // maps with a XYZGen should implement this
//! @}
			
			// System function to set slot type for all subtexmaps in a tree.
			/*! \remarks Implemented by the System.\n\n
			This method is used internally to set the slot type for all subtexmaps in
			a tree. */
			CoreExport void RecursInitSlotType(int sType);

//! \name Output Level
//! @{
			/*! \remarks Sets the output level at the specified time. It is used to set the output
			level of the embedded Texout object, principally by importing plug-ins.
			It is implemented in all Texmaps.
			\par Parameters:
			<b>TimeValue t</b>\n\n
			The time to set the output level.\n\n
			<b>float v</b>\n\n
			The value to set.
			\par Default Implementation:
			<b>{}</b> */
			virtual void SetOutputLevel(TimeValue t, float v) {}

			/*! \remarks			Implemented by the system.\n\n
			Returns true only if all submaps and itself have a meaningful output.\n\n
			Returns false if at least one sub-texmap or itself does not have a meaningful
			output\n\n
			The output of a texmap is meaningful in a given ShadeContext if it is the same
			as when the scene is rendered. If the map cannot determine whether the output
			value is the same as when rendered, it should not be meaningful. This method
			can be called before <b>EvalColor()</b> or <b>EvalMono()</b> on a texmap in
			order to decide whether to call these methods at all or if their return values
			should be used in further calculations.
			\par Parameters:
			<b>ShadeContext\& sc</b>\n\n
			This describes the context of the question. */
			CoreExport virtual bool IsOutputMeaningful( ShadeContext& sc );

			// The default implementation returns false
			/*! \remarks			Returns TRUE if the output of <b>this</b> texmap is meaningful for the given
			context; it should not take into account subtexmaps. This method is called by
			<b>IsOutputMeaningful()</b>.
			\par Parameters:
			<b>ShadeContext\& sc</b>\n\n
			This describes the context of the question.
			\par Default Implementation:
			<b>{ return false; }</b> */
			virtual bool IsLocalOutputMeaningful( ShadeContext& sc ) { return false; }
//! @}

//! \name Loading Bitmap Files
//! @{
			// called prior to render: missing map names should be added to NameAccum.
			// return 1: success,   0:failure. 
			/*! \remarks This method is called prior to rendering to allow the plug-in to load any
			bitmap files it requires.\n\n
			Note that <b>LoadMapFiles()</b> is called to load map files only, not to
			list the missing files. The missing files are listed using the
			<b>EnumAuxFiles()</b> method, which allows enumerating them without
			loading them.\n\n
			Also Note: There is currently not an <b>UnloadMapFiles()</b> method.
			There are a couple of ways to do this however. One is to call
			<b>Interface::FreeAllBitmaps()</b>. That method traverses the scene
			reference hierarchy and calls <b>Animatable::FreeAllBitmaps()</b> on each
			item. Another approach is to evaluate the Material / TextureMap hierarchy
			on each material. Then call <b>Animatable::FreeAllBitmaps()</b> yourself
			in the <b>MtlEnum::proc()</b> shown below.\n\n
			\code
			class MtlEnum	
			{
				virtual void proc(MtlBase *m) = 0;
			};
			
			void EnumMtlTree(MtlBase *mb, MtlEnum &tenum)
			{
				tenum.proc(mb);
				for (int i=0; i<mb->NumSubTexmaps(); i++) {
					Texmap *st = mb->GetSubTexmap(i);
					if (st)
						EnumMtlTree(st,tenum);
				}
				if (IsMtl(mb)) {
					Mtl *m = (Mtl *)mb;
					for (i=0; i<m->NumSubMtls(); i++) {
						Mtl *sm = m->GetSubMtl(i);
						if (sm)
							EnumMtlTree(sm,tenum);
					}
				}
			}
			\endcode 
			Now just define a subclass of MtlEnum that does what you want, and call
			EnumMtlTree. In this particular case it is more efficient than
			enumerating the entire reference hierarchy. If you do want to enumerate
			the entire reference hierarchy, here's how:\n\n
			\code
			class RefEnumProc
			{
				virtual void proc(ReferenceMaker *rm)=0;
			};
			
			void EnumRefs(ReferenceMaker *rm, RefEnumProc &proc)
			{
				proc.proc(rm);
				for (int i=0; i<rm->NumRefs(); i++) {
					ReferenceMaker *srm = rm->GetReference(i);
					if (srm) EnumRefs(srm,proc);
				}
			}
			\endcode
				Just define a subclass of RefEnumProc that does what you want, and call
					EnumRefs on the part of the reference hierarchy you want to enumerate.
					For example:\n\n
			\code
			class MyEnum: public RefEnumProc
			{
				void proc(ReferenceMaker *rm) {
					// do something ...
				}
			}
			
			void afunction(Mtl* m)
			{
				MyEnum enumer;
				EnumRefs(m,&enumer);
			}
			\endcode 
			\par Parameters:
			<b>TimeValue t</b>\n\n
			The time the maps are being loaded.
			\return  Always return nonzero from this method.
			\par Default Implementation:
			<b>{ return 1; }</b> */
			virtual int LoadMapFiles(TimeValue t) { return 1; } 

			// Render a 2-d bitmap version of the map.
			/*! \remarks			This method is used to render a 2D bitmap version of this texmap.
			\par Parameters:
			<b>TimeValue t</b>\n\n
			The time at which to render the texmap to the bitmap.\n\n
			<b>Bitmap *bm</b>\n\n
			A pointer to a bitmap to render to. This bitmap must be created at the
			resolution you wish to render to.\n\n
			<b>float scale3D=1.0f</b>\n\n
			This is a scale factor applied to 3D Texmaps. This is the scale of the
			surface in 3d space that is mapped to UV. This controls how much of the
			texture appears in the bitmap representation.\n\n
			<b>BOOL filter = FALSE</b>\n\n
			If TRUE the bitmap is filtered. It is quite a bit slower to rescale
			bitmaps with filtering on. 
			\par Default Implementation:
			The default implementation calls <b>Interface::RenderTexmap()</b>. */
			CoreExport virtual void RenderBitmap(TimeValue t, Bitmap *bm, float scale3D=1.0f, BOOL filter = FALSE);
//! @}

			CoreExport void RefAdded(RefMakerHandle rm);

			// Schematic View Animatable Overrides...
			CoreExport SvGraphNodeReference SvTraverseAnimGraph(IGraphObjectManager *gom, Animatable *owner, int id, DWORD flags);


			// Is the texture High Dynamic Range?
			/*! \remarks			Returns nonzero if the texture is returning high dynamic range data;
			otherwise zero.
			\par Default Implementation:
			<b>{ return false; }</b> */
			virtual int IsHighDynamicRange( ) const { return false; }
};

/*! \sa  Class Texmap.\n\n
\par Description:
This class provides a method <b>AddMapName()</b> that is called when a Texmap
is attempting to load its map and it cannot be found.  */
class NameAccum: public MaxHeapOperators 
{
	 public:
			/*! \remarks Destructor. */
			virtual ~NameAccum() {;}
			/*! \remarks Implemented by the System.\n\n
			This method is called to add the name of a map that was not found.
			\par Parameters:
			<b>MCHAR *name</b>\n\n
			The name to add. */
			virtual  void AddMapName(const MCHAR *name)=0;
};

#pragma warning(pop)

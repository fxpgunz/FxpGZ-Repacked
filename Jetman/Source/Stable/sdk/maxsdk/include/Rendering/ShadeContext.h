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

#include "RenderGlobalContext.h"
#include "../BaseInterface.h"
#include "../Color.h"
#include "../AColor.h"
#include "../CoreExport.h"
#include "../Box3.h"

#pragma warning(push)
#pragma warning(disable:4100)

class LightDesc;
class RenderGlobalContext;
class INode;
class Object;
class Texmap;
class IRenderElement;

#define N_MAX_RENDER_ELEMENTS 32

// Transform Reference frames: 
enum RefFrame { REF_CAMERA=0, REF_WORLD, REF_OBJECT };

// > 4/19/02 - 12:25am --MQM--  
// PLUG-IN DEVELOPER NOTE:
//
//    In this release we are using the xshadeID in the
//    ShadeContext to store information during Light Tracer 
//    and VIZ Radiosity renderings.  The developer should be
//    sure to copy the xshadeID whenever creating new shade
//    contexts based off of an existing ShadeContext instance.
//
//    The renderer performs two passes when a radiosity
//    plugin is on.  The first pass is a very minimal pass,
//    and the second is the normal pass.  Shader plugins
//    can test for the pre-pass flag and possibly do less
//    (or no) work.
//
//    During the regular pass, the Light Tracer will max out
//    the rayLevel to halt raytraced reflections during regathering.  
//    Shaders should also not do specular shading or other things 
//    that will increase variance and/or slow regathering performance 
//    down. 

// we are inside of a regathering ray
#define SHADECONTEXT_REGATHERING_FLAG        0x80000000  
#define SHADECONTEXT_IS_REGATHERING( sc )    ( (sc).xshadeID & SHADECONTEXT_REGATHERING_FLAG )

// we are in a pre-pass in the scanline renderer
#define SHADECONTEXT_PREPASS_FLAG            0x40000000  
#define SHADECONTEXT_IS_PREPASS( sc )        ( (sc).xshadeID & SHADECONTEXT_PREPASS_FLAG )

// in recursive eval, so we don't try to 
// regather something that we're already 
// in the middle of...
#define SHADECONTEXT_RECURSIVE_EVAL_FLAG     0x20000000  
#define SHADECONTEXT_IS_RECURSIVE_EVAL( sc ) ( (sc).xshadeID & SHADECONTEXT_RECURSIVE_EVAL_FLAG )

// matte material needs a way of getting the shadow values out
// of light tracer.  (this is going away when we can change the sdk!!)
#define SHADECONTEXT_GUESS_SHADOWS_FLAG         0x10000000
#define SHADECONTEXT_GUESS_SHADOWS( sc )     ( (sc).xshadeID & SHADECONTEXT_GUESS_SHADOWS_FLAG )

// scanline renderer thread #
#define SHADECONTEXT_THREAD_MASK          0x0000F000  
#define SHADECONTEXT_THREAD_SHIFT            12
#define SHADECONTEXT_GET_THREAD( sc )        ( ( (sc).xshadeID & SHADECONTEXT_THREAD_MASK )  >> SHADECONTEXT_THREAD_SHIFT )
#define SHADECONTEXT_SET_THREAD( sc, thread )   { (sc).xshadeID = ( (sc).xshadeID & (~SHADECONTEXT_THREAD_MASK) )  |  ( ( (thread) << SHADECONTEXT_THREAD_SHIFT ) & SHADECONTEXT_THREAD_MASK ); }

// worker thread #
#define SHADECONTEXT_WORKER_MASK          0x00000F00
#define SHADECONTEXT_WORKER_SHIFT            8
#define SHADECONTEXT_GET_WORKER( sc )        ( ( (sc).xshadeID & SHADECONTEXT_WORKER_MASK )  >> SHADECONTEXT_WORKER_SHIFT )
#define SHADECONTEXT_SET_WORKER( sc, worker )   { (sc).xshadeID = ( (sc).xshadeID & (~SHADECONTEXT_WORKER_MASK) )  |  ( ( (worker) << SHADECONTEXT_WORKER_SHIFT ) & SHADECONTEXT_WORKER_MASK ); }

// bounce # (1..255, 0 not used)
#define SHADECONTEXT_BOUNCE_MASK          0x000000FF
#define SHADECONTEXT_GET_BOUNCE( sc )        ( (sc).xshadeID & SHADECONTEXT_BOUNCE_MASK )
#define SHADECONTEXT_SET_BOUNCE( sc, bounce )   { (sc).xshadeID = ( (sc).xshadeID & (~SHADECONTEXT_BOUNCE_MASK) )  |  ( (bounce) & SHADECONTEXT_BOUNCE_MASK ); }

// general macro to init regathering info in the shade context.
// this wipes out other flags in the xshadeID (prepass, etc)
#define SHADECONTEXT_SET_REGATHERING_INFO( sc, thread, worker, bounce ) \
{ (sc).xshadeID = SHADECONTEXT_REGATHERING_FLAG  |  \
    ( ( (thread) << SHADECONTEXT_THREAD_SHIFT ) & SHADECONTEXT_THREAD_MASK )  |  \
    ( ( (worker) << SHADECONTEXT_WORKER_SHIFT ) & SHADECONTEXT_WORKER_MASK )  |  \
    ( (bounce) & SHADECONTEXT_BOUNCE_MASK ); }


//! Values that may be assigned to ShadeContext::mode.
enum SCMode
{
    //! In normal mode, the material should do the entire illumination including transparency, refraction, etc.
    SCMODE_NORMAL = 0,
    //! In shadow mode, you are just trying to find out what color the shadow is that is falling on an object.In this case, all you care about is transmitted color.
    SCMODE_SHADOW = 1,
    //! Performing render to texture
    SCMODE_BAKE = 2,
    //! The shade context is currently used to render (or bake) a Texmap, for use as a preview in the material editor or for viewport display. In this
    //! mode, no renderer is used - the texture map is evaluated directly.
    SCMODE_TEXMAP = 3,
	//! The shade context is currently used to bake a 2D Texmap, for use in a renderer/viewer that does not support the texmap. The UVGenerator is
	//! disabled in this mode, so the unit square of the texture will be renderer. As in the previous mode, no renderer is used - the texture map is evaluated directly.
	SCMODE_TEXTURE_BAKE = 4,
};

/*! \sa  Class ShadeContext, Class Color.\n\n
\par Description:
An instance of this class is a data member of the <b>ShadeContext</b>. This is
used to contain the computed color and transparency of the pixel being shaded
by a material. All methods of this class are implemented by the system.
\par Data Members:
<b>ULONG flags;</b>\n\n
These flags are not currently used.\n\n
<b>Color c;</b>\n\n
Shaded color of the pixel.\n\n
<b>Color t;</b>\n\n
Transparency of the pixel.\n\n
<b>float ior;</b>\n\n
Index of refraction of the pixel.\n\n
<b>int gbufId;</b>\n\n
The G-buffer ID. This allows the <b>MixIn()</b> method to pick the id of the
material which was blended in the highest proportion.  */
class ShadeOutput : public BaseInterfaceServer {
	 public:
			ULONG flags;
			Color c;  // shaded color
			Color t;  // transparency
			float ior;  // index of refraction
			int gbufId;

			// render elements
			int nElements;
			AColor   elementVals[ N_MAX_RENDER_ELEMENTS ];

			/*! \remarks This method is used to blend the output of two texmaps, for
			example, in the Mix texmap. The function is as follows:\n
			\code
			void ShadeOutput::MixIn(ShadeOutput &a, float f)
			{
				if (f<=0.0f) {
					(*this) = a;
					return;
				}
				else if (f>=1.0f) {
					return;
				}
				else {
					float s = 1.0f - f;
					flags |= a.flags;
					c = s*a.c + f*c;
					t = s*a.t + f*t;
					ior = s*a.ior + f*ior;
					if (f<=0.5f) gbufId = a.gbufId;
				}
			}
			\endcode 
			This does a blend of <b>a</b> with <b>(*this)</b>. This blend is applied
			to the color, transparency, and index of refraction. The flags of are
			OR'ed together.
			\par Parameters:
			<b>ShadeOutput\& a</b>\n\n
			The output of the texmap to blend in.\n\n
			<b>float f</b>\n\n
			The amount to blend in, i.e.:\n\n
			<b>a.MixIn(b, 1.0f)</b> results in 100% of <b>a</b> and none of <b>b</b>.
			*/
			CoreExport void MixIn(ShadeOutput& a, float f);  // (*this) =  (1-f)*(*this) + f*a;
			ShadeOutput():nElements(0), flags(0), gbufId(0){};
			ShadeOutput( int nEle ):flags(0), gbufId(0){ nElements = nEle; }
			/*! \remarks Implemented by the System.\n\n
			This method resets the data member such that: <b>c</b> is set to black,
			<b>t</b> is set to black, <b>ior</b> is set to 1.0, <b>gbufId</b> is set to 0,
			and the <b>flags</b> are set to 0.
			\par Parameters:
			<b>int n = -1</b>\n\n
			By supplying a negative value this method will clear elements but leave the
			number of elements unchanged. */
			void Reset( int nEle = -1 )
			{ 
				 flags = 0;
				 gbufId = 0;
				 c.Black(); t.Black(); ior = 1.0f;
				 // clear nEle's worth of render elements
				 if (nEle >= 0)
						nElements = nEle;
				 for( int i=0; i < nElements; ++i )
						elementVals[i].r = elementVals[i].g =
							 elementVals[i].b = elementVals[i].a = 0.0f;
			}
			CoreExport ShadeOutput* Clone();       

			// Render Element interface
			// allocate new val, return index
			int NewElementVal(){
				 DbgAssert((nElements+1)<N_MAX_RENDER_ELEMENTS); 
				 return nElements++; 
			}
			int NElementVals(){ return nElements; }
			// override new val & set nElements directly, for clear mostly
			void SetNElements( int n ){
				 DbgAssert( n>=0 && n<N_MAX_RENDER_ELEMENTS); 
				 nElements = n;
			}
			// return rgba value of element n
			AColor ElementVal( int n ){ 
				 DbgAssert( n>=0 && n<N_MAX_RENDER_ELEMENTS); 
				 return elementVals[n]; 
			}
			// set rgba value of element n
			void SetElementVal( int n, AColor color ){ 
				 DbgAssert( n>=0 && n<N_MAX_RENDER_ELEMENTS); 
				 elementVals[n] = color;
			}
			// sum rgba value intof element n
			void AccumElementVal( int n, AColor color){
				 DbgAssert( n>=0 && n<N_MAX_RENDER_ELEMENTS); 
				 elementVals[n] += color;
			}
			// sum all color components of a shade output
			void Sum( ShadeOutput* pSample ){
				 c += pSample->c; t += pSample->t;
				 for( int i = 0; i < nElements; ++i )
						elementVals[i] += pSample->elementVals[i];
			}
			void Scale( float x ){
				 c *= x; t *= x;
				 for( int i = 0; i < nElements; ++i )
						elementVals[i] *= x;
			}
			// sum all color components, w/o alpha of a shade output
			void SumClr( ShadeOutput* pSample ){
				 c += pSample->c; 
				 for( int i = 0; i < nElements; ++i ){
						elementVals[i].r += pSample->elementVals[i].r;
						elementVals[i].g += pSample->elementVals[i].g;
						elementVals[i].b += pSample->elementVals[i].b;
				 }
			}

			ShadeOutput& operator += ( ShadeOutput& a ){
				 c += a.c;  
				 t += a.t; 
				 for( int i = 0; i < nElements; ++i )
						elementVals[i] += a.elementVals[i];
				 return *this;
			}
			ShadeOutput& operator *= ( Color x ){
				 c *= x; // t *= x; 
				 AColor ac( x );
				 ac.a = Intens( x );
				 for( int i = 0; i < nElements; ++i )
						elementVals[i] *= ac;
				 return *this;
			}

			ShadeOutput& operator = ( const ShadeOutput& a ){
				 flags = a.flags;
				 c = a.c; t = a.t; 
				 ior = a.ior;
				 gbufId = a.gbufId;
				 nElements = a.nElements;
				 for( int i = 0; i < nElements; ++i )
						elementVals[i] = a.elementVals[i];
				 return *this;
			}


	 };

//
// Shade Context: passed into Mtls and Texmaps
//
/*! \sa  ~{ Materials, Textures and Maps }~, 
Class LightDesc, Class RenderGlobalContext, Class Color, Class ShadeOutput, Class AColor, Class UVGen,  Class Box3,  Class IPoint2, Class Point2, Class IRenderElement, Class Point3, Class Class_ID.\n\n
\par Description:
This class is passed to materials and texture maps. It contains all the
information necessary for shading the surface at a pixel.\n\n
Normally, the <b>ShadeContext</b> is provided by the 3ds Max renderer. However
developers that need to create their own <b>ShadeContext</b> for use in passing
to the texture and material evaluation functions can do so by deriving a class
from <b>ShadeContext</b> and providing implementations of the virtual methods.
Some sample code is available demonstrating how this is done in
<b>/MAXSDK/SAMPLES/OBJECTS/LIGHT.CPP</b> (see the code for <b>class SCLight :
public ShadeContext</b>). The default implementations of these methods are
shown for developers that need to create their own <b>ShadeContext</b>.\n\n
Note that raytracing (and all shading calculations in the default renderer)
takes place in camera space.\n\n
For additional information on the methods <b>DP()</b>, <b>Curve()</b>,
<b>DUVW()</b> and <b>DPdUVW()</b> see
<a href="class_shade_context.html#A_GM_shadec_add">Additional Notes</a>.\n\n
All methods are implemented by the system unless noted otherwise.
\par Data Members:
<b>BOOL doMaps</b>\n\n
Indicates if texture maps should be applied.\n\n
<b>BOOL filterMaps;</b>\n\n
Indicates if textures should be filtered.\n\n
<b>BOOL shadow</b>\n\n
Indicates if shadows should be applied.\n\n
<b>BOOL backFace;</b>\n\n
Indicates if we are on the back side of a 2-sided face.\n\n
<b>int mtlNum</b>\n\n
The material number of the face being shaded. This is the sub-material number
for multi-materials.\n\n
<b>Color ambientLight</b>\n\n
This is the color of the ambient light.\n\n
<b>int nLights;</b>\n\n
This is the number of lights being used in a render, which is the number of
active lights in the scene, or if there are none, 2 for the default lights. 
For example, this is used in the Standard material in a loop like this:\n\n
\code
LightDesc *l;
for (int i=0; i\<sc.nLights; i++) {
	l = sc.Light(i);
	..etc
}
\endcode
<b>int rayLevel;</b>\n\n
This data member is available in release 2.0 and later only.\n\n
This is used to limit the number of reflections for raytracing. For instance,
if you're rendering a hall of mirrors, and the ray is reflecting back and
forth, you don't want the raytracing to go forever. Every time
<b>Texmap::EvalColor()</b> gets called again on a ray you create a new
<b>ShadeContext</b> and bump up the <b>rayLevel</b> one. This allows you to
test this value and see if it has reached the limit of how deep to go (if it
reaches a maximum level, you can return black for example).\n\n
Note that it is conceivable that more than one raytrace material can be in
effect at a time (from different developers). In such a case, where one surface
might have one raytracer and another surface a different one, and a ray was
bouncing back and forth between them, each needs to be aware of the other. This
is why this value is here -- the two texmaps each modify and check it.\n\n
<b>int xshadeID;</b>\n\n
This data member is available in release 2.0 and later only.\n\n
This is currently not used.\n\n
<b>RenderGlobalContext *globContext;</b>\n\n
This data member is available in release 2.0 and later only.\n\n
Points to an instance of <b>RenderGlobalContext</b>. This class describes the
properties of the global rendering environment. This provides information such
as the renderer in use, the project type for rendering, the output device width
and height, several matrices for transforming between camera and world
coordinates, the environment map, the atmospheric effects, the current time,
field rendering information, and motion blur information.\n\n
<b>LightDesc *atmosSkipLight;</b>\n\n
The light description of lights to prevent self shadowing by volumetric lights.\n\n
<b>RenderGlobalContext *globContext;</b>\n\n
A pointer to the rendering global context.\n\n
<b>ShadeOutput out;</b>\n\n
This is where the material should leave its results.\n\n
The following is a discussion of blending the ShadeContext.out.c and
ShadeContext.out.t together to get the final color:\n\n
The (c,t) returned by shaders is interpreted as follows: t.r is the
(premultiplied) alpha for the r-channel, etc.\n\n
So if you want to composite (c,t) over a background b,\n\n
color = b*t + c ( where the multiplication of b and t multiplies the individual
components ).\n\n
When you want to convert a (c,t) to a simple R,G,B,Alpha, just average together
the components of t to get Alpha. (and use the r,g,b components of c directly).
*/
class ShadeContext : public InterfaceServer {
public:
    //! The rendering mode in which the shade context is currently operating. Maybe be used
    //! by materials and texmaps to modify their behaviour.
    SCMode mode;
	BOOL doMaps;                  // apply texture maps?
	BOOL filterMaps;              // should texture be filtered                
	BOOL shadow;                  // apply shadows?
	BOOL backFace;                // are we on the back side of a 2-Sided face?
	int mtlNum;                   // sub-mtl number for multi-materials
	Color ambientLight;              // ambient light 
	int nLights;                  // number of lights;
	int rayLevel;
	int xshadeID;                    // different for every shade call in a render.
	LightDesc *atmosSkipLight;
	RenderGlobalContext *globContext;
	ShadeOutput out;                    // where a shading leaves its results

	// Reset output & render elements: 
	// param < 0 clears existing elements, leaving nElements unchanged
	// n >= 0 clears n elements & set nElements to n, default is 0
	/*! \remarks Sets the surface color output and surface transparency output to Black.
	\par Parameters:
	<b>int n = -1</b>\n\n
	By supplying a negative value this method will clear elements but leave the
	number of elements unchanged. */
	void ResetOutput( int n = -1 ) { out.Reset(n); }   

	/*! \remarks	 Returns the Class_ID of this ShadeContext. This is used to distinguish
	different ShadeContexts.
	\par Default Implementation:
	<b>{ return Class_ID(0,0); }</b> */
	virtual Class_ID ClassID() { return Class_ID(0,0); }  // to distinguish different ShadeContexts.
	/*! \remarks	 Returns TRUE if this rendering is for the material editor sample sphere
	(geometry); otherwise FALSE. */
	virtual BOOL InMtlEditor()=0; // is this rendering the mtl editor sample sphere?
	/*! \remarks Returns the state of the antialiasing switch in the renderer
	dialog - TRUE if on; FALSE if off.
	\par Default Implementation:
	<b>{return 0;}</b> */
	virtual int Antialias() {return 0;}
	/*! \remarks This method returns the projection type.
	\return  A value of 0 indicates perspective projection; a value of 1
	indicates parallel projection.
	\par Default Implementation:
	<b>{return 0;}</b> */
	virtual int ProjType() { return 0;} // returns: 0: perspective, 1: parallel
	/*! \remarks This method returns the 'i-th' light. Use data member
	<b>nLights</b> to get the total number of lights.
	\par Parameters:
	<b>int n</b>\n\n
	Specifies the light to return. */
	virtual LightDesc* Light(int n)=0;  // get the nth light. 
	/*! \remarks Returns the current time value (the position of the frame slider).
	\return  The current time. */
	virtual TimeValue CurTime()=0;      // current time value
	/*! \remarks Returns the node ID for the item being rendered or -1 if not
	set. This ID is assigned when the scene is being rendered - each node is
	simply given an ID - 0, 1, 2, 3, etc.
	\par Default Implementation:
	<b>{return -1;}</b> */
	virtual int NodeID() { return -1; }
	/*! \remarks Returns the INode pointer of the node being rendered. This
	pointer allows a developer to access the properties of the node. See
	Class INode.
	\par Default Implementation:
	<b>{ return NULL; }</b> */
	virtual INode *Node() { return NULL; }
	/*! \remarks	 Returns the evaluated object for this node. When rendering, usually one
	calls <b>GetRenderMesh()</b> to get the mesh to render. However, at certain
	times you might want to get the object itself from the node. For example,
	you could then call <b>ClassID()</b> on the object and determine its type.
	Then the object could be operated on procedurally (for instance you could
	recognize it as a true sphere, cylinder or torus). Note that this method
	will return NULL if object is motion blurred.\n\n
	For example, here is how you can check if the object is a particle system:\n\n
	\code 
	// . . .
	Object *ob = sc.GetEvalObject();
	if (ob \&\& ob-\>IsParticleSystem()) { // . . . }
	\endcode
	\par Default Implementation:
	<b>{ return NULL; }</b> */
	virtual Object *GetEvalObject() { return NULL; } // Returns the evaluated object for this node. 
	// Will be NULL if object is motion blurred  
	/*! \remarks The coordinates relative to triangular face. The barycentric
	coordinates of a point <b>p</b> relative to a triangle describe that point
	as a weighted sum of the vertices of the triangle. If the barycentric
	coordinates are <b>b0, b1</b>, and <b>b2</b>, then:\n\n
	<b>p = b0*p0 + b1*p1 + b2*p2;</b>\n\n
	where <b>p0, p1</b>, and <b>p2</b> are the vertices of the triangle. The
	Point3 returned by this method has the barycentric coordinates stored in the
	its three coordinates. These coordinates are relative to the current
	triangular face being rendered. These barycentric coordinates can be used to
	interpolate <b>any</b> quantity whose value is known at the vertices of the
	triangle. For example, if a radiosity shader had available the illumination
	values at each of the three vertices, it could determine the illumination at
	the current point using the barycentric coordinates.
	\par Default Implementation:
	<b>{ return Point3(0,0,0);}</b> */
	virtual Point3 BarycentricCoords() { return Point3(0,0,0);}  // coords relative to triangular face 
	/*! \remarks Returns the index of the face being rendered. For the
	scan-line renderer, which renders only triangle meshes, this is the index of
	the face in the Mesh data structure. This is meant for use in plug-in
	utilities such as a radiosity renderer, which stores a table of data,
	indexed on face number, in the Nodes's AppData, for use in a companion
	material. */
	virtual int FaceNumber()=0;         // 
	/*! \remarks Returns the interpolated normal (in camera space). This is the
	value of the face normal facing towards the camera. This is affected by
	<b>SetNormal()</b> below. */
	virtual Point3 Normal()=0;       // interpolated surface normal, in camera coords: affected by SetNormal()
	/*! \remarks This method will set the value of the face normal facing
	towards the camera. This may be used to temporarily perturb the normal. The
	Standard material uses this for example because it implements bump mapping.
	It changes the normal and then calls other lighting functions, etc. These
	other method then see this changed normal value. When it is done it puts
	back the previous value.
	\par Parameters:
	<b>Point3 p</b>\n\n
	The normal to set. */
	virtual void SetNormal(Point3 p) {} // used for perturbing normal
	/*! \remarks	 Returns the original surface normal (not affected by <b>SetNormal()</b>
	above.)
	\par Default Implementation:
	<b>{ return Normal(); }</b> */
	virtual Point3 OrigNormal() { return Normal(); } // original surface normal: not affected by SetNormal();
	/*! \remarks This returns the geometric normal. For triangular mesh objects
	this means the face normal. Normals are unit vectors. */
	virtual Point3 GNormal()=0;      // geometric (face) normal
	/*! \remarks This is an estimate of how fast the normal is varying. For
	example if you are doing environment mapping this value may be used to
	determine how big an area of the environment to sample. If the normal is
	changing very fast a large area must be sampled otherwise you'll get
	aliasing. This is an estimate of <b>dN/dsx, dN/dsy</b> put into a single
	value. */
	virtual float  Curve() { return 0.0f; }          // estimate of dN/dsx, dN/dsy
	/*! \remarks This method returns the unit view vector, from the camera
	towards P, in camera space. */
	virtual Point3 V()=0;            // Unit view vector: from camera towards P 
	/*! \remarks	 Sets the view vector as returned by <b>V()</b>.
	\par Parameters:
	<b>Point3 p</b>\n\n
	The view vector set. */
	virtual void SetView(Point3 p)=0;   // Set the view vector
	/*! \remarks	 This is the original view vector that was not affected by
	<b>ShadeContext::SetView()</b>.
	\par Default Implementation:
	<b>{ return V(); }</b> */
	virtual Point3 OrigView() { return V(); } // Original view vector: not affected by SetView();
	/*! \remarks This takes the current view vector and the current normal
	vector and calculates a vector that would result from reflecting the view
	vector in the surface. This returns the reflection vector. */
	virtual  Point3 ReflectVector()=0;  // reflection vector
	/*! \remarks This is similar to the method above however it calculates the
	view vector being refracted in the surface. This returns the refraction
	vector.
	\par Parameters:
	<b>float ior</b>\n\n
	The relative index of refraction between the air and the material. */
	virtual  Point3 RefractVector(float ior)=0;  // refraction vector
	/*! \remarks	 Set index of refraction.
	\par Parameters:
	<b>float ior</b>\n\n
	The index of refraction to set. This value can be any positive (non-zero)
	value.
	\par Default Implementation:
	<b>{}</b> */
	virtual void SetIOR(float ior) {} // Set index of refraction
	/*! \remarks	 Returns the index of refraction.
	\par Default Implementation:
	<b>{ return 1.0f; }</b> */
	virtual float GetIOR() { return 1.0f; } // Get index of refraction
	/*! \remarks Returns the camera position in camera space. For the 3ds Max
	renderer this will always be 0,0,0. */
	virtual Point3 CamPos()=0;       // camera position
	/*! \remarks Returns the point to be shaded in camera space. */
	virtual Point3 P()=0;            // point to be shaded;
	/*! \remarks This returns the derivative of P, relative to the pixel. This
	gives the renderer or shader information about how fast the position is
	changing relative to the screen. */
	virtual Point3 DP()=0;           // deriv of P, relative to pixel, for AA
	/*! \remarks This returns the derivative of P, relative to the pixel - same
	as above. This method just breaks it down into x and y. */
	virtual void DP(Point3& dpdx, Point3& dpdy){};  // deriv of P, relative to pixel
	/*! \remarks Returns the point to be shaded in object coordinates. 
	*/
	virtual Point3 PObj()=0;            // point in obj coords
	/*! \remarks Returns the derivative of <b>PObj()</b>, relative to the pixel. 
	*/
	virtual Point3 DPObj()=0;           // deriv of PObj, rel to pixel, for AA
	/*! \remarks Returns the object extents bounding box in object coordinates.
	*/
	virtual Box3 ObjectBox()=0;      // Object extents box in obj coords
	/*! \remarks Returns the point to be shaded relative to the object box
	where each component is in the range of -1 to +1. */
	virtual Point3 PObjRelBox()=0;      // Point rel to obj box [-1 .. +1 ] 
	/*! \remarks Returns the derivative of <b>PObjRelBox()</b>. This is the
	derivative of the point relative to the object box where each component is
	in the range of -1 to +1. */
	virtual Point3 DPObjRelBox()=0;     // deriv of Point rel to obj box [-1 .. +1 ] 
	/*! \remarks Retrieves the point relative to the screen where the lower
	left corner is 0,0 and the upper right corner is 1,1.
	\par Parameters:
	<b>Point2\& uv</b>\n\n
	The point.\n\n
	<b>Point2 \&duv</b>\n\n
	The derivative of the point. */
	virtual void ScreenUV(Point2& uv, Point2 &duv)=0; // screen relative uv (from lower left)
	/*! \remarks Returns the integer screen coordinate (from the upper left).
	*/
	virtual IPoint2 ScreenCoord()=0; // integer screen coordinate (from upper left)
	/*! \remarks	 Return the surface point at the center of the fragment in floating point
	screen coordinates. See the documentation for <b>Sampler::DoSample()</b> for
	an explanation of the use of this method. See
	Class Sampler.
	\par Default Implementation:
	<b>{ return Point2(0.0,0.0); }</b> */
	virtual Point2 SurfacePtScreen(){ return Point2(0.0,0.0); } // return the surface point in screen coords

	/*! \remarks Returns the UVW coordinates for the point.
	\par Parameters:
	<b>int channel=0;</b>\n\n
	Specifies the channel for the values. One of the following:\n\n
	<b>0</b>: Vertex Color Channel.\n\n
	<b>1</b> through <b>99</b>: Mapping Channels. */
	virtual Point3 UVW(int channel=0)=0;         // return UVW coords for point
	/*! \remarks This method returns the UVW derivatives for the point. This is
	used for filtering texture maps and antialiasing procedurals that are using
	UVW. Note that in standard 3ds Max textures, the <b>UVGen</b> class is used,
	and it calls this method itself. See the methods <b>UVGen::GetBumpDP()</b>
	for more details for using <b>UVGen</b>. If you are not using <b>UVGen</b>
	then you can use this method and <b>UVW()</b>. <b>UVW()</b> gets the UVW
	coordinates of the point and <b>DUVW()</b> gets the change in the UVWs for
	the point. This tells you a maximum change for each of UVW. This tells you
	how much of the area of the map to sample. So when you call the
	<b>Bitmap</b> method <b>GetFiltered(float u, float v, float du, float dv,
	BMM_Color_64 *ptr)</b> this tells you how big the sample should be. This
	lets you filter or average over this area to keep the map from aliasing.
	\par Parameters:
	<b>int channel=0;</b>\n\n
	Specifies the channel for the values. One of the following:\n\n
	<b>0</b>: Vertex Color Channel.\n\n
	<b>1</b> through <b>99</b>: Mapping Channels. */
	virtual Point3 DUVW(int channel=0)=0;        // return UVW derivs for point
	/*! \remarks This returns the bump basis vectors for UVW in camera space.
	Note that if you want to retrieve these bump basis vectors that are altered
	by the UVGen instance use the method <b>UVGen::GetBumpDP()</b>. Also see the
	Advanced Topics section
	~{ Materials, Textures and Maps }~ for more details on bump mapping.
	\par Parameters:
	<b>Point3 dP[3]</b>\n\n
	The bump basic vectors. <b>dP[0]</b> is a vector corresponding to the U
	direction. <b>dp[1]</b> corresponds to V, and <b>dP[2]</b> corresponds to
	W.\n\n
	<b>int channel=0;</b>\n\n
	Specifies the channel for the values. One of the following:\n\n
	<b>0</b>: Vertex Color Channel.\n\n
	<b>1</b> through <b>99</b>: Mapping Channels. */
	virtual void DPdUVW(Point3 dP[3],int channel=0)=0; // Bump vectors for UVW (camera space)

	// BumpBasisVectors: this is going to replace DPdUVW: Now compute bump vectors for 
	//  specific 2D cases only.
	//    axis = AXIS_UV, AXIS_VW, AXIS_WU    
	//    return:     0 => not implemented,  1=> implemented
	// If this function is implemented, it will be used instead of DPdUVW.
	/*! \remarks	 This method should replace DpDUVW over time but is left in place as not to
	break 3rd party plugins. If this method returns 1, that is assumed to mean
	it is implemented, and it will be used instead of DpDUVW.
	\par Parameters:
	<b>Point3 dP[2]</b>\n\n
	The bump basic vectors. <b>dP[0]</b> is a vector corresponding to the U
	direction. <b>dp[1]</b> corresponds to V, and <b>dP[2]</b> corresponds to
	W.\n\n
	<b>int axis</b>\n\n
	Specified the 2D cases for: <b>AXIS_UV</b>, <b>AXIS_VW</b>, or
	<b>AXIS_WU</b>.\n\n
	<b>int channel=0;</b>\n\n
	Specifies the channel for the values. One of the following:\n\n
	<b>0</b>: Vertex Color Channel.\n\n
	<b>1</b> through <b>99</b>: Mapping Channels.
	\par Default Implementation:
	<b>{ return 0; }</b> */
	virtual int BumpBasisVectors(Point3 dP[2], int axis, int channel=0) { return 0; } 


	virtual BOOL IsSuperSampleOn(){ return FALSE; }
	virtual BOOL IsTextureSuperSampleOn(){ return FALSE; }
	virtual int GetNSuperSample(){ return 0; }
	virtual float GetSampleSizeScale(){ return 1.0f; }

	// UVWNormal: returns a vector in UVW space normal to the face in UVW space. This can 
	// be CrossProd(U[1]-U[0],U[2]-U[1]), where U[i] is the texture coordinate
	// at the ith vertex of the current face.  Used for hiding textures on
	// back side of objects.
	/*! \remarks	 This method returns a vector in UVW space normal to the face in UVW space.
	This can be CrossProd(U[1]-U[0],U[2]-U[1]), where U[i] is the texture
	coordinate at the i-th vertex of the current face. This may be used for
	hiding textures on back side of objects.
	\par Parameters:
	<b>int channel=0;</b>\n\n
	Specifies the channel for the values. One of the following:\n\n
	<b>0</b>: Vertex Color Channel.\n\n
	<b>1</b> through <b>99</b>: Mapping Channels.
	\par Default Implementation:
	<b>{ return Point3(0,0,1); }</b> */
	virtual Point3 UVWNormal(int channel=0) { return Point3(0,0,1); }  

	// diameter of view ray at this point
	/*! \remarks	 Returns the diameter of the ray cone at the pixel point (the point it
	intersects the surface being shaded). This is a dimension in world units. As
	a ray is propagated it is updated for each new surface that is encountered.
	\par Default Implementation:
	<b>{ return Length(DP()); }</b> */
	virtual float RayDiam() { return Length(DP()); } 

	// angle of ray cone hitting this point: gets increased/decreased by curvature 
	// on reflection
	/*! \remarks	 Returns the angle of a ray cone hitting this point. It gets
	increased/decreased by curvature on reflection.\n\n
	Visualize a small pyramid, with the top at the eye point, and its sides
	running through each corner of the pixel to be rendered, then onto the
	scene. Then visualize a small cone fitting inside this pyramid. This method
	returns the angle of that cone. When rendering, if the ray cone goes out and
	hits a flat surface, the angle of reflection will always be constant for
	each pixel. However, if the ray cone hits a curved surface, the angle will
	change between pixels. This change in value give some indication of how fast
	the sample size is getting bigger.
	\par Default Implementation:
	<b>{ return 0.0f; }</b> */
	virtual float RayConeAngle() { return 0.0f; } 

	/*! \remarks This is used by the Standard material to do the reflection
	maps and the refraction maps. Given the map, and a direction from which you
	want to view it, this method changes the view vector to be the specified
	vector and evaluates the function.
	\par Parameters:
	<b>Texmap *map</b>\n\n
	The map to evaluate.\n\n
	<b>Point3 view</b>\n\n
	The view direction.
	\return  The color of the map, in r, g, b, alpha. */
	CoreExport virtual AColor EvalEnvironMap(Texmap *map, Point3 view); //evaluate map with given viewDir
	/*! \remarks Retrieves the background color and the background transparency.
	\par Parameters:
	<b>class Color \&bgCol</b>\n\n
	The returned background color.\n\n
	<b>class Color \&transp</b>\n\n
	The returned transparency.\n\n
	<b>int fogBG</b>\n\n
	Specifies you want the current atmospheric shaders to be applied to the
	background color. If TRUE the shaders are applied; if FALSE they are not. */
	virtual void GetBGColor(Color &bgcol, Color& transp, BOOL fogBG=TRUE)=0;   // returns Background color, bg transparency

	// Camera ranges set by user in camera's UI.
	/*! \remarks Returns the camera near range set by the user in the camera's
	user interface. */
	virtual float CamNearRange() {return 0.0f;}
	/*! \remarks Returns the camera far range set by the user in the camera's
	user interface. */
	virtual float CamFarRange() {return 0.0f;}

	// Transform to and from internal space
	/*! \remarks Transforms the specified point from internal camera space to
	the specified space.
	\par Parameters:
	<b>const Point3\& p</b>\n\n
	The point to transform.\n\n
	<b>RefFrame ito</b>\n\n
	The space to transform the point to. One of the following values:\n\n
	<b>REF_CAMERA</b>\n\n
	<b>REF_WORLD</b>\n\n
	<b>REF_OBJECT</b>
	\return  The transformed point, in the specified space. */
	virtual Point3 PointTo(const Point3& p, RefFrame ito)=0; 
	/*! \remarks Transforms the specified point from the specified coordinate
	system to internal camera space.
	\par Parameters:
	<b>const Point3\& p</b>\n\n
	The point to transform.\n\n
	<b>RefFrame ifrom</b>\n\n
	The space to transform the point from. One of the following values:\n\n
	<b>REF_CAMERA</b>\n\n
	<b>REF_WORLD</b>\n\n
	<b>REF_OBJECT</b>
	\return  The transformed point in camera space. */
	virtual Point3 PointFrom(const Point3& p, RefFrame ifrom)=0; 
	/*! \remarks Transform the vector from internal camera space to the
	specified space.
	\par Parameters:
	<b>const Point3\& p</b>\n\n
	The vector to transform.\n\n
	<b>RefFrame ito</b>\n\n
	The space to transform the vector to. One of the following values:\n\n
	<b>REF_CAMERA</b>\n\n
	<b>REF_WORLD</b>\n\n
	<b>REF_OBJECT</b> */
	virtual Point3 VectorTo(const Point3& p, RefFrame ito)=0; 
	/*! \remarks Transform the vector from the specified space to internal
	camera space.
	\par Parameters:
	<b>const Point3\& p</b>\n\n
	The vector to transform.\n\n
	<b>RefFrame ifrom</b>\n\n
	The space to transform the vector from. One of the following values:\n\n
	<b>REF_CAMERA</b>\n\n
	<b>REF_WORLD</b>\n\n
	<b>REF_OBJECT</b> */
	virtual Point3 VectorFrom(const Point3& p, RefFrame ifrom)=0; 
	/*! \remarks	 Transform the vector from internal camera space to the specified space
	without scaling.
	\par Parameters:
	<b>const Point3\& p</b>\n\n
	The vector to transform.\n\n
	<b>RefFrame ito</b>\n\n
	The space to transform the vector to. One of the following values:\n\n
	<b>REF_CAMERA</b>\n\n
	<b>REF_WORLD</b>\n\n
	<b>REF_OBJECT</b> */
	CoreExport virtual Point3 VectorToNoScale(const Point3& p, RefFrame ito); 
	/*! \remarks	 Transform the vector from the specified space to internal camera space
	without scaling.\n\n
	Note: This method was added to correct a problem that was occurring in 3D
	Textures when the bump perturbation vectors were transformed from object
	space to camera space, so they are oriented correctly as the object rotates.
	If the object has been scaled, this transformation causes the perturbation
	vectors to be scale also, which amplifies the bump effect. This method is
	used to rotate the perturbation vectors so they are correctly oriented in
	space, without scaling them.
	\par Parameters:
	<b>const Point3\& p</b>\n\n
	The vector to transform.\n\n
	<b>RefFrame ifrom</b>\n\n
	<b>RefFrame ifrom</b>\n\n
	The space to transform the vector from. One of the following values:\n\n
	<b>REF_CAMERA</b>\n\n
	<b>REF_WORLD</b>\n\n
	<b>REF_OBJECT</b> */
	CoreExport virtual Point3 VectorFromNoScale(const Point3& p, RefFrame ifrom); 

	// After being evaluated, if a map or material has a non-zero GBufID, it should
	// call this routine to store it into the shade context.
	/*! \remarks When a map or material is evaluated (in <b>Shade()</b>,
	<b>EvalColor()</b> or <b>EvalMono()</b>), if it has a non-zero
	<b>gbufID</b>, it should call this routine to store the <b>gbid</b> into the
	shade context.\n\n
	Note: Normally a texmap calls this method so the index would be set for all
	of the area covered by the texture. There is no reason that this has to be
	done for every pixel however. A texture could just set the ID for particular
	pixels. This could allow post processing routines (for example a glow) to
	only process part of a texture and not the entire thing. For example, at the
	beginning of texmap's <b>EvalColor()</b> one typically has code that
	does:\n\n
	<b> if (gbufid) sc.SetGBufferID(gbufid);</b>\n\n
	This takes the <b>gbufid</b> (which is in <b>MtlBase</b>) and (if it is
	non-zero) stores it into the shade context. The renderer, after evaluating
	the <b>Shade()</b> function for the material at a pixel, looks at the
	gbufferID left in the shade context, and stores it into the gbuffer at that
	pixel. So if the texmap adds another condition like\n\n
	\code
	if (inHotPortion)
	if (gbufid) sc.SetGBufferID(gbufid);
	\endcode  
	It will do it for just the chosen pixels.
	\par Parameters:
	<b>int gbid</b>\n\n
	The ID to store. */
	virtual void SetGBufferID(int gbid) { out.gbufId = gbid; }

	/*! \remarks This method is used internally. 
	*/
	virtual FILE* DebugFile() { return NULL; }

	/*! \remarks	 Returns the color of the global environment map from the given view
	direction.
	\par Parameters:
	<b>Point3 dir</b>\n\n
	Specifies the direction of view.
	\par Default Implementation:
	<b>{ return AColor(0,0,0,0); }</b> */
	virtual AColor EvalGlobalEnvironMap(Point3 dir) { return AColor(0,0,0,0); }

	/*! \remarks	 This method is used with texture maps only. If a map is multiply instanced
	within the same material, say on the diffuse channel and on the shininess
	channel, it will return the same value each time its evaluated. Its a waste
	of processor time to re-evaluate the map twice. This method allows you to
	cache the value so it won't need to be computed more than once.\n\n
	Note that the cache is automatically cleared after each ShadeContext call.
	This is used within one evaluation of a material hierarchy.
	\par Parameters:
	<b>Texmap *map</b>\n\n
	Points to the texmap storing the cache (usually the plug-ins <b>this</b>
	pointer).\n\n
	<b>AColor \&c</b>\n\n
	The color to store.
	\return  TRUE if the color was returned; otherwise FALSE.
	\par Default Implementation:
	<b>{ return FALSE; }</b>
	\par Sample Code:
	This code from <b>/MAXSDK/SAMPLES/MATERIALS/NOISE.CPP</b> and shows how the cache is retrieved and stored:\n\n
	\code
	RGBA Noise::EvalColor(ShadeContext& sc) {
	Point3 p,dp;
	if (!sc.doMaps) return black;
	AColor c;
	// If the cache exists, return the color
	if (sc.GetCache(this,c))
	return c;
	// Otherwise compute the color
	. . .
	// At the end of the eval the cache is stored
	sc.PutCache(this,c);
	}
	\endcode
	*/
	virtual BOOL GetCache(Texmap *map, AColor &c){ return FALSE; }
	/*! \remarks	 Retrieves a floating point value from the cache. See the AColor version
	above for details.
	\par Parameters:
	<b>Texmap *map</b>\n\n
	Points to the texmap storing the cache (usually the plug-ins <b>this</b>
	pointer).\n\n
	<b>float \&f</b>\n\n
	The value to store.
	\return  TRUE if the value was returned; otherwise FALSE.
	\par Default Implementation:
	<b>{ return FALSE; }</b> */
	virtual BOOL GetCache(Texmap *map, float &f) { return FALSE; }
	/*! \remarks	 Retrieves a Point3 value from the cache. See the AColor version above for
	details.
	\par Parameters:
	<b>Texmap *map</b>\n\n
	Points to the texmap storing the cache (usually the plug-ins <b>this</b>
	pointer).\n\n
	<b>Point3 \&p</b>\n\n
	The point to store.
	\return  TRUE if the value was returned; otherwise FALSE.
	\par Default Implementation:
	<b>{ return FALSE; }</b> */
	virtual BOOL GetCache(Texmap *map, Point3 &p){ return FALSE; }
	/*! \remarks	 Puts a color to the cache. See the method <b>GetCache(Texmap *map, const
	AColor \&c)</b> above for details.
	\par Parameters:
	<b>Texmap *map</b>\n\n
	Points to the texmap storing the cache (usually the plug-ins <b>this</b>
	pointer).\n\n
	<b>const AColor \&c</b>\n\n
	The color to store.
	\par Default Implementation:
	<b>{}</b> */
	virtual void PutCache(Texmap *map, const AColor &c){}
	/*! \remarks	 Puts a floating point value to the cache. See the method <b>GetCache(Texmap
	*map, const AColor \&c)</b> above for details.
	\par Parameters:
	<b>Texmap *map</b>\n\n
	Points to the texmap storing the cache (usually the plug-ins <b>this</b>
	pointer).\n\n
	<b>const float f</b>\n\n
	The floating point value to store.
	\par Default Implementation:
	<b>{}</b> */
	virtual void PutCache(Texmap *map, const float f) {}
	/*! \remarks	 Puts a floating point value to the cache. See the method <b>GetCache(Texmap
	*map, const AColor \&c)</b> above for details.
	\par Parameters:
	<b>Texmap *map</b>\n\n
	Points to the texmap storing the cache (usually the plug-ins <b>this</b>
	pointer).\n\n
	<b>const Point3 \&p</b>\n\n
	The Point3 value to store.
	\par Default Implementation:
	<b>{}</b> */
	virtual void PutCache(Texmap *map, const Point3 &p){}
	/*! \remarks	 Removes the specified cache.
	\par Parameters:
	<b>Texmap *map</b>\n\n
	Points to the texmap storing the cache (usually the plug-ins <b>this</b>
	pointer).
	\par Default Implementation:
	<b>{}</b> */
	virtual void TossCache(Texmap *map) {}
	// Generic expansion function
	/*! \remarks	This is a general purpose function that allows the API to be extended in the
	future. The 3ds Max development team can assign new <b>cmd</b> numbers and
	continue to add functionality to this class without having to 'break' the API.\n\n
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
	virtual INT_PTR Execute(int cmd, ULONG_PTR arg1=0, ULONG_PTR arg2=0, ULONG_PTR arg3=0) { return 0; } 

	// These are used to prevent self shadowing by volumetric lights
	/*! \remarks	 This method, along with <b>SetAtmosSkipLight()</b> below, are used by the
	lights to avoid self-shadowing when applying atmospheric shadows. This
	method returns a pointer to the <b>LightDesc</b> instance currently calling
	the <b>Atmosphere::Shade()</b> method when computing atmospheric
	shadows.\n\n
	Here's how they are used:\n\n
	(1) When computing the atmospheric shadows:(somewhere in
	<b>::LightDesc::Illuminate()</b>) do the following:\n\n
	<b>sc.SetAtmosSkipLight(this);</b>\n\n
	<b>sc.globContext-\>atmos-\>Shade(sc, lightPos, sc.P(), col, trans);</b>\n\n
	<b>sc.SetAtmosSkipLight(NULL);</b>\n\n
	(2) In <b>LightDesc::TraverseVolume()</b> do the following:\n\n
	<b>if (sc.GetAtmosSkipLight()==this)</b>\n\n
	<b>return;</b>
	\par Default Implementation:
	<b>{ return atmosSkipLight; }</b> */
	/*! \remarks	 This method can be used to determine from within the ShadeContext if a
	volumetric light should be prevented from generating self-shadows.
	\par Default Implementation:
	<b>{ return atmosSkipLight; }</b>\n\n
	*/
	LightDesc *GetAtmosSkipLight() { return atmosSkipLight; }
	/*! \remarks	 This method sets the <b>LightDesc</b> instance currently calling the
	<b>Atmosphere::Shade()</b> method. See <b>GetAtmosSkipLight()</b> above.
	\par Parameters:
	<b>LightDesc *lt</b>\n\n
	Points to the LightDesc to set.
	\par Default Implementation:
	<b>{ atmosSkipLight = lt; }</b> */
	/*! \remarks	 This method allows you to set the volumetric light that should be prevented
	from generating self-shadows.
	\par Parameters:
	<b>LightDesc *lt</b>\n\n
	A pointer to the light to set.
	\par Default Implementation:
	<b>{ atmosSkipLight = lt; }</b>\n\n
	*/
	void SetAtmosSkipLight(LightDesc *lt) { atmosSkipLight = lt; }

	// render element methods
	/*! \remarks	 Returns the number of render elements.
	\par Default Implementation:
	<b>{ return globContext-\>NRenderElements(); }</b> */
	virtual int NRenderElements() { 
		return globContext ? globContext->NRenderElements():0;
	}
	/*! \remarks	 Returns an interface to the 'i-th' render element.
	\par Parameters:
	<b>int n</b>\n\n
	The zero based index of the render element to return.
	\par Default Implementation:
	<b>{ return globContext-\>GetRenderElement(n); }</b> */
	virtual IRenderElement *GetRenderElement(int n) { 
		return globContext ? globContext->GetRenderElement(n) : NULL;
	}

	// diffuse illum utility, computes incoming diffuse illumination, for matte/shadow
	/*! \remarks	 Computes and returns the incoming diffuse illumination color (for
	matte/shadow). */
	CoreExport virtual Color DiffuseIllum();

    /*! \deprecated This method merely forwarded the call to <b>globContext->pToneOp</b>. For simplicity of maintenance, it has been removed;
        globContext->pToneOp should be used directly instead. Make sure to check that both globContext and pToneOp are not null. */
	MAX_DEPRECATED bool IsPhysicalSpace() const;
    /*! \deprecated This method merely forwarded the call to <b>globContext->pToneOp</b>. For simplicity of maintenance, it has been removed;
        globContext->pToneOp should be used directly instead. Make sure to check that both globContext and pToneOp are not null. */
	template< class T > MAX_DEPRECATED void ScaledToRGB( T& energy ) const;
    /*! \deprecated This method merely forwarded the call to <b>globContext->pToneOp</b>. For simplicity of maintenance, it has been removed;
        globContext->pToneOp should be used directly instead. Make sure to check that both globContext and pToneOp are not null. */
	MAX_DEPRECATED float ScaledToRGB( float energy ) const;
    /*! \deprecated This method merely forwarded the call to <b>globContext->pToneOp</b>. For simplicity of maintenance, it has been removed;
        globContext->pToneOp should be used directly instead. Make sure to check that both globContext and pToneOp are not null. */
	MAX_DEPRECATED void ScaledToRGB( );
    /*! \deprecated This method merely forwarded the call to <b>globContext->pToneOp</b>. For simplicity of maintenance, it has been removed;
        globContext->pToneOp should be used directly instead. Make sure to check that both globContext and pToneOp are not null. */
	template< class T > MAX_DEPRECATED void ScalePhysical(T& energy) const;
    /*! \deprecated This method merely forwarded the call to <b>globContext->pToneOp</b>. For simplicity of maintenance, it has been removed;
        globContext->pToneOp should be used directly instead. Make sure to check that both globContext and pToneOp are not null. */
    MAX_DEPRECATED float ScalePhysical(float energy) const;
    /*! \deprecated This method merely forwarded the call to <b>globContext->pToneOp</b>. For simplicity of maintenance, it has been removed;
        globContext->pToneOp should be used directly instead. Make sure to check that both globContext and pToneOp are not null. */
	template< class T > MAX_DEPRECATED void ScaleRGB(T& energy) const;
    /*! \deprecated This method merely forwarded the call to <b>globContext->pToneOp</b>. For simplicity of maintenance, it has been removed;
        globContext->pToneOp should be used directly instead. Make sure to check that both globContext and pToneOp are not null. */
	MAX_DEPRECATED float ScaleRGB(float energy) const;

	/*! \remarks Constructor. The data members are initialized as follows:\n\n
	<b>mode = SCMODE_NORMAL; nLights = 0; shadow = TRUE; rayLevel = 0;
	globContext = NULL; atmosSkipLight = NULL;</b> */
	ShadeContext() {
		mode = SCMODE_NORMAL; nLights = 0; shadow = TRUE;  rayLevel = 0; 
		globContext = NULL; atmosSkipLight = NULL; 
	}
};

#pragma warning(pop)

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

#include "../AColor.h"
#include "../Color.h"
#include "../Matrix3.h"
#include "../BaseInterface.h"
#include "../RenderElements.h"

#pragma warning(push)
#pragma warning(disable:4100)

class Renderer;
class Texmap;
class Atmospheric;
class ToneOperator;
class FilterKernel;
class RenderInstance;
class ShadeContext;
class Ray;
struct ISect;
class ISectList;
class ViewParams;

// Value given to blurFrame for non blurred objects  ( Object motion blur)
#define NO_MOTBLUR (-1) 

/*! \sa  Class ShadeContext, Class Renderer, Class RenderInstance, Class Texmap, Class Atmospheric, Class Matrix3, Class Point2,  Class Point3,  Class AColor, Class Color, Class IRenderElementMgr, Class IRenderElement.\n\n
\par Description:
A pointer to an instance of this class is a data member of the ShadeContext
(<b>RenderGlobalContext *globContext;</b>). This can be used by materials,
texmaps, etc. to retrieve information about the global rendering environment.
This is information such as the renderer in use, the project type for
rendering, the output device width and height, several matrices for
transforming between camera and world coordinates, the environment map, the
atmospheric effects, the current time, field rendering information, and motion
blur information.\n\n
Note that raytracing (and all shading calculations in the default renderer)
take place in camera space.\n\n
When a ray intersects on the face edge it can happen that no intersection is
returned. One way to handle this situation is to perturb the ray minimally so
it will point in a slightly different direction. This presumes that you are
fairly sure that the no intersection is probably not what you're looking for.
<b>IntersectRay()</b> is linear in the number of faces so <b>NUM_ATTEMPTS</b>
should be kept small.\n\n
\code
for (perturb=0; perturb < NUM_ATTEMPTS; perturb++)
{
	Matrix3 ptb;
	float prop = ((float)rand()) / ((float) RAND_MAX);
	// gets random rotation of up to half a degree.
	float ang = PI*prop/360.0f;
	switch (perturb%3) {
		case 0: ptb = RotateXMatrix (ang); break;
		case 1: ptb = RotateYMatrix (ang); break;
		case 2: ptb = RotateZMatrix (ang); break;
	}
	ray.dir = ptb*ray.dir;
	// try IntersectRay() again, see if you get a hit.
}
\endcode 
\par Data Members:
<b>Renderer *renderer;</b>\n\n
A pointer to the active renderer.\n\n
<b>int projType;</b>\n\n
Returns the type of projection used during rendering. One of the following
values:\n\n
<b>PROJ_PERSPECTIVE</b>\n\n
<b>PROJ_PARALLEL</b>\n\n
<b>int devWidth;</b>\n\n
The width in pixels of the output device.\n\n
<b>int devHeight;</b>\n\n
The height in pixels of the output device.\n\n
<b>float xscale;</b>\n\n
The X scale factor for mapping from world space to screen space.\n\n
<b>float yscale;</b>\n\n
The Y scale factor for mapping from world space to screen space.\n\n
<b>float xc;</b>\n\n
The X center point used in mapping from world space to screen space.\n\n
<b>float yc;</b>\n\n
The Y center point used in mapping from world space to screen space.\n\n
<b>BOOL antialias;</b>\n\n
TRUE if antialiasing is enabled; otherwise FALSE.\n\n
<b>Matrix3 camToWorld;</b>\n\n
This matrix may be used to transform coordinates from camera space to world
space.\n\n
<b>Matrix3 worldToCam;</b>\n\n
This matrix may be used to transform coordinates from world space to camera
space.\n\n
<b>float nearRange;</b>\n\n
The near range setting of the camera.\n\n
<b>float farRange;</b>\n\n
The far range setting of the camera.\n\n
<b>float devAspect;</b>\n\n
The <b>pixel</b> aspect ratio of a device pixel. This is the height /
width.\n\n
<b>float frameDur;</b>\n\n
This defines the duration of one frame in floating point units. This is used,
for example, by video post rendering where the user can stretch time. A video
post frame might be 1/2 a frame long for instance.\n\n
<b>Texmap *envMap;</b>\n\n
The environment map (which may be NULL).\n\n
<b>Color globalLightLevel;</b>\n\n
This parameter is available in release 3.0 and later only.\n\n
This is the global light level.\n\n
<b>Atmospheric *atmos;</b>\n\n
The atmosphere effects (which may be NULL).\n\n
<b>ToneOperator* pToneOp;</b>\n\n
This data member is available in release 4.0 and later only.\n\n
The tone operator, may be NULL\n\n
<b>TimeValue time;</b>\n\n
The current time.\n\n
<b>BOOL wireMode;</b>\n\n
This parameter is available in release 3.0 and later only.\n\n
TRUE if rendering in wire frame mode; otherwise FALSE.\n\n
<b>float wire_thick;</b>\n\n
This parameter is available in release 3.0 and later only.\n\n
The global wire thickness.\n\n
<b>BOOL force2Side;</b>\n\n
TRUE if force two-sided rendering enabled; otherwise FALSE.\n\n
<b>BOOL inMtlEdit;</b>\n\n
TRUE if the rendering is being done in the materials editor; otherwise
FALSE.\n\n
<b>BOOL fieldRender;</b>\n\n
TRUE if field rendering is being done; otherwise FALSE.\n\n
<b>BOOL first_field;</b>\n\n
TRUE if this is the first field; FALSE if it's the second.\n\n
<b>BOOL field_order;</b>\n\n
Determines which field is first. 0 if the even first; 1 if odd first.\n\n
<b>BOOL objMotBlur;</b>\n\n
This is used for handling object motion blur in ray-trace maps and materials.
TRUE if object motion blur is enabled; FALSE if it's disabled.\n\n
<b>int nBlurFrames;</b>\n\n
This is used for handling object motion blur in ray-trace maps and materials.
The number of object motion blur time slices. See
Class RenderInstance.  */
class RenderGlobalContext : public BaseInterfaceServer
{
protected:
	 IRenderElementMgr *mpIRenderElementMgr; // current render element manager

public:
	 Renderer* renderer;
	 int projType;  // PROJ_PERSPECTIVE or PROJ_PARALLEL
	 int devWidth, devHeight;
	 float xscale, yscale;
	 float xc,yc;
	 BOOL antialias;
	 Matrix3 camToWorld, worldToCam;
	 float nearRange, farRange;
	 float devAspect;          // PIXEL aspect ratio of device pixel H/W
	 float frameDur;
	 Texmap *envMap;
	 Color globalLightLevel;
	 Atmospheric *atmos;
	 ToneOperator* pToneOp;  // The tone operator, may be NULL
	 TimeValue time;
	 BOOL wireMode;    // wire frame render mode?
	 float wire_thick; // global wire thickness
	 BOOL force2Side;  // is force two-sided rendering enabled
	 BOOL inMtlEdit;      // rendering in mtl editor?
	 BOOL fieldRender; // are we rendering fields
	 BOOL first_field; // is this the first field or the second?
	 BOOL field_order; // which field is first: 0->even first,  1->odd first

	 BOOL objMotBlur;  // is object motion blur enabled
	 int nBlurFrames;  // number of object motion blur time slices

	 bool simplifyAreaLights; //should area lights degrade to point lights

	 // Methods
	 RenderGlobalContext() : pToneOp(NULL), mpIRenderElementMgr(NULL) { }

	 // render element methods
	 /*! \remarks	 Returns a pointer to the Render Element Manager. See
	 Class IRenderElementMgr. */
	 IRenderElementMgr *GetRenderElementMgr() { return mpIRenderElementMgr; }
	 /*! \remarks	 Sets the render element manager being used.
	 \par Parameters:
	 <b>IRenderElementMgr *pIRenderElementMgr</b>\n\n
	 Points to the render element manager to set. */
	 void SetRenderElementMgr(IRenderElementMgr *pIRenderElementMgr) { mpIRenderElementMgr = pIRenderElementMgr; }
	 /*! \remarks	 Returns the number of render elements. */
	 int NRenderElements()
	 {
		return mpIRenderElementMgr ? mpIRenderElementMgr->NumRenderElements() : 0;
	 } 
	 /*! \remarks	 Returns a pointer to the specified render element (or NULL if not found).
	 \par Parameters:
	 <b>int n</b>\n\n
	 The zero based index of the render element. */
	 IRenderElement* GetRenderElement(int n)
	 {
		return mpIRenderElementMgr ? mpIRenderElementMgr->GetRenderElement(n) : NULL;
	 }
	 
	 /*! \remarks Computes the screen space coordinates of the point passed in
	 world coordinates. This is implemented as:
	 \par Parameters:
	 <b>Point3 p</b>\n\n
	 The point to map to screen space. */
	 Point2 MapToScreen(Point3 p)
	 {  
			if (projType==1)
			{
				 return Point2(  xc + xscale*p.x , yc + yscale*p.y);
			}
			else
			{
				 if (p.z==0.0f)
						p.z = .000001f;
				 return Point2( xc + xscale*p.x/p.z,  yc + yscale*p.y/p.z);
			}
	 }

	 /*! \remarks Returns a pointer to the current anti-aliasing filter from the
	 renderer. See Class FilterKernel. */
	 virtual FilterKernel* GetAAFilterKernel(){ return NULL; }
	 /*! \remarks Returns the filter size of the current anti-aliasing filter.
	 */
	 virtual float GetAAFilterSize(){ return 0.0f; }

	 // Access RenderInstances
	 /*! \remarks Returns the number of RenderInstances. */
	 virtual int NumRenderInstances() { return 0; }
	 /*! \remarks Returns a pointer to the 'i-th' <b>RenderInstance</b>.
	 \par Parameters:
	 <b>int i</b>\n\n
	 Specifies which <b>RenderInstance</b> to return (<b>0</b> through
	 <b>NumRenderInstances()-1</b>). */
	 virtual RenderInstance* GetRenderInstance( int i ) { return NULL; }

	 // Evaluate the global environment map using ray as point of view,
	 // optionally with atmospheric effect.
	 /*! \remarks This method evaluates the global environment map using the
	 specified ray as a point of view, and returns the resulting color.
	 \par Parameters:
	 <b>ShadeContext \&sc</b>\n\n
	 The shade context.\n\n
	 <b>Ray \&r</b>\n\n
	 Defines the direction of view of the environment. See
	 Class Ray.\n\n
	 <b>BOOL applyAtmos</b>\n\n
	 TRUE if atmospheric effects should be considered; otherwise FALSE.
	 \par Default Implementation:
	 <b>{ return AColor(0.0f,0.0f,0.0f,1.0f); }</b> */
	 virtual AColor EvalGlobalEnvironMap(ShadeContext &sc, Ray &r, BOOL applyAtmos)
	 { 
			return AColor(0.0f,0.0f,0.0f,1.0f); 
	 }

	 /*! \remarks This method takes the specified <b>ray</b> and intersects it
	 with the single RenderInstance <b>inst</b>.
	 \par Parameters:
	 <b>RenderInstance *inst</b>\n\n
	 The render instance to intersect. The Mesh may be retrieved via <b>Mesh\& m
	 = *(inst-\>mesh);</b>\n\n
	 <b>Ray\& ray</b>\n\n
	 Defines the direction to check. This is the point to look from, and a normal
	 vector specifying the direction to look. See Class Ray.\n\n
	 <b>ISect \&isct</b>\n\n
	 The information about the first opaque object hit by the ray is returned
	 here. See Structure ISect.\n\n
	 <b>ISectList \&xpList</b>\n\n
	 The list of transparent objects that are intersected on the way to the
	 opaque one are returned here. See Class ISectList.\n\n
	 <b>BOOL findExit</b>\n\n
	 TRUE to compute the exit point; FALSE to not compute it. Once a ray has been
	 intersected with a transparent object and you want to find out where the
	 refracted ray leaves the object, this parameter may be set to TRUE. This
	 allows the ray to look at the inside faces of the object and compute the
	 intersection point at exit. */
	 virtual void IntersectRay(RenderInstance *inst, Ray& ray, ISect &isct, ISectList &xpList, BOOL findExit) {}
	 /*! \remarks This method takes the specified <b>ray</b> and intersects it
	 with the entire 3ds Max scene.
	 \par Parameters:
	 <b>Ray \&ray</b>\n\n
	 Defines the direction to check. This is the point to look from, and a normal
	 vector specifying the direction to look. See Class Ray.\n\n
	 <b>int skipID</b>\n\n
	 This specifies an ID (from <b>RenderInstance::nodeID</b>) that is skipped in
	 the intersection computations. This is used to prevent self
	 intersection.\n\n
	 <b>ISect \&hit</b>\n\n
	 The information about the first opaque object hit by the ray is returned
	 here. See Structure ISect.\n\n
	 <b>ISectList \&xplist</b>\n\n
	 The list of transparent objects that are intersected on the way to the
	 opaque one are returned here. See Class ISectList.\n\n
	 <b>int blurFrame = NO_MOTBLUR</b>\n\n
	 <b>NO_MOTBLUR</b> is used for non-motion blurred objects. If this is not
	 equal to <b>NO_MOTBLUR</b>, it should be in the range <b>0</b> to
	 <b>nBlurFrames-1</b>. In that case, this method will only consider blur
	 objects corresponding to that blur sub-frame.\n\n
	 When object motion blur is turned on, for each object, several objects are
	 generated. Each of these objects is given a number. This corresponds to the
	 value <b>RenderInstance::objMotBlurFrame</b>.\n\n
	 This method will always intersect objects that aren't motion blurred.
	 However, if this is set to a number other than <b>NO_MOTBLUR</b>, then when
	 it comes to a motion blurred object, it will only look at the sub-object
	 corresponding to the specified slice in time.\n\n
	 This may be used to do a kind of dither where for each of the sub-samples,
	 this number is randomly selected. In this way the different motion blur
	 slices will basically blur together and give a motion blurred ray trace
	 result. */
	 virtual BOOL IntersectWorld(Ray &ray, int skipID, ISect &hit, ISectList &xplist, int blurFrame = NO_MOTBLUR) { return FALSE; }

	 /*! \remarks	 Returns a pointer to a class which describes the properties of a view being
	 rendered.. See Class ViewParams. */
	 virtual ViewParams* GetViewParams() { return NULL; } 

	 /*! \remarks This method is used internally.
	 \par Default Implementation:
	 <b>{ return NULL; };</b> */
	 virtual FILE* DebugFile() { return NULL; }

	 // Generic expansion function
	/*! \remarks	This is a general purpose function that allows the API to be extended in the
	future. The 3ds Max development team can assign new <b>cmd</b> numbers and
	continue to add functionality to this class without having to 'break' the API.
	\par Parameters:
	<b>int cmd</b>\n\n
	The index of the command to execute.\n\n
	<b>ULONG arg1=0</b>\n\n
	Optional argument 1. See the documentation where the <b>cmd</b> option is
	discussed for more details on these parameters.\n\n
	<b>ULONG arg2=0</b>\n\n
	Optional argument 2.\n\n
	<b>ULONG arg3=0</b>\n\n
	Optional argument 3.
	\return  An integer return value. See the documentation where the <b>cmd</b>
	option is discussed for more details on the meaning of this value.
	\par Default Implementation:
	<b>{ return 0; }</b> */
	 virtual INT_PTR Execute(int cmd, ULONG_PTR arg1=0, ULONG_PTR arg2=0, ULONG_PTR arg3=0) { return 0; } 
};

/*! \sa Class RenderGlobalContext, Class RenderInstance, Class ISectList.
\remarks This structure is available in release 2.0 and later only. \n
This structure is updated by the RenderGlobalContext::IntersectWorld() and 
RenderGlobalContext::IntersectRay() methods. It stores information about 
the intersection of a ray and a single triangle in a mesh.
*/
struct ISect: public MaxHeapOperators {
	 /*! The ray that was intersected has a unit vector specifying a direction.
	 The ray defines an infinite line in the specified direction. If you take
	 this value t and multiply it by the direction itself it gives a point on
	 the ray. This specifies a distance along the vector. For instance if
	 this way 5.0, the point would 5.0 units along the ray vector. This
	 is the point of intersection along the ray.  */
	 float t;   // ray parameter
	 /*! TRUE if the ray is exiting the object; otherwise FALSE.  */
	 BOOL exit;
	 /*! TRUE if the ray hits a back face of the triangle; otherwise FALSE.  */
	 BOOL backFace;
	 /*! Points to the render instance associated with this triangle hit.  */
	 RenderInstance *inst;
	 /*! The face number of the triangle.  */
	 int fnum;  // face number
	 /*! The barycentric coordinates of the intersection.  */
	 Point3 bc; // barycentric coords
	 /*! The intersection point in object coordinates.  */
	 Point3 p;  // intersection point, object coords
	 /*! The intersection point in camera coordinates.  */
	 Point3 pc;  // intersection point, camera coords
	 /*! The material requirements of the intersected face. See \ref materialRequirementsFlags.  */
	 ULONG matreq;
	 /*! The material number for a multi-material.  */
	 int mtlNum;
	 /*! Points to the next ISect structure in the list.  */
	 ISect *next;
	 };

/*! \sa  Structure ISect.\n\n
\par Description:
It provides a list of ray / triangle intersection structures (<b>struct
ISect</b>). Methods are available for initializing the list, adding to the
list, and selectively removing items from the list.\n\n
All methods of this class are implemented by the system.\n\n
Note the following global functions may be used to create and free instances of
this class. */
class ISectList: public MaxHeapOperators {
	 public:
	 ISect *first;
	 /*! \remarks Constructor. The list is set to empty. */
	 ISectList() { first = NULL; }
	 /*! \remarks Destructor. The list is freed. <b>first</b> is set to NULL. */
	 ~ISectList() { Init(); }
	 /*! \remarks Returns TRUE if the list is empty; otherwise FALSE. */
	 BOOL IsEmpty() { return first?0:1; }
	 /*! \remarks Adds the specified <b>ISect</b> to the list with the most
	 distant layers first.
	 \par Parameters:
	 <b>ISect *is</b>\n\n
	 Points to the <b>ISect</b> structure to add to the list. */
	 CoreExport void Add(ISect *is); 
	 /*! \remarks Removes the <b>ISect</b> structures from the list whose
	 <b>t</b> values are less than or equal to the specified value <b>a</b>.
	 \par Parameters:
	 <b>float a</b>\n\n
	 The ray parameter used for comparison. */
	 CoreExport void Prune(float a); 
	 /*! \remarks Deletes any items from the list and sets <b>first</b> to NULL.
	 */
	 CoreExport void Init();
};

/*! \remarks This global function is available in release 2.0 and later
only.\n\n
Returns a pointer to a new <b>ISect</b> structure. */
CoreExport ISect *GetNewISect();
/*! \remarks This global function is available in release 2.0 and later
only.\n\n
Deletes the <b>ISect</b> structure whose pointer is passed.
\par Parameters:
<b>ISect *is</b>\n\n
Points to the <b>ISect</b> structure to free.
\par Data Members:
<b>ISect *first;</b>\n\n
A pointer to the first intersection.  */
CoreExport void DiscardISect(ISect *is);

#pragma warning(pop)
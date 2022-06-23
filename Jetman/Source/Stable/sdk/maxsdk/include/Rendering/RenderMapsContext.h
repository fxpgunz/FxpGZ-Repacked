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

#include "../MaxHeap.h"
#include "../Box2.h"

class INode;
class ViewParams;
struct SubRendParams;
class Matrix3;
class Box3;
class RenderGlobalContext;
class Bitmap;
class Point4;

/*! 
An instance of this class is passed into the MtlBase::BuildMaps()
method. This is used for the Mirror and Automatic Cubic maps. These maps
callback to methods of this class to perform a rendering from a particular
view. Sample code using these methods is available in
<b>/MAXSDK/SAMPLES/MATERIALS/MIRROR.CPP</b> and <b>ACUBIC.CPP</b>. All methods
of this class are implemented by the system.  
\see  Class INode, Class ViewParams, Structure SubRendParams, Class Matrix3,  Class Box3,  Class Point4, Class Bitmap, Class RenderGlobalContext.*/
class RenderMapsContext: public MaxHeapOperators
{ 
public:
	/*! Destructor. */
	virtual ~RenderMapsContext() { }
	/*! Returns the INode pointer of the node being rendered. This
	pointer allows a developer to access the properties of the node. See
	Class INode. */
	virtual INode *GetNode()=0;
	/*! Returns the node ID for the item being rendered or -1 if not
	set. This ID is assigned when the scene is being rendered - each node is
	simply given an ID: 0, 1, 2, 3, etc. The NodeRenderID() is simply a
	number automatically assigned to every node being rendered so that they can
	be differentiated in texture maps such as the Auto-cubic, which needs to
	store a cubic map for each node it is applied to. */
	virtual int NodeRenderID()=0;
	/*! Retrieves the current view dependent parameters.
	\par Parameters:
	<b>ViewParams \&vp</b>\n\n
	The <b>ViewParams</b> instance to update. */
	virtual void GetCurrentViewParams(ViewParams &vp)=0;
	/*! Retrieves the sub-render parameters.
	\par Parameters:
	<b>SubRendParams \&srp</b>\n\n
	The <b>SubRendParams</b> instance to update. */
	virtual void GetSubRendParams(SubRendParams &srp)=0;
	/*! Returns the current sub material index or -1 if at node
	material level. */
	virtual int SubMtlIndex()=0;
	virtual void SetSubMtlIndex(int mindex)=0;
	/*! Computes the plane containing the current material or sub
	material. This is used by the Flat Mirror material.
	\par Parameters:
	<b>float pl[4]</b>\n\n
	The plane containing the current material or sub material. The four float
	values in <b>pl[4]</b> represent the plane equation. If you call the four
	values A,B,C and D, then the plane equation of the plane is Ax + By + cZ +
	D = 0. */
	virtual void FindMtlPlane(float pl[4])=0;
	/*! This method computes the rectangle in screen space of the
	specified material. This uses the viewTM, but assumes the remaining view
	params (devWidth, devHeight, devAspect, fov) are the same as the main
	render. This method is specific to the Flat Mirror material.
	\par Parameters:
	<b>Rect \&sbox</b>\n\n
	The resulting 2D box.\n\n
	<b>Matrix3* viewTM=NULL</b>\n\n
	The view matrix.\n\n
	<b>int mtlIndex=-1</b>\n\n
	The material index, or -1 if a node level material. */
	virtual void FindMtlScreenBox(Rect &sbox, Matrix3* viewTM=NULL, int mtlIndex=-1)=0;
	/*! \remarks	This method computes the bounding box in camera space of the object
	associated with the reflection or refraction map being built in a call to a
	map's BuildMaps() method. */
	virtual Box3 CameraSpaceBoundingBox()=0;
	/*! \remarks	This method computes the bounding box in object space of the object
	associated with the reflection or refraction map being built in a call to a
	map's BuildMaps() method. */
	virtual Box3 ObjectSpaceBoundingBox()=0;
	/*! \remarks	This method returns the object to world transformation. */
	virtual Matrix3 ObjectToWorldTM()=0;
	/*! \remarks	Returns a pointer to a class that describes properties of the rendering
	environment. */
	virtual RenderGlobalContext *GetGlobalContext() { return NULL; }
	/*! Renders the scene and stores in the result into <b>bm</b>.
	\par Parameters:
	<b>Bitmap *bm</b>\n\n
	The Bitmap to render the result to. The properties of this bitmap define
	the properties of the render (such as the width and height).\n\n
	<b>ViewParams \&vp</b>\n\n
	The ViewParams.\n\n
	<b>SubRendParams \&srp</b>\n\n
	The SubRendParams.\n\n
	<b>Point4 *clipPlanes=NULL</b>\n\n
	This is a pointer to an array of Point4s, each of which represents a clip
	plane. If it is non-null, the renderer will clip all objects against these
	planes in addition to the normal left/right/top/bottom clipping. This is
	used by the Mirror material to clip away stuff that is behind the mirror.
	If not needed this may default to NULL.\n\n
	<b>int nClipPlanes=0</b>\n\n
	The number of clipping planes above. A maximum of 6 is possible.
	\return  Nonzero on success; otherwise zero. */
	virtual	int Render(Bitmap *bm, ViewParams &vp, SubRendParams &srp, Point4 *clipPlanes=NULL, int nClipPlanes=0)=0; 
	/*! \remarks	This is a general purpose function that allows the API to be extended in the
	future. The 3ds Max development team can assign new <b>cmd</b> numbers and
	continue to add functionality to this class without having to 'break' the API.
	\par Parameters:
	<b>int cmd</b>\n\n
	The index of the command to execute.\n\n
	<b>ULONG_PTR arg1=0</b>\n\n
	Optional argument 1. See the documentation where the <b>cmd</b> option is
	discussed for more details on these parameters.\n\n
	<b>ULONG_PTR arg2=0</b>\n\n
	Optional argument 2.\n\n
	<b>ULONG_PTR arg3=0</b>\n\n
	Optional argument 3.
	\return  An integer return value. See the documentation where the <b>cmd</b>
	option is discussed for more details on the meaning of this value. */
	virtual INT_PTR Execute(int /*cmd*/, ULONG_PTR /*arg1*/=0, ULONG_PTR /*arg2*/=0, ULONG_PTR /*arg3*/=0) { return 0; } 
};	
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

#include "../BaseInterface.h"
#include "../Matrix3.h"

#define PROJ_PERSPECTIVE 0   // perspective projection
#define PROJ_PARALLEL 1      // parallel projection

/*! Describes the properties of a view that is being rendered. These are
properties such as the type of view (parallel or perspective), its clipping
distances, width, height, zoom factor, field-of-view, etc.
\par Data Members:
<b>Matrix3 prevAffineTM;</b>\n\n
This is the world space to camera space transformation matrix computed 2 ticks
before the <b>affineTM</b> matrix below.\n\n
<b>Matrix3 affineTM;</b>\n\n
This matrix will take a point in world space and convert it to camera space (or
world to view space if it's a viewport). The camera coordinates are set up
looking down the -Z axis, X is to the right, and Y is up.\n\n
<b>int projType;</b>\n\n
One of the following values:\n\n
<b>PROJ_PERSPECTIVE</b>\n\n
The view is a perspective projection.\n\n
<b>PROJ_PARALLEL</b>\n\n
The view is a parallel projection.\n\n
<b>float hither, yon;</b>\n\n
The hither and yon clipping distances.\n\n
<b>float distance;</b>\n\n
The distance from the view point to the image (view) plane.\n\n
<b>float zoom;</b>\n\n
The zoom factor of the viewport for parallel projection. The zoom factor gives
the amount of magnification relative to a standard view width of 400 pixels.
This is best explained via the following code fragment:
ComputeViewParams() computes the projection factors for a given view,
and MapToScreen() applies these factors to map a point from 3D camera
coordinates to 2D screen coordinates.\code

#define VIEW_DEFAULT_WIDTH ((float)400.0)
void SRendParams::ComputeViewParams(const ViewParams&vp)
{
	if (vp.projType == PROJ_PERSPECTIVE) {
		float fac = -(float)(1.0 / tan(0.5*(double)vp.fov));
		xscale = fac*dw2;		 // dw2 = float(devWidth)/2.0
		yscale = -devAspect*xscale;
	}
	else {
		xscale = (float)devWidth/(VIEW_DEFAULT_WIDTH*vp.zoom);
		yscale = -devAspect*xscale;
	}
}

Point2 SRendParams::MapToScreen(Point3 p)
{
	Point2 s;
	if (proj_type==PROJ_PERSPECTIVE) {
		s.x = dw2 + xscale*p.x/p.z;
		s.y = dh2 + yscale*p.y/p.z;
	}
	else {
		s.x = dw2 + xscale*p.x;
		s.y = dh2 + yscale*p.y;
	}
	return s;
}
\endcode 
<b>float fov;</b>\n\n
Field of view in radians for perspective projections.\n\n
<b>float nearRange;</b>\n\n
The near environment range setting (used for fog effects).\n\n
<b>float farRange;</b>\n\n
The far environment setting (used for fog effects).  
 \see  Class Matrix3, Class Renderer, Class RendParams.*/
class ViewParams : public BaseInterfaceServer {
public:
	Matrix3 prevAffineTM; // world space to camera space transform 2 ticks previous 
	Matrix3 affineTM;  // world space to camera space transform
	int projType;      // PROJ_PERSPECTIVE or PROJ_PARALLEL
	float hither,yon;
	float distance; // to view plane
	// Parallel projection params
	float zoom;  // Zoom factor 
	// Perspective params
	float fov; 	// field of view
	float nearRange; // for fog effects
	float farRange;  // for fog effects
	ViewParams() : distance(0.0f), farRange(0.0f), hither(0.0f), zoom(0.0f), 
		fov(0.0f), yon(0.0f), projType(0), nearRange(0.0f) {};
	// Generic expansion function
	/*! This is a general purpose function that allows the API to be extended in the
	future. The 3ds Max development team can assign new <b>cmd</b> numbers and
	continue to add functionality to this class without having to 'break' the API.
	\par Parameters:
	<b>int cmd</b>\n\n
	The index of the command to execute.\n\n
	<b>ULONG_PTR  arg1=0</b>\n\n
	Optional argument 1. See the documentation where the <b>cmd</b> option is
	discussed for more details on these parameters.\n\n
	<b>ULONG_PTR  arg2=0</b>\n\n
	Optional argument 2.\n\n
	<b>ULONG_PTR  arg3=0</b>\n\n
	Optional argument 3.
	\return  An integer return value. See the documentation where the <b>cmd</b>
	option is discussed for more details on the meaning of this value. */
	virtual INT_PTR Execute(int /*cmd*/, ULONG_PTR /*arg1*/=0, ULONG_PTR /*arg2*/=0, ULONG_PTR /*arg3*/=0) { return 0; } 
};


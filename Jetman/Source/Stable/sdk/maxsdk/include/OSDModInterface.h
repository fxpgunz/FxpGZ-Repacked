//**************************************************************************/
// Copyright (c) 2015 Autodesk, Inc.
// All rights reserved.
// 
//  Use of this software is subject to the terms of the Autodesk license 
//  agreement provided at the time of installation or download, or which 
//  otherwise accompanies this software in either electronic or hard copy form.
//**************************************************************************/

#pragma once
#include "OSDParamInterface.h"

/** \file OSDModInterface.h

		This header file provides an interface into Max's OpenSubdiv Modifier for setting certain parameters.
		It derives from the OpenSubdivParameters interface, which provides read access (see OSDParamInterface.h).
*/

#define OSD_MODIFIER_INTERFACE Interface_ID(0x108c71c7, 0x23f0641a)

/** \class OSDModifierInterface

	If a feature needs to determine and/or set the OpenSubdiv modifier's parameters, it can use the GetInterface method
	to query the Modifier for the OSD_MODIFIER_INTERFACE interface.  If the Modifier is an OpenSubdiv modifier, it will
	return a pointer to this interface class, otherwise it will return NULL.

	Once you have a valid pointer, you can use this class to retrieve and set the parameters for the OpenSubdiv modifier

*/

class OSDModifierInterface : public OpenSubdivParameters {
public:
	/** \enum OperationMode
		These enums tell whether to use CPU or GPU computation for OpenSubdiv
	*/
	enum OperationMode {
		OPMODE_CPU=0,
		OPMODE_GPU
	};

	OSDModifierInterface() {}
	virtual ~OSDModifierInterface() {}

	/** Set the OpenSubdiv Vertex Boundary parameter
		\param type - The OpenSubdiv VertexBoundary type to be used; see enums in OSDParamInterface.h
	*/
	virtual void SetInterpolateBoundaryVert(OpenSubdivParameters::VertexBoundary type) = 0;

	/** Set the OpenSubdiv Varying Boundary parameter
		\param type - The OpenSubdiv VaryingBoundary type to be used; see enums in OSDParamInterface.h
	*/
	virtual void SetInterpolateBoundaryFVar(OpenSubdivParameters::VaryingBoundary type) = 0;

	/** Set the OpenSubdiv Crease Mode parameter
		\param type - The OpenSubdiv CreaseMode type to be used; see enums in OSDParamInterface.h
	*/
	virtual void SetCreaseMethod(OpenSubdivParameters::CreaseMode type) = 0;

	/** Set the OpenSubdiv Smooth Triangle parameter
		\param sw - true if OpenSubdiv is to smooth triangles in its processing
	*/
	virtual void SetSmoothTriangle(bool sw) = 0;

	/** Set the OpenSubdiv Propagate Corners parameter
		\param sw - true if OpenSubdiv is to propagate corners in its processing
	*/
	virtual void SetPropagateCorners(bool sw) = 0;

	/** Set the OpenSubdiv Adaptive parameter
		\param sw - true if OpenSubdiv is to use adaptive subdivision in its processing
	*/
	virtual void SetAdaptive(bool sw) = 0;

	/** Set the operation mode.
		\param mode - The operation mode (see enums above)
	*/
	virtual void SetOperationMode(OSDModifierInterface::OperationMode mode) = 0;

	/** Set the uniform tessellation level of this object for the renderer.
		\param t - The time at which to set
		\param level - The number of iterations
	*/
	virtual void SetRenderTessellationLevel(TimeValue t, unsigned int level) = 0;

	/** Set the general uniform tessellation level of this object.
		\param t - The time at which to set
		\param level - The number of iterations
	*/
	virtual void SetGeneralTessellationLevel(TimeValue t, unsigned int level) = 0;

	/** Set the OpenSubdiv Adaptive Tessellation level parameter
		\param t - The time at which to set
		\param level - The maximum number of iterations for adaptive tessellation
	*/
	virtual void SetAdaptiveTessellationLevel(TimeValue t, unsigned int level) = 0;
};

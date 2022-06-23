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
#include "../Color.h"

#pragma warning(push)
#pragma warning(disable:4100)

/*! This class is passed into the method ObjLightDesc::Update(). The methods
of this class are implemented by the 3ds Max scanline renderer. Developer
creating other renderer plug-ins may choose to implement the methods of this
class if they wish to use the same architecture. Developers who wish to take
advantage of the 3ds Max volumetric light effects should implement the methods
of this class. The volumetric lights are set up to work with this mechanism.
\see  Class ObjLightDesc.
 */
class RendContext: public MaxHeapOperators
{
public:
	/*! Destructor. */
	virtual ~RendContext() {}
	/*! This method is used to update the progress bar and check the keyboard and
	mouse for user cancellation. A plug-in renderer should override this
	method by calling the RendProgressCallback::Progress() method on the
	RendProgressCallback passed in to the Renderer::Render()
	method.
	\par Parameters:
	\param done	This is the number completed so far.
	\param total This is the total number of things to complete.
	\return  Nonzero to continue; zero if the user has canceled. */
	virtual int Progress(int done, int total) const { return 1; }
	//! \brief	This is a multiplier that scales the brightness of all scene lights.
	/*! it doesn't affect the ambient light level. It is included in
	RendContext so the lights can use it to multiply times the light's color. */
	virtual Color GlobalLightLevel() const = 0;
};

#pragma warning(pop)
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

#include "CoreExport.h"
#include <AnimatableInterfaceIDs.h>

// forward declarations
class ReferenceTarget;

#define I_REFTARGWRAPPINGREFTARG (I_USERINTERFACE + 0x00004003) 

//! \brief An interface for ReferenceTargets that wrap ReferenceTargets, where the wrapping ReferenceTarget provides UI/storage on top of the wrapped ReferenceTarget
class CoreExport IRefTargWrappingRefTarg
{
public:
	//! \brief Returns the wrapped ReferenceTarget.
	/*!  
    \param wrappingObject The wrapper for which to potentially resolve the wrapped object.
	\param recurse - If true, the implementation should check to see if the wrapped ReferenceTarget derives from IRefTargWrappingRefTarg, and if so recurse into that object.
	\return The wrapped ReferenceTarget.
	*/
    static ReferenceTarget* GetWrappedObject(ReferenceTarget* wrappingObject, bool recurse);

	//! \brief Returns the wrapped ReferenceTarget.
	/*!  
	\param recurse - If true, the implementation should check to see if the wrapped ReferenceTarget derives from IRefTargWrappingRefTarg, and if so recurse into that object.
	\return The wrapped ReferenceTarget.
	*/
	virtual ReferenceTarget* GetWrappedObject(bool recurse) const = 0;
};

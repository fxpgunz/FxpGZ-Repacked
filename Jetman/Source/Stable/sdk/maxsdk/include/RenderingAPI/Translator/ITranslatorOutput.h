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

#include <assert1.h>
#include <Noncopyable.h>

#include <memory>

namespace MaxSDK
{;
namespace RenderingAPI
{;

//==================================================================================================
// class ITranslatorOutput
//
/*! Base class for all translator outputs.

    As the base class for translator outputs, this class is used to safely perform dynamic casts. It contains no public methods.
*/
class ITranslatorOutput :
    public MaxSDK::Util::Noncopyable
{
public:

    // No public methods

protected:

    // Protected destructor prevents accidental deletion using this interface. Translator outputs must be deleted using the deleter specified
    // when initializing the shared_ptr.
    virtual ~ITranslatorOutput() {}
};

}}	// namespace 


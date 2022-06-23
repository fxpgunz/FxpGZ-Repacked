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

#include <maxheap.h>

namespace MaxSDK
{;
namespace RenderingAPI
{;

//==================================================================================================
// class IRenderSession
//
/*! Common base class for IOfflineRenderSession and IInteractiveRenderSession

    A render session is a temporary object which manages the render processing for renderer plugin. Whereas the renderer plugin manages the user-visible
    parameters and file I/O, the session manages the act of rendering. A session is created when a user starts rendering, and then destroyed when
    rendering is finished - enabling simple lifetime control for the contents of the renderer.

    Multiple render sessions may exist, in parallel, for a single renderer plugin. This enables, for example, the material editor to render previews
    while an interactive (ActiveShade) session is active. 
 */
class IRenderSession :
    public MaxHeapOperators
{
public:

    virtual ~IRenderSession() {}
};

}}	// namespace 


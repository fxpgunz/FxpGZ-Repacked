//////////////////////////////////////////////////////////////////////////////
//
//  Copyright 2014 Autodesk, Inc.  All rights reserved.
//
//  Use of this software is subject to the terms of the Autodesk license 
//  agreement provided at the time of installation or download, or which 
//  otherwise accompanies this software in either electronic or hard copy form.   
//
//////////////////////////////////////////////////////////////////////////////
#pragma once

namespace MaxSDK 
{

class IInteractiveRenderer;
//! \deprecated in 3ds Max 2017. All methods of IAbortableRenderer have been moved to class IInteractiveRenderer.
typedef MAX_DEPRECATED IInteractiveRenderer IAbortableRenderer;

}

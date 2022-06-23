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

// The contents of this header file (render.h) was split into multiple header files, one for each class, in 3ds Max 2017.
// render.h is still provided for legacy support.
#include "Rendering/IInteractiveRenderer.h"
#include "Rendering/IIRenderMgr.h"

// -----------------------------------------------------------------------------
//
//  Action Table Ids for the default ActiveShade renderer
//
#define ID_IRENDER_PRESHADE             40601
#define ID_IRENDER_RESHADE              40602
#define ID_IRENDER_AUTOMATIC_PRESHADE   40603
#define ID_IRENDER_AUTOMATIC_RESHADE    40604
#define ID_IRESHADE_TOGGLE_TOOLBAR_DOCKED 40710
#define ID_IRESHADE_ACT_ONLY_MOUSE_UP   40714

//! \deprecated in 3ds Max 2017. IInteractiveRender is now queried through Renderer::GetIInteractiveRender(), rather than through
//! the interface mechanism.
extern const MAX_DEPRECATED int I_RENDER_ID;


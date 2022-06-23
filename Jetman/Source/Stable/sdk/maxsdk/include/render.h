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
#include "Rendering/FrameRendParams.h"
#include "Rendering/IRendParams.h"
#include "Rendering/IScanRenderer.h"
#include "Rendering/RendContext.h"
#include "Rendering/Renderer.h"
#include "Rendering/RenderInstance.h"
#include "Rendering/RenderMapsContext.h"
#include "Rendering/RendParamDlg.h"
#include "Rendering/RendParams.h"
#include "Rendering/RendPickProc.h"
#include "Rendering/RendProgressCallback.h"
#include "Rendering/SubRendParams.h"
#include "Rendering/ViewParams.h"

#define DONT_CLIP 1.0E38f

//! \deprecated in 3ds Max 2017. All methods of IRendererRequirements have been moved to class Renderer.
typedef MAX_DEPRECATED Renderer IRendererRequirements;

// Provide for legacy code compatibility, but ideally shouldn't be used - call Renderer::HasRequirement directly.
inline bool RendererHasRequirement(Renderer* renderer, Renderer::Requirement requirement) 
{
	return (renderer != NULL) ? renderer->HasRequirement(requirement) : false;
}


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
{;
namespace RenderingAPI
{;

inline bool IRenderSettingsContainer::GetIsMEditRender() const
{
    return (GetRenderTargetType() == RenderTargetType::MaterialEditor);
}

}}	// namespace



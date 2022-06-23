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

namespace MaxSDK
{;
namespace RenderingAPI
{;

inline TranslationResult::TranslationResult()
    : m_value(Success)
{

}

inline TranslationResult::TranslationResult(const State val)
    : m_value(val)
{

}

inline TranslationResult::operator State() const
{
    return m_value;
}

}}	// namespace 

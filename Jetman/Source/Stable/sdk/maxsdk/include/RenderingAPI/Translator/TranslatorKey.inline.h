//////////////////////////////////////////////////////////////////////////////
//
//  Copyright 2015 Autodesk, Inc.  All rights reserved.
//
//  Use of this software is subject to the terms of the Autodesk license 
//  agreement provided at the time of installation or download, or which 
//  otherwise accompanies this software in either electronic or hard copy form.   
//
//////////////////////////////////////////////////////////////////////////////
#include <memory.h>

namespace MaxSDK
{;
namespace RenderingAPI
{;

inline TranslatorKey::TranslatorKey(const size_t hash)
	: m_hash(hash)
{

}

inline size_t TranslatorKey::get_hash() const
{
    return m_hash;
}

}}	// namespace 


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

inline TranslatorStatistics::TranslatorStatistics()
    : mNumLights(0),
    mNumGeomObjects(0),
    mNumFaces(0)
{

}

inline size_t TranslatorStatistics::GetNumLights() const
{
    return mNumLights;
}

inline size_t TranslatorStatistics::GetNumGeomObjects() const
{
    return mNumGeomObjects;
}

inline size_t TranslatorStatistics::GetNumFaces() const
{
    return mNumFaces;
}

inline void TranslatorStatistics::AddLights(const size_t numLights)
{
    mNumLights += numLights;
}

inline void TranslatorStatistics::AddGeomObject(const size_t numGeomObjects)
{
    mNumGeomObjects += numGeomObjects;
}

inline void TranslatorStatistics::AddFaces(const size_t numFaces)
{
    mNumFaces += numFaces;
}


}}	// namespace 



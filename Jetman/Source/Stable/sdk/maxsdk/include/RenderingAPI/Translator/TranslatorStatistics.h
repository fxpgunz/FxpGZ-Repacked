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

//==================================================================================================
// class TranslatorStatistics
//
/*! Container for translation statistics.

    Simple container with which translation statistics are accumulated.
*/
class TranslatorStatistics
{
public:

    //! Initializes all statistics to 0.
    TranslatorStatistics();

    //! The total number of lights.
    size_t GetNumLights() const;
    //! The total number of geometric objects.
    size_t GetNumGeomObjects() const;
    //! The total number of faces.
    //! \remark The exact meaning of <i>faces</i> is defined by the renderer. It could triangles, quads, lollipops, or else.
    size_t GetNumFaces() const;

    //! Adds the given number of lights to the statistics.
    void AddLights(const size_t numLights);
    //! Adds the given number of geometric objects to the statistics.
    void AddGeomObject(const size_t numGeomObjects);
    //! Adds the given number of faces to the statistics.
    //! \remark The exact meaning of <i>faces</i> is defined by the renderer. It could triangles, quads, lollipops, or else.
    void AddFaces(const size_t numFaces);

private:

    size_t mNumLights;
    size_t mNumGeomObjects;
    size_t mNumFaces;
};

}}	// namespace 

#include "TranslatorStatistics.inline.h"

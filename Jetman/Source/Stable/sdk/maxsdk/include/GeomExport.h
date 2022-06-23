//**************************************************************************/
// Copyright (c) 1998-2006 Autodesk, Inc.
// All rights reserved.
// 
//  Use of this software is subject to the terms of the Autodesk license 
//  agreement provided at the time of installation or download, or which 
//  otherwise accompanies this software in either electronic or hard copy form.
//**************************************************************************/
#pragma once
#include "3dsmaxconfig.h"

#ifdef BUILD_GEOMETRY
#define GEOMEXPORT __declspec( dllexport )
#else
#define GEOMEXPORT __declspec( dllimport )
#endif


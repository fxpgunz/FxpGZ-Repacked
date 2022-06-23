// Copyright (c) 1998-2011 Autodesk, Inc.
// All rights reserved.
// 
//  Use of this software is subject to the terms of the Autodesk license 
//  agreement provided at the time of installation or download, or which 
//  otherwise accompanies this software in either electronic or hard copy form.

#include "3dsmaxconfig.h"

#ifdef DllExport
#undef DllExport
#endif

#ifdef IMPORTING
#define DllExport	__declspec( dllimport )
#else
#define DllExport	__declspec( dllexport )
#endif


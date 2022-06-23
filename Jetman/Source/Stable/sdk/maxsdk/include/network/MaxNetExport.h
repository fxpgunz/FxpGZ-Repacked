//**************************************************************************/
// Copyright (c) 1998-2008 Autodesk, Inc.
// All rights reserved.
// 
//  Use of this software is subject to the terms of the Autodesk license 
//  agreement provided at the time of installation or download, or which 
//  otherwise accompanies this software in either electronic or hard copy form.
//**************************************************************************/
#pragma once

#ifndef MAXNETEXPORT
	#define MAXNETEXPORT __declspec( dllimport )
#else
	#define MAXNETEXPORT __declspec( dllexport )
#endif
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

#ifdef RENDERING_API_MODULE
	#define RenderingAPIExport __declspec(dllexport)
#else
	#define RenderingAPIExport __declspec(dllimport)
#endif




















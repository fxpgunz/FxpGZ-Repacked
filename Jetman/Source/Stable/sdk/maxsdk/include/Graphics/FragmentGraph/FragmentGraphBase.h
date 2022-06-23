//
// Copyright 2013 Autodesk, Inc.  All rights reserved. 
//
//  Use of this software is subject to the terms of the Autodesk license 
//  agreement provided at the time of installation or download, or which 
//  otherwise accompanies this software in either electronic or hard copy form.
//
//

#pragma once

#include "../../maxheap.h"

#ifdef FRAGMENT_GRAPH_MODULE
#	define FragmentGraphAPI __declspec(dllexport)
#else
#	define FragmentGraphAPI __declspec(dllimport)
#endif

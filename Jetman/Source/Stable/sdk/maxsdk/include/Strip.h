//
// Copyright 2009 Autodesk, Inc.  All rights reserved. 
//
//  Use of this software is subject to the terms of the Autodesk license 
//  agreement provided at the time of installation or download, or which 
//  otherwise accompanies this software in either electronic or hard copy form.
#pragma once
#include "MaxHeap.h"
#include "maxtypes.h"
#include <WTypes.h>
#include "TabTypes.h"

//! \brief Graphics Strip Class
class Strip : public MaxHeapOperators
{
public:
	MtlID	mID;
	DWORD	smGrp;
	DWTab	v;
	DWTab	n;
	DWTab	tv;
	void AddVert(DWORD vtx)	{ v.Append(1, &vtx); }
	void AddVert(DWORD vtx, DWORD tvtx) { v.Append(1, &vtx); tv.Append(1, &tvtx); }
	void AddVertN(DWORD vtx, DWORD nor) { v.Append(1, &vtx); n.Append(1, &nor); }
	void AddVertN(DWORD vtx, DWORD tvtx, DWORD nor) { v.Append(1, &vtx); tv.Append(1, &tvtx); n.Append(1, &nor); }
};

typedef Strip* StripPtr;
typedef Tab<StripPtr> StripTab;
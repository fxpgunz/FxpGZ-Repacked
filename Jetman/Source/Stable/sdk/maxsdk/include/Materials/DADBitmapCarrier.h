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

#include "../ref.h"
#include "../plugapi.h"

#pragma warning(push)
#pragma warning(disable:4100)

// this class used for Drag/and/dropping Bitmaps
/*! \sa  Class ReferenceTarget.\n\n
\par Description:
It is used to provide drag and drop functionality for Bitmaps. See the
following global function for getting a pointer to the instances of this
class.\n\n
All methods of this class are implemented by the system. */
class DADBitmapCarrier: public ReferenceTarget {
	 MSTR name;
	 public:
	 /*! \remarks Sets the name of the bitmap carrier.
	 \par Parameters:
	 <b>MSTR \&nm</b>\n\n
	 The name to set. */
	 void SetName(const MSTR &nm) { name = nm; }
	 /*! \remarks	 Returns the name of the bitmap carrier. */
	 MSTR& GetName() { return name; }    

	 Class_ID ClassID() { return Class_ID(0,1); }
	 SClass_ID SuperClassID() { return BITMAPDAD_CLASS_ID; }     
	 void DeleteThis() { }
	 virtual void GetClassName(MSTR& s) { s = _M("DADBitmapCarrier"); } // from Animatable

	 RefResult NotifyRefChanged(const Interval& changeInt, RefTargetHandle hTarget, 
						PartID& partID, RefMessage message, BOOL propagate) { return REF_DONTCARE; }     

	 RefTargetHandle Clone(RemapDir &remap) { assert(0); return NULL; }

	 };

// Get a pointer to the BitmapCarrier (their's only two of these: one for the
// source, and one for the destination:  don't try to delete these; 
/*! \remarks This global function returns a pointer to the
<b>BitmapCarrier</b>. Note that there is only two of these: one for the source,
and one for the destination. Developers should not delete these. Sample code
using this class is available in <b>/MAXSDK/SAMPLES/MATERIALS/BMTEX.CPP</b>.
 */
CoreExport DADBitmapCarrier *GetDADBitmapCarrier();

#pragma warning(pop)

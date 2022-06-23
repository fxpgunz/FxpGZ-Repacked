//**************************************************************************/
// Copyright (c) 1998-2006 Autodesk, Inc.
// All rights reserved.
// 
//  Use of this software is subject to the terms of the Autodesk license 
//  agreement provided at the time of installation or download, or which 
//  otherwise accompanies this software in either electronic or hard copy form.
//**************************************************************************/
#pragma once

#include "maxheap.h"
#include "maxtypes.h"

/*! \sa  Class Animatable.
\par Description:
This class is used by <b>Animatable::MapKeys()</b>. It provides a method
<b>map()</b> that is used to apply the mapping required by the system to the
TimeValue passed.  */
class TimeMap: public MaxHeapOperators {
	public:
		/*! \remarks Destructor. */
		virtual ~TimeMap() {}
		/*! \remarks Implemented by the System.\n\n
		This is a time map function that takes a TimeValue and returns a
		TimeValue. A plug-in developer just calls this map method to alter the
		TimeValue.
		\par Parameters:
		<b>TimeValue t</b>\n\n
		This is the input time.
		\return  The modified time. */
		virtual TimeValue map(TimeValue t)=0;
		/*! \remarks This method is not used. */
		virtual TimeValue prevmap(TimeValue t)=0;
	};

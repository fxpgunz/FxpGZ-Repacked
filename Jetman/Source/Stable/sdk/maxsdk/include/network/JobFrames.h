//**************************************************************************/
// Copyright (c) 1998-2008 Autodesk, Inc.
// All rights reserved.
// 
//  Use of this software is subject to the terms of the Autodesk license 
//  agreement provided at the time of installation or download, or which 
//  otherwise accompanies this software in either electronic or hard copy form.
//**************************************************************************/
#pragma once

#include <WTypes.h>
#include "../maxheap.h"
#include "../maxnet_types.h"

#define NO_FRAME				0x0FFFFFFF

#define FRAME_WAITING   		0
#define FRAME_ASSIGNED  		1
#define FRAME_COMPLETE  		2

/*! \sa Class MaxNetManager, Structure HSERVER
\remarks This structure is available in release 4.0 and later only. \n\n
This structure is used by the Network Rendering API to store a jobs frame progress information.
*/
struct JOBFRAMES : public MaxHeapOperators
{
	/*! The current state of this frame which is one of the following values:
	- FRAME_WAITING \n
	The frame is waiting to be assigned and rendered.
	- FRAME_ASSIGNED  \n
	The frame is assigned to the server.
	- FRAME_COMPLETE  \n
	The frame has completed rendering.
	- NO_FRAME \n
	There are no frames to be rendered.
	*/
	char	state;
	/*! The frame number. */
	int	frame;
	/*! The handle to the server rendering this frame. */
	HSERVER	hServer;
	/*! The time it took to render this frame, in milliseconds. */
	DWORD	elapsed;
};

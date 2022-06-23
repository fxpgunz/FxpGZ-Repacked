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
#include "../strbasic.h"
#include "../maxheap.h"

/*! \sa Class MaxNetManager
\remarks This structure is available in release 4.0 and later only. \n\n
This structure is used by the Network Rendering API to store network status information. */
struct NetworkStatus : public MaxHeapOperators
{
	/*! The number of packets dropped due to buffer overflows. */
	DWORD		dropped_packets;
	/*! The number of bad formed packets. */
	DWORD		bad_packets;
	/*! The total number of TCP requests since boot time. */
	DWORD		tcprequests;
	/*! The total number of UDP requests since boot time. */
	DWORD		udprequests;
	/*! The system boot time. */
	SYSTEMTIME	boot_time;
	/*! Reserved for future use. */
	char		reserved[32];
};

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
#include "ConfigurationBlock.h"
#include "NetworkStatus.h"
#include "../maxheap.h"

#define _SERVER_INFO_VERSION 400

/*! \sa Class MaxNetManager, Structure NetworkStatus, Structure ConfigurationBlock
\remarks This structure is available in release 4.0 and later only. \n\n
This structure is used by the Network Rendering API to store information about a Server.
*/
struct ServerInfo: public MaxHeapOperators {
	/*! The size of the structure, being sizeof(ServerInfo). */
	DWORD				size;
	/*! The version information, defined by _SERVER_INFO_VERSION. */
	DWORD				version;
	/*! The total number of frames rendered. */
	DWORD				total_frames;
	/*! The total time spent rendering, in hours. */
	float				total_time;
	/*! Performance index */
	float				index;
	/*! The network system configuration data. Refer to this structure for more information. */
	ConfigurationBlock	cfg;
	/*! The network status information. */
	NetworkStatus		net_status;
	/*! Reserved for future use. */
	char				reserved[32];
};

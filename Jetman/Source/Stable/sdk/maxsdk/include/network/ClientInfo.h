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
#include "../maxheap.h"

#define _CLIENT_INFO_VERSION 400

/*! \sa Class MaxNetManager, Structure ConfigurationBlock
\remarks This structure is available in release 4.0 and later only. \n\n
This structure is used by the Network Rendering API to store information about a Client.
*/
struct ClientInfo : public MaxHeapOperators
{
	/*! The size of the structure, being sizeof(ClientInfo). */
	DWORD				size;
	/*! The version information, defined by _CLIENTINFO_VERSION. */
	DWORD				version;
	/*! The network system configuration data. Refer to this structure for more information. */
	ConfigurationBlock	cfg;
	/*! TRUE if the Client is currently controlling the queue. */
	bool				controller;
	/*! The UDP port being used for network communications. */
	short		  		udp_port;
	/*! Reserved for future use. */
	char				reserved[32];
};

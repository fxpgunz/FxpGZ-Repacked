//**************************************************************************/
// Copyright (c) 1998-2006 Autodesk, Inc.
// All rights reserved.
// 
//  Use of this software is subject to the terms of the Autodesk license 
//  agreement provided at the time of installation or download, or which 
//  otherwise accompanies this software in either electronic or hard copy form.
//**************************************************************************/
#pragma once

// Property IDs
#define PROPID_APPDATA					0x00000010
#define PROPID_EASELIST					0x00000020
#define PROPID_MULTLIST					0x00000030
#define PROPID_NOTETRACK				0x00000040
#define PROPID_CLEARCACHES				0x00000050
#define PROPID_HAS_WSM					0x00000060
#define PROPID_PSTAMP_SMALL				0x00000070
#define PROPID_PSTAMP_LARGE				0x00000071
#define PROPID_CUSTATTRIB				0x00000072
#define PROPID_HARDWARE_MATERIAL		0x00000073
#define PROPID_SIMPLE_MATERIAL			0x00000074
#define PROPID_PSTAMP_TINY				0x00000078
#define PROPID_SVDATA					0x00000080
#define PROPID_FORCE_RENDER_MESH_COPY	0x000000100
#define PROPID_EVAL_STEPSIZE_BUG_FIXED	0x1000
#define PROPID_USER						0x0000FFFF

// Values above PROPID_USER can be used by plug-ins. 
// Note: that a plug-in should only put user defined properties on it's
// own list. So IDs only have to be unique within a plug-in. If a plug-in
// needs to attach data to another object, it can do so via APP_DATA.
